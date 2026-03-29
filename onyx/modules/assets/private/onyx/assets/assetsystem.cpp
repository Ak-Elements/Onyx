#include <onyx/assets/assetsystem.h>

#include <onyx/thread/async/asynctask.h>

#include <onyx/assets/assetserializer.h>

namespace onyx::assets {
HashMap< StringId32, InplaceFunction< Reference< AssetInterface >( IEngine& ) > > AssetSystem::s_registeredAssets = {};
HashMap< StringId32, UniquePtr< IAssetSerializer > > AssetSystem::s_registeredSerializer = {};
HashMap< StringView, AssetType > AssetSystem::s_extensionToAssetType = {};

namespace {
bool GetAllAssetMetaData( HashMap< AssetId, AssetMetaData >& outAssetsMetaData ) {
    // async creation of asset meta data
    using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

    for ( auto& [ mountIdentifier, mountPoint ] : file_system::path::getMountPoints() ) {
        if ( mountIdentifier == file_system::path::TmpMountPointId )
            continue;

        for ( const std::filesystem::directory_entry& entry : recursive_directory_iterator( mountPoint.Path ) ) {
            if ( entry.is_regular_file() ) {
                // skip meta files - in the future this should only parse meta files and disregard other files
                // but only graphs currently have meta files
                if ( entry.path().extension().compare( ".ometa" ) == 0 ) {
                    continue;
                }

                const uint32_t version = 0;

                AssetMetaData metaData;

                String assetPath = entry.path().lexically_relative( mountPoint.Path ).generic_string();
                toLower( assetPath );

                assetPath = mountPoint.Prefix + assetPath;
                metaData.Id = AssetId( FilePath( assetPath ) );
                metaData.Path = assetPath;

                metaData.Version = version;

                String extension = metaData.Path.extension().generic_string();
                if ( extension.empty() == false ) {
                    extension = extension.substr( 1 ); // ignore .
                }

                outAssetsMetaData.try_emplace( metaData.Id, metaData );
            }
        }
    }

    return true;
}
} // namespace

AssetSystem::AssetSystem( IEngine& engine )
    : m_engine( &engine ) {
    if ( GetAllAssetMetaData( m_assetsMetaData ) == false ) {
        ONYX_LOG_FATAL( "Failed loading asset meta data" );
        return;
    }
}

AssetSystem::~AssetSystem() {
    m_assetsMetaData.clear();
    m_loadedAssets.clear();
}

void AssetSystem::reloadAsset( AssetId id ) {
    const auto assetIt = m_assetsMetaData.find( id );

    if ( assetIt == m_assetsMetaData.end() ) {
        ONYX_LOG_WARNING( "Missing asset with id:{}.", id.get() );
        return;
    }

    const AssetMetaData& metaData = assetIt->second;
    if ( metaData.Handle != InvalidIndex64 ) {
        AssetHandle< AssetInterface >& reloadAsset = m_loadedAssets[ metaData.Handle ];
        reloadAsset->setState( AssetState::Loading );
        {
            std::lock_guard lock( m_mutex );
            const UniquePtr< IAssetSerializer >& serializer = s_registeredSerializer.at(
                StringId32( static_cast< uint32_t >( metaData.Type ) ) );
            m_ioHandler.requestLoad( metaData, reloadAsset, serializer, m_engine );
        }
    }
}
} // namespace onyx::assets
