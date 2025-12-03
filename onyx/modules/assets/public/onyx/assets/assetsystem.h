#pragma once

#include <onyx/engine/enginesystem.h>

#include <onyx/assets/asset.h>
#include <onyx/assets/assetloader.h>

#include <onyx/filesystem/path.h>
#include <onyx/log/logger.h>

namespace Onyx
{
    class IEngine;
}

namespace Onyx::Assets
{
    struct IAssetSerializer;

    struct AssetId;

    class AssetSystem : public IEngineSystem
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Assets::AssetModule";
        StringId32 GetTypeId() const override { return TypeId; }

        AssetSystem(IEngine& engine);
        ~AssetSystem() override;

        template <typename T>
        Reference<T> Create()
        {
            const InplaceFunction<Reference<AssetInterface>(IEngine&)>& createFunctor = s_RegisteredAssets.at(T::TypeId);
            Reference<T> newAsset = createFunctor(*m_Engine);
            newAsset->SetState(AssetState::Loaded);

            return newAsset;
        }

        template <typename T> requires std::is_base_of_v<AssetInterface, T>
        static DynamicArray<StringView> GetExtensions()
        {
            constexpr AssetType type = static_cast<AssetType>(T::TypeId.GetId());
            return GetExtensions(type);
        }

        static DynamicArray<StringView> GetExtensions(AssetType type)
        {
            DynamicArray<StringView> extensions;
            for (auto&& [extension, assetType] : s_ExtensionToAssetType)
            {
                if (assetType == type)
                {
                    extensions.emplace_back(extension);
                }
            }
            return extensions;
        }

        Optional<const AssetMetaData*> TryGetAssetMeta(AssetId id) const
        {
            auto it = m_AssetsMetaData.find(id);
            if (it == m_AssetsMetaData.end())
                return std::nullopt;

            return &it->second;
        }

        const AssetMetaData& GetAssetMeta(AssetId id) const
        {
            return m_AssetsMetaData.at(id);
        }

        DynamicArray<AssetMetaData> GetAvailableAssets(AssetType type) const
        {
            DynamicArray<AssetMetaData> availableAssets;
            availableAssets.reserve(m_AssetsMetaData.size());

            for (const AssetMetaData& assetMeta : (m_AssetsMetaData | std::views::values))
            {
                String extension(assetMeta.GetExtension());
                if ((type != AssetType::Invalid) && (s_ExtensionToAssetType[extension] != type))
                {
                    continue;
                }

                availableAssets.push_back(assetMeta);
            }

            return availableAssets;
        }

        template <typename T> requires std::is_base_of_v<AssetInterface, T>
        DynamicArray<AssetMetaData> GetAvailableAssets()
        {
            constexpr AssetType type = static_cast<AssetType>(T::TypeId.GetId());

            DynamicArray<AssetMetaData> availableAssets;
            availableAssets.reserve(m_AssetsMetaData.size());

            for (const AssetMetaData& assetMeta : (m_AssetsMetaData | std::views::values))
            {
                if (assetMeta.Type == type)
                {
                    availableAssets.push_back(assetMeta);
                }
            }

            return availableAssets;
        }

        template <typename T, onyxU64 N>
        constexpr bool GetAsset(const CompileTimeString<N>& path, Reference<T>& outAsset)
        {
            AssetId id = static_cast<AssetId>(Hash::FNV1aHash(path, 0));
            return GetAsset(id, outAsset, false);
        }

        template <typename T>
        bool GetAsset(AssetId id, Reference<T>& outAsset)
        {
            return GetAsset(id, outAsset, false);
        }

        template <typename T>
        bool GetAsset(AssetId id, Reference<T>& outAsset, bool forceLoad);

        template <typename T>
        bool GetAsset(AssetId id, Reference<T>& outAssetReference) const;

#if ONYX_IS_EDITOR
        // Used to load copies of assets for working copies in editors (e.g.: Scene / Nodegraph)
        template <typename T>
        bool GetAssetUnmanaged(AssetId id, Reference<T>& outAssetReference);

