#include <onyx/assets/assetsystem.h>

#include <onyx/thread/async/asynctask.h>

#include <onyx/assets/assetserializer.h>

namespace Onyx::Assets
{
    HashMap <StringId32, InplaceFunction<Reference<AssetInterface>()>> AssetSystem::registeredAssets = {};
    HashMap <StringId32, UniquePtr<AssetSerializer>> AssetSystem::registeredSerializer = {};
    HashMap<StringView, AssetType> AssetSystem::extensionToAssetType = {};

    namespace
    {
        bool GetAllAssetMetaData(HashMap<AssetId, AssetMetaData>& outAssetsMetaData)
        {
            // async creation of asset meta data
            using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

            for (auto& [mountIdentifier, mountPoint] : FileSystem::Path::GetMountPoints())
            {
                if (mountIdentifier == FileSystem::Path::TMP_MOUNT_POINT_ID)
                    continue;

                for (const std::filesystem::directory_entry& entry : recursive_directory_iterator(mountPoint.Path))
                {
                    if (entry.is_regular_file())
                    {
                        // skip meta files - in the future this should only parse meta files and disregard other files
                        // but only graphs currently have meta files
                        if (entry.path().extension().compare(".ometa") == 0)
                        {
                            continue;
                        }

                        const onyxU32 version = 0;

                        AssetMetaData metaData;

                        String assetPath = entry.path().lexically_relative(mountPoint.Path).generic_string();
                        ToLower(assetPath);

                        assetPath = mountPoint.Prefix + assetPath;
                        metaData.Id = AssetId(FileSystem::Filepath(assetPath));
                        metaData.Path = entry.path();

                        metaData.Version = version;

                        String extension = metaData.Path.extension().generic_string();
                        if (extension.empty() == false)
                        {
                            extension = extension.substr(1); // ignore .
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
        if (GetAllAssetMetaData(m_AssetsMetaData) == false)
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
                const UniquePtr<AssetSerializer>& serializer = registeredSerializer.at(StringId32(static_cast<onyxU32>(metaData.Type)));
                m_IOHandler.RequestLoad(metaData, reloadAsset, serializer);
            }
        }
    }
}
