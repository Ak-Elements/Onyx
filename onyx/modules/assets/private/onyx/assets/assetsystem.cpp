#include <onyx/assets/assetsystem.h>

#include <onyx/thread/async/asynctask.h>

#include <onyx/assets/assetserializer.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/filesystem/textdeserializer.h>
#include <onyx/filesystem/textserializer.h>

namespace onyx::assets {
HashMap< StringId32, InplaceFunction< Reference< AssetInterface >( IEngine& ) > > AssetSystem::s_registeredAssets = {};
HashMap< StringId32, UniquePtr< IAssetSerializer > > AssetSystem::s_registeredSerializer = {};
HashMap< StringView, AssetType > AssetSystem::s_extensionToAssetType = {};

namespace {
constexpr StringView AssetMetaExtension = ".meta";
}

namespace {
Optional< AssetMetaData > tryReadAssetMetadata( const FilePath& path ) {
    String metadataFileContent;
    if( file_system::OnyxFile::readAll( file_system::path::getFullPath( path ), metadataFileContent ) == false ) {
        return std::nullopt;
    }

    file_system::TextDeserializer deserializer( metadataFileContent );
    AssetMetaData metaData;
    deserializer.read( metaData );
    return metaData;
}

Optional< AssetMetaData > createAssetMetadata( const FilePath& metadataPath, const FilePath& assetPath ) {
    AssetMetaData metadata;
    metadata.Id = AssetId( Guid64Generator::getGuid() );
    metadata.Path = assetPath;
    metadata.Format = AssetFormat::Binary;
    metadata.Version = 0;

    file_system::TextSerializer serializer;
    serializer.write( metadata );
    const String& serlialized = serializer.toString();
    if( serlialized.empty() )
        return std::nullopt;

    if( file_system::OnyxFile::writeAll( file_system::path::getFullPath( metadataPath ), serlialized ) ) {
        return metadata;
    }

    return std::nullopt;
}

bool getAllAssetMetaData( HashMap< AssetId, AssetMetaData >& outAssetsMetaData ) {
    // async creation of asset meta data
    using RecursiveDirectoryIterator = std::filesystem::recursive_directory_iterator;

    HashSet< FilePath > metadataFiles;
    HashSet< FilePath > assetFiles;
    for( auto& [ mountIdentifier, mountPoint ] : file_system::path::getMountPoints() ) {
        if( mountIdentifier == file_system::path::TmpMountPointId )
            continue;

        for( const std::filesystem::directory_entry& entry : RecursiveDirectoryIterator( mountPoint.Path ) ) {
            if( entry.is_regular_file() == false ) {
                continue;
            }

            const FilePath path = entry.path();
            const FilePath relativePath = path.lexically_relative( mountPoint.Path );
            const FilePath mountPointPath = mountPoint.Prefix / relativePath;
            const String extension = path.extension().generic_string();

            if( ignoreCaseEqual( extension, ".ometa" ) ) {
                continue;
            }

            if( ignoreCaseEqual( extension, AssetMetaExtension ) ) {
                metadataFiles.emplace( mountPointPath );
                continue;
            }

            assetFiles.emplace( mountPointPath );
        }
    }

    for( const FilePath& assetPath : assetFiles ) {
        // metadata files share the same name but with a . and .meta extension
        // the dot at the start will mark them as hidden in linux
        const FilePath metadataPath = FilePath( assetPath )
                                          .replace_filename( "." + assetPath.filename().generic_string().append(
                                                                       AssetMetaExtension ) );

        auto it = std::ranges::find_if( metadataFiles, [ & ]( const FilePath& path ) {
            return ignoreCaseEqual( path.generic_string(), metadataPath.generic_string() );
        } );
        Optional< AssetMetaData > metadataOptional;
        if( it == metadataFiles.end() ) {
            metadataOptional = createAssetMetadata( metadataPath, assetPath );
        } else {
            metadataOptional = tryReadAssetMetadata( metadataPath );
            metadataFiles.erase( it );
        }

        if( metadataOptional.has_value() ) {
            AssetMetaData& metadata = metadataOptional.value();
            metadata.Path = assetPath;
            outAssetsMetaData.try_emplace( metadata.Id, metadata );
        } else {
            ONYX_LOG_ERROR( "Failed loading asset meta for {}.", assetPath );
        }
    }

    // cleanup / delete metadata files that are missing assets
    for( const FilePath& metadataPath : metadataFiles ) {
        std::filesystem::remove( file_system::path::getFullPath( metadataPath ) );
    }

    return true;
}

} // namespace

AssetSystem::AssetSystem( IEngine& engine )
    : m_engine( &engine ) {
    if( getAllAssetMetaData( m_assetsMetaData ) == false ) {
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

    if( assetIt == m_assetsMetaData.end() ) {
        ONYX_LOG_WARNING( "Missing asset with id:{}.", id.get() );
        return;
    }

    const AssetMetaData& metaData = assetIt->second;
    if( metaData.Handle != InvalidIndex64 ) {
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

AssetId AssetSystem::resolveAssetId( const FilePath& path ) const {
    // FilePath resolvedPath = file_system::path::convertToMountPath( path );
    for( auto&& [ id, metadata ] : m_assetsMetaData ) {
        if( ignoreCaseEqual( metadata.Path.generic_string(), path.generic_string() ) )
            return id;
    }

    return AssetId::invalid();
}
} // namespace onyx::assets
