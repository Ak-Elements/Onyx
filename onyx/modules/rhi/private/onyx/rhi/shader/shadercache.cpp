#include <onyx/rhi/shader/shadercache.h>

#include <onyx/hash.h>
#include <onyx/rhi/shader/shadercompiler.h>
#include <onyx/rhi/vulkan/graphicsapi.h>

#include <onyx/filesystem/filestream.h>
#include <onyx/filesystem/onyxfile.h>

namespace onyx::rhi {
ShaderCache::ShaderCache( GraphicsSystem& graphicsSystem )
    : m_graphicsSystem( graphicsSystem ) {
#if !ONYX_IS_RETAIL
    const FilePath shaderCacheDirectory = file_system::path::getFullPath( ShaderCachePath );
    if( file_system::path::exists( shaderCacheDirectory ) == false ) {
        file_system::path::createDirectory( shaderCacheDirectory );
    }

    for( const FilePath& shaderDirectory : getShaderDirectories() ) {
        m_directoryWatcher.addPath( shaderDirectory, true );

        file_system::path::enumerateFiles( shaderDirectory, [ & ]( const FilePath& path ) {
            if( path.has_filename() == false )
                return true;

            if( path.extension() != ".h" )
                return true;

            // TODO: enqueue in a different thread
            FilePath mountPointPath = file_system::path::convertToMountPath( path );

            String content;
            if( file_system::OnyxFile::ReadAll( path, content ) == false ) {
                ONYX_LOG_ERROR( "Failed reading shader file. ({})", path );
                return true;
            }

            uint64_t fileHash = hash::fnV1aHash< uint64_t >( mountPointPath.generic_string() );
            uint64_t shaderFileHash = hash::fnV1aHash< uint64_t >( content, fileHash );

            m_includesCache[ fileHash ] = {
                .Path = mountPointPath,
                .ShaderHash = shaderFileHash,
            };
            return true;
        } );
    }

    m_directoryWatcher.onFileChanged.Connect< &ShaderCache::onFileChanged >( this );
#endif
}

bool ShaderCache::getOrLoadShader( const FilePath& shaderPath, Reference< Shader >& outShader ) {
    // TODO: Hash should be hash of properties not just the path
    uint64_t fileHash = hash::fnV1aHash< uint64_t >( shaderPath.generic_string() );

    auto entryIt = m_cache.find( fileHash );
    bool hasEntry = entryIt != m_cache.end();

    if( hasEntry ) {
        outShader = entryIt->second.Shader;
    } else {
        entryIt = m_cache.emplace( fileHash, outShader ).first;
    }

    ShaderCacheEntry& entry = entryIt->second;

    // we have that shader already cached
    FilePath absoluteFilepath = file_system::path::getFullPath( shaderPath );
    file_system::OnyxFile shaderSource = file_system::OnyxFile( absoluteFilepath );
    String shaderCode;
    if( file_system::OnyxFile::ReadAll( absoluteFilepath, shaderCode ) == false ) {
        ONYX_LOG_ERROR( "Missing shader file. ({})", shaderPath );
        return false;
    }

    uint64_t shaderHash = hash::fnV1aHash< uint64_t >( shaderCode, fileHash );

    // cached version is still valid we can return it
    // TODO: This is not correct when doing a reload from a header change
    StringView path = format::format( "{}/{:x}.ocache", ShaderCachePath, shaderHash );
    const FilePath& diskShaderCachePath = file_system::path::getFullPath( path );
    if( hasEntry ) {
        if( isEntryUpToDate( entry, shaderHash ) ) {
            // outEntry = entry;
            outShader = entry.Shader;
            return true;
        }
    } else // get from shader cache
    {
        // check shader disk cache before recompiling / reloading shaders
        if( file_system::path::exists( diskShaderCachePath ) ) {
            bool hasLoaded = loadCacheFromDisk( diskShaderCachePath, shaderPath, entry );
            if( hasLoaded && isEntryUpToDate( entry, shaderHash ) ) {
                m_cache[ fileHash ] = entry;
                outShader = m_cache[ fileHash ].Shader;
                return true;
            }
        }
    }

    // ShaderReflectionInfo reflectionInfo;
    // InplaceArray< ShaderStageCacheEntry, MAX_SHADER_STAGES > stages;
    shader_compiler::compile( m_graphicsSystem, shaderPath, shaderCode, *entry.Shader );

    // for( uint8_t i = enums::toIntegral( ShaderStage::Vertex ); i < enums::toIntegral( ShaderStage::Count ); ++i ) {
    //     ShaderStage stage = static_cast< ShaderStage >( i );
    //     const DynamicArray< uint32_t >& stageByteCode = stages[ i ].ByteCode;
    //
    //     if( stageByteCode.empty() ) {
    //         if( !entry.Stages[ i ].ByteCode.empty() ) {
    //             entry.Stages[ i ].ByteCode.clear();
    //             entry.Stages[ i ].IncludeHashes.clear();
    //             entry.Shader->removeStage( stage );
    //         }
    //         continue;
    //     }
    //
    //     entry.Shader->addStage( m_graphicsSystem, stage, stageByteCode );
    //     entry.Stages[ i ] = stages[ i ];
    // }

    // Create descriptors for shader stage
    entry.Shader->setShaderHash( shaderHash );
    entry.Shader->setPath( shaderPath.generic_string() );
    // entry.Shader->updateReflectionData( m_graphicsSystem, reflectionInfo );
    entry.ShaderHash = shaderHash;

    // save out to disk
    // saveCacheToDisk( entry, diskShaderCachePath );

    outShader = entry.Shader;
    return true;
}

void ShaderCache::clear() {
    m_cache.clear();
}

bool ShaderCache::loadCacheFromDisk( const FilePath& diskShaderCachePath,
                                     [[maybe_unused]] const FilePath& shaderPath,
                                     ShaderCacheEntry& outEntry ) {
    file_system::OnyxFile shaderDiskCacheFile = file_system::OnyxFile( diskShaderCachePath );
    file_system::FileStream stream = shaderDiskCacheFile.OpenStream( file_system::OpenMode::Binary |
                                                                     file_system::OpenMode::Read );

    if( stream.isValid() == false )
        return false;

    stream.read( outEntry.ShaderHash );

    if( outEntry.Shader->loadFromDisk( m_graphicsSystem, stream ) )
        return false;

    stream.read( outEntry.IncludeHashes );

    ShaderReflectionInfo reflectionInfo;
    stream.read( reflectionInfo );

    outEntry.Shader->setShaderHash( outEntry.ShaderHash );
#if !ONYX_IS_RETAIL
    outEntry.Shader->setPath( shaderPath.generic_string() );
#endif

    // outEntry.Shader->updateReflectionData( m_graphicsSystem, reflectionInfo );
    return true;
}

void ShaderCache::saveCacheToDisk( const ShaderCacheEntry& entry, const FilePath& diskShaderCachePath ) {
    file_system::OnyxFile shaderDiskCacheFile = file_system::OnyxFile( diskShaderCachePath );
    file_system::FileStream stream = shaderDiskCacheFile.OpenStream( file_system::OpenMode::Binary |
                                                                     file_system::OpenMode::Write );

    stream.write( entry.ShaderHash );

    if( entry.Shader->write( stream ) )
        return;

    stream.writeRaw( entry.IncludeHashes );
    stream.write( entry.Shader->getReflectionData() );
}

void ShaderCache::onFileChanged( const FilePath& path, file_system::FileWatcher::FileAction /*action*/ ) {
    // handled by the asset system
    if( path.extension() == "oshader" )
        return;

    // FilePath mountPointPath = file_system::path::ConvertToMountPath(path);
    ////String genericPath = mountPointPath.generic_string();
    // uint64_t pathHash = hash::FNV1aHash<uint64_t>(mountPointPath.generic_string());
    // for (const ShaderCacheEntry& entry : m_Cache | std::views::values)
    //{
    //    //if (entry.ShaderPathHash == pathHash)
    //    //{
    //    //    ONYX_LOG_INFO("shader changed");
    //    //}

    //    //if (entry.Includes.contains(pathHash))
    //    //{
    //    //    ONYX_LOG_INFO("shader header changed");
    //    //}
    //}
}

bool ShaderCache::isEntryUpToDate( const ShaderCacheEntry& entry, uint64_t shaderHash ) const {
    if( entry.ShaderHash != shaderHash )
        return false;

    return entry.IncludeHashes.empty() || areIncludesUpToDate( entry.IncludeHashes );
}

bool ShaderCache::areIncludesUpToDate( const HashMap< uint64_t, uint64_t >& includeHashes ) const {
    const auto predicate = [ & ]( const std::pair< uint64_t, uint64_t >& includeEntry ) {
        auto includeIt = m_includesCache.find( includeEntry.first );
        return includeIt != m_includesCache.end() && includeEntry.second == includeIt->second.ShaderHash;
    };

    return std::ranges::all_of( includeHashes, predicate );
}
} // namespace onyx::rhi
