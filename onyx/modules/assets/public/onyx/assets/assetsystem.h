#pragma once

#include <onyx/engine/enginesystem.h>

#include <onyx/assets/asset.h>
#include <onyx/assets/assetloader.h>

#include <onyx/filesystem/path.h>
#include <onyx/log/logger.h>
#include <onyx/string/compiletimestring.h>

namespace Onyx::Assets
{
    struct AssetSerializer;

    struct AssetId;

    class AssetSystem : public IEngineSystem
    {
    public:
        AssetSystem();
        ~AssetSystem() override;

        void Init();
        void Shutdown();

        template <typename T>
        Reference<T> Create()
        {
            constexpr onyxU32 assetTypeHash = TypeHash<T>();
            const CreateAssetFunction& createFunctor = registeredAssets.at(assetTypeHash);

            Reference<T> newAsset = createFunctor();
            newAsset->SetState(AssetState::Loaded);

            return newAsset;
        }

        template <typename T> requires std::is_base_of_v<AssetInterface, T>
        static DynamicArray<StringView> GetExtensions()
        {
            constexpr AssetType type = static_cast<AssetType>(TypeHash<T>());
            return GetExtensions(type);
        }

        static DynamicArray<StringView> GetExtensions(AssetType type)
        {
            DynamicArray<StringView> extensions;
            for (auto&& [extension, assetType] : extensionToAssetType)
            {
                if (assetType == type)
                {
                    extensions.emplace_back(extension);
                }
            }
            return extensions;
        }

        const AssetMetaData& GetAssetMeta(AssetId id) const
        {
            //ONYX_ASSERT(m_AssetsMetaData.contains(id), "Asset with this ID(0x{:x}) is unknown", id.Get());
            return m_AssetsMetaData.at(id);
        }

        DynamicArray<AssetMetaData> GetAvailableAssets(AssetType type) const
        {
            DynamicArray<AssetMetaData> availableAssets;
            availableAssets.reserve(m_AssetsMetaData.size());

            for (const AssetMetaData& assetMeta : (m_AssetsMetaData | std::views::values))
            {
                if ((type != AssetType::Invalid) && (extensionToAssetType[assetMeta.Extension] != type))
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
            constexpr AssetType type = static_cast<AssetType>(TypeHash<T>());

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
            constexpr AssetId id = static_cast<AssetId>(Hash::FNV1aHash64(path));
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
        bool SaveAsset(AssetId id);

        template <typename T>
        bool SaveAssetAs(const FileSystem::Filepath& newPath, const Reference<T>& asset);
#endif

        template <typename AssetT, IAssetSerializer SerializerT, typename... Args>
        static constexpr bool Register(Args&&... args)
        {
            //static_assert(is_specialization_of_v<Asset, T>, "Class T has to inherit from Asset");
            constexpr onyxU32 assetTypeHash = TypeHash<AssetT>();
            ONYX_ASSERT(registeredAssets.contains(assetTypeHash) == false, "Asset with that type is already registered.");
            registeredAssets[assetTypeHash] = []() { return Reference<AssetT>::Create(); };
            registeredSerializer[assetTypeHash] = MakeUnique<SerializerT>(std::forward<Args>(args)...);

            std::apply([](auto&&... extension)
                { (
                    ( extensionToAssetType[extension] = static_cast<AssetType>(assetTypeHash)),
                    ...);
                },
                SerializerT::Extensions);

            return true;
        }

        void ReloadAsset(AssetId id);

    private:
        std::mutex m_Mutex;
        AssetIOHandler m_IOHandler;

        HashMap<AssetId, AssetMetaData> m_AssetsMetaData;
        DynamicArray<Reference<AssetInterface>> m_LoadedAssets;

        using CreateAssetFunction = InplaceFunction<Reference<AssetInterface>()>;
        static HashMap<onyxU32, CreateAssetFunction> registeredAssets;
        static HashMap<onyxU32, UniquePtr<AssetSerializer>> registeredSerializer;
        static HashMap<String, AssetType> extensionToAssetType;
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

        constexpr onyxU32 assetTypeHash = TypeHash<T>();
        metaData.Type = static_cast<AssetType>(assetTypeHash);

        if ((metaData.Handle != INVALID_INDEX_64) && forceLoad)
        {
            Reference<AssetInterface>& reloadAsset = m_LoadedAssets[metaData.Handle];
            {
                std::lock_guard lock(m_Mutex);
                const UniquePtr<AssetSerializer>& serializer = registeredSerializer.at(assetTypeHash);
                m_IOHandler.RequestLoad(metaData, reloadAsset, serializer);
            }
            outAsset = reloadAsset;
            return true;
        }

        const CreateAssetFunction& createFunctor = registeredAssets.at(assetTypeHash);
        const UniquePtr<AssetSerializer>& serializer = registeredSerializer.at(assetTypeHash);

        Reference<AssetInterface> newAsset = createFunctor();
        newAsset->SetId(id);
        newAsset->SetState(AssetState::Loading);

        {
            std::lock_guard lock(m_Mutex);
            if (metaData.Handle == INVALID_INDEX_64)
            {
                metaData.Handle = m_LoadedAssets.size();
                newAsset = m_LoadedAssets.emplace_back(std::move(newAsset));
                m_IOHandler.RequestLoad(metaData, newAsset, serializer);
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

        constexpr onyxU32 assetTypeHash = TypeHash<T>();
        const CreateAssetFunction& createFunctor = registeredAssets.at(assetTypeHash);
        const UniquePtr<AssetSerializer>& serializer = registeredSerializer.at(assetTypeHash);

        Reference<AssetInterface> assetCopy = createFunctor();
        assetCopy->SetId(id);
        assetCopy->SetState(AssetState::Loading);
        m_IOHandler.RequestLoad(metaData, assetCopy, serializer);

        outAssetReference = assetCopy;
        return true;
    }

    template <typename T>
    bool AssetSystem::SaveAsset(AssetId id)
    {
        const auto assetIt = m_AssetsMetaData.find(id);

        if (assetIt == m_AssetsMetaData.end())
        {
            ONYX_LOG_ERROR("Missing asset with id:{}.", id.Get());
            return false;
        }

        const AssetMetaData& metaData = assetIt->second;
        if (metaData.Handle == INVALID_INDEX_64)
        {
            return false;
        }

        const AssetType assetTypeHash = metaData.Type;
        const UniquePtr<AssetSerializer>& serializer = registeredSerializer.at((onyxU32)assetTypeHash);

        m_IOHandler.RequestSave(metaData, m_LoadedAssets.at(metaData.Handle), serializer);
        return true;
    }

    template <typename T>
    bool AssetSystem::SaveAssetAs(const FileSystem::Filepath& newPath, const Reference<T>& asset)
    {
        constexpr onyxU32 assetTypeHash = TypeHash<T>();
        AssetId newAssetId(newPath);
        AssetMetaData metaData{ newPath.stem().string(), newPath.extension().string().substr(1), newPath, newAssetId, static_cast<AssetType>(assetTypeHash), INVALID_INDEX_64, 0};
        const UniquePtr<AssetSerializer>& serializer = registeredSerializer.at(assetTypeHash);

        //TODO: should only add it IF we finish saving
        m_AssetsMetaData.try_emplace(newAssetId, metaData);
        
        m_IOHandler.RequestSave(metaData, asset, serializer);
        return true;
    }

#endif
}
