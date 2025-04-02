#include <onyx/assets/assetsystem.h>

#include <onyx/thread/async/asynctask.h>

#include <onyx/assets/assetserializer.h>

namespace Onyx::Assets
{
    HashMap <onyxU32, InplaceFunction<Reference<AssetInterface>()>> AssetSystem::registeredAssets = {};
    HashMap <onyxU32, UniquePtr<AssetSerializer>> AssetSystem::registeredSerializer = {};
    HashMap<onyxU32, AssetType> AssetSystem::extensionToAssetType = {};

    namespace
    {
        bool GetAllAssetMetaData(const HashMap<onyxU32, AssetType>& assetExtensionsToAssetType, HashMap<AssetId, AssetMetaData>& outAssetsMetaData)
        {

            // async creation of asset meta data
            using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

            for (auto& [mountIdentifier, mountPoint] : FileSystem::Path::GetMountPoints())
            {
                if (mountIdentifier == Hash::FNV1aHash32("tmp:/"))
                    continue;

                for (const std::filesystem::directory_entry& entry : recursive_directory_iterator(mountPoint.Path))
                {
                    if (entry.is_regular_file())
                    {
                        const onyxU32 version = 0;

                        AssetMetaData metaData;

                        String assetPath = entry.path().lexically_relative(mountPoint.Path).string();
                        std::ranges::replace(assetPath, '\\', '/');

                        assetPath = mountPoint.Prefix + assetPath;
                        metaData.Id = AssetId(FileSystem::Filepath(assetPath));
                        metaData.Path = entry.path();
                        metaData.Name = metaData.Path.stem().string();
                        metaData.Version = version;

                        const String& extension = metaData.Path.extension().string().substr(1); // ignore .
                        onyxU32 extensionHash = Hash::FNV1aHash32(extension);

                        if (assetExtensionsToAssetType.contains(extensionHash))
                        {
                            metaData.Type = assetExtensionsToAssetType.at(extensionHash);
                        }
                        else
                        {
                            metaData.Type = AssetType::Invalid;
                        }

                        outAssetsMetaData.try_emplace(metaData.Id, metaData);
                    }
                }
            }

            return true;
        }
    }

    AssetSystem::AssetSystem() = default;
    AssetSystem::~AssetSystem() = default;

    void AssetSystem::Init()
    {
        if (GetAllAssetMetaData(extensionToAssetType, m_AssetsMetaData))
        {
            ONYX_LOG_FATAL("Failed loading asset meta data");
            return;
        }
    }

    void AssetSystem::Shutdown()
    {
        m_AssetsMetaData.clear();
        m_LoadedAssets.clear();
    }

    void AssetSystem::ReloadAsset(AssetId id)
    {
        const auto assetIt = m_AssetsMetaData.find(id);

        if (assetIt == m_AssetsMetaData.end())
        {
            ONYX_LOG_WARNING("Missing asset with id:{}.", id.Get());
            return;
        }

        const AssetMetaData& metaData = assetIt->second;
        if (metaData.Handle != INVALID_INDEX_64)
        {
            Reference<AssetInterface>& reloadAsset = m_LoadedAssets[metaData.Handle];
            reloadAsset->SetState(AssetState::Loading);
            {
                std::lock_guard lock(m_Mutex);
                const UniquePtr<AssetSerializer>& serializer = registeredSerializer.at(static_cast<onyxU32>(metaData.Type));
                m_IOHandler.RequestLoad(metaData, reloadAsset, serializer);
            }
        }
    }
}