        template <typename T>
        bool SaveAsset(const Reference<T>& asset);

        template <typename T>
        bool SaveAssetAs(const FileSystem::Filepath& newPath, const Reference<T>& asset);
#endif

        template <typename AssetT> requires std::is_base_of_v<AssetInterface, AssetT >
        static constexpr bool Register()
        {
            constexpr StringId32 typeId(AssetT::TypeId);
            ONYX_ASSERT(s_RegisteredAssets.contains(typeId) == false, "Asset with that type is already registered.");
            if constexpr (std::is_abstract_v<AssetT>)
            {
                s_RegisteredAssets[typeId] = [](IEngine& engine) -> Reference<AssetInterface> { return AssetT::Create(engine); };
            }
            else
            {
                s_RegisteredAssets[typeId] = [](IEngine&) -> Reference<AssetInterface> { return Reference<AssetT>::Create(); };
            }

            return true;
        }

        template <typename SerializerT, typename... Args>
        static constexpr bool Register(Args&&... args)
        {
            ONYX_ASSERT(s_RegisteredSerializer.contains(SerializerT::AssetT::TypeId) == false, "Serializer with that type is already registered.");
            s_RegisteredSerializer[SerializerT::AssetT::TypeId] = MakeUnique<SerializerT>(std::forward<Args>(args)...);

            for (StringView extension : SerializerT::Extensions)
            {
                s_ExtensionToAssetType[extension] = static_cast<AssetType>(SerializerT::AssetT::TypeId.GetId());
            }

            return true;
        }

        void ReloadAsset(AssetId id);

    private:
        std::mutex m_Mutex;
        AssetIOHandler m_IOHandler;

        HashMap<AssetId, AssetMetaData> m_AssetsMetaData;
        DynamicArray<Reference<AssetInterface>> m_LoadedAssets;

        static HashMap<StringId32, InplaceFunction<Reference<AssetInterface>(IEngine&)>> s_RegisteredAssets;
        static HashMap<StringId32, UniquePtr<IAssetSerializer>> s_RegisteredSerializer;
        static HashMap<StringView, AssetType> s_ExtensionToAssetType;

        IEngine* m_Engine = nullptr;
    };

    template <typename T>
    bool AssetSystem::GetAsset(AssetId id, Reference<T>& outAsset, bool forceLoad)
    {
        auto assetIt = m_AssetsMetaData.find(id);
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        if (assetIt == m_AssetsMetaData.end())
        {
            ONYX_LOG_ERROR("Missing asset with id:{}.", id.Get());
            return false;
        }
#endif

        AssetMetaData& metaData = assetIt->second;
        if ((metaData.Handle != INVALID_INDEX_64) && (forceLoad == false))
        {
            outAsset = m_LoadedAssets[metaData.Handle];
            return true;
        }

        constexpr StringId32 assetTypeHash = T::TypeId;
        metaData.Type = static_cast<AssetType>(assetTypeHash.GetId());

        if constexpr (HasAssetFormat<T>)
            metaData.Format = T::Format;
        
        if ((metaData.Handle != INVALID_INDEX_64) && forceLoad)
        {
            Reference<AssetInterface>& reloadAsset = m_LoadedAssets[metaData.Handle];
            {
                std::lock_guard lock(m_Mutex);
                const UniquePtr<IAssetSerializer>& serializer = s_RegisteredSerializer.at(assetTypeHash);
                m_IOHandler.RequestLoad(metaData, reloadAsset, serializer, m_Engine);
            }
            outAsset = reloadAsset;
            return true;
        }

        const InplaceFunction<Reference<AssetInterface>(IEngine&)>& createFunctor = s_RegisteredAssets.at(assetTypeHash);
        const UniquePtr<IAssetSerializer>& serializer = s_RegisteredSerializer.at(assetTypeHash);

        Reference<AssetInterface> newAsset = createFunctor(*m_Engine);
        newAsset->SetId(id);
        newAsset->SetState(AssetState::Loading);

        {
            std::lock_guard lock(m_Mutex);
            if (metaData.Handle == INVALID_INDEX_64)
            {
                metaData.Handle = static_cast<onyxS64>(m_LoadedAssets.size());
                newAsset = m_LoadedAssets.emplace_back(std::move(newAsset));
                m_IOHandler.RequestLoad(metaData, newAsset, serializer, m_Engine);
            }
        }

        outAsset = newAsset;
        return true;
    }

    template <typename T>
    bool AssetSystem::GetAsset(AssetId id, Reference<T>& outAssetReference) const
    {
        const auto assetIt = m_AssetsMetaData.find(id);
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        if (assetIt == m_AssetsMetaData.end())
        {
            ONYX_LOG_ERROR("Missing asset with id:{}.", id.Get());
            return false;
        }
#endif

        const AssetMetaData& metaData = assetIt->second;
        if (metaData.Handle == INVALID_INDEX_64)
        {
            return false;
        }

        outAssetReference = m_LoadedAssets[metaData.Handle];
        return true;
    }

#if ONYX_IS_EDITOR
    template <typename T>
    bool AssetSystem::GetAssetUnmanaged(AssetId id, Reference<T>& outAssetReference)
    {
        const auto assetIt = m_AssetsMetaData.find(id);

        if (assetIt == m_AssetsMetaData.end())
        {
            ONYX_LOG_ERROR("Missing asset with id:{}.", id.Get());
            return false;
        }

        const AssetMetaData& metaData = assetIt->second;

        constexpr StringId32 assetTypeHash = T::TypeId;
        const InplaceFunction<Reference<AssetInterface>(IEngine&)>& createFunctor = s_RegisteredAssets.at(assetTypeHash);
        const UniquePtr<IAssetSerializer>& serializer = s_RegisteredSerializer.at(assetTypeHash);

        Reference<AssetInterface> assetCopy = createFunctor(*m_Engine);
        assetCopy->SetId(id);
        assetCopy->SetState(AssetState::Loading);
        m_IOHandler.RequestLoad(metaData, assetCopy, serializer, m_Engine);

        outAssetReference = assetCopy;
        return true;
    }

    template <typename T>
    bool AssetSystem::SaveAsset(const Reference<T>& asset)
    {
        AssetId assetId = asset->GetId();
        const auto assetIt = m_AssetsMetaData.find(assetId);

        if (assetIt == m_AssetsMetaData.end())
        {
            ONYX_LOG_ERROR("Missing asset with id:{}.", assetId.Get());
            return false;
        }

        const AssetMetaData& metaData = assetIt->second;
        if (metaData.Handle == INVALID_INDEX_64)
        {
            return false;
        }

        constexpr StringId32 assetTypeHash = T::TypeId;
        const UniquePtr<IAssetSerializer>& serializer = s_RegisteredSerializer.at((onyxU32)assetTypeHash);
        m_IOHandler.RequestSave(metaData, asset, serializer, m_Engine);
        return true;
    }

    template <typename T>
    bool AssetSystem::SaveAssetAs(const FileSystem::Filepath& newPath, const Reference<T>& asset)
    {
        constexpr StringId32 assetTypeHash = T::TypeId;
        AssetId newAssetId(newPath);
        AssetMetaData metaData{ .Path = newPath, .Id = newAssetId, .Type = static_cast<AssetType>(assetTypeHash.GetId()) };
        const UniquePtr<IAssetSerializer>& serializer = s_RegisteredSerializer.at(assetTypeHash);

        //TODO: should only add it IF we finish saving
        m_AssetsMetaData.try_emplace(newAssetId, metaData);
        
        m_IOHandler.RequestSave(metaData, asset, serializer, m_Engine);
        return true;
    }
#endif
}
