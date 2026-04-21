#include <onyx/rhi/shader/shadercache.h>

#include <onyx/hash.h>
#include <onyx/rhi/shader/shadercompiler.h>
#include <onyx/rhi/shader/shaderpreprocessor.h>
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

    for( const FilePath& shaderDirectory : GetShaderDirectories() ) {
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

    // TODO: remove shader stages that got removed from the file
    // re-load & recompile & reflection of shaders
    ShaderPreprocessor preprocessor;
    if( preprocessor.PreprocessShader( shaderCode ) == false ) {
        ONYX_LOG_ERROR( "Failed preprocessing of shader. ({})", shaderPath );
        return false;
    }

    HashSet< String > stageIncludes;
    const InplaceArray< PreprocessedShader, MAX_SHADER_STAGES >& shaderStagesSource = preprocessor.GetStages();

    ShaderReflectionInfo reflectionInfo;

    // const bool isExistingShader = entry.Shader.IsValid();
    for( uint8_t i = enums::toIntegral( ShaderStage::Vertex ); i < enums::toIntegral( ShaderStage::Count ); ++i ) {
        ShaderStage stage = static_cast< ShaderStage >( i );
        const PreprocessedShader& preprocessedShader = shaderStagesSource[ i ];
        if( preprocessedShader.m_IsValid ) {
            ShaderStageCacheEntry& stageCacheEntry = entry.Stages[ i ];
            const uint64_t stageHash = hash::fnV1aHash< uint64_t >( preprocessedShader.m_Code, shaderHash );
            if( ( stageCacheEntry.Hash != stageHash ) ||
                ( areIncludesUpToDate( stageCacheEntry.IncludeHashes ) == false ) ) {
                stageCacheEntry.Hash = stageHash;
                stageCacheEntry.ByteCode.clear();
                stageCacheEntry.IncludeHashes.clear();
                stageIncludes.clear();

                String shaderCPreprocessedSource;
                if( ShaderCompiler::Preprocess( m_graphicsSystem,
                                                absoluteFilepath,
                                                preprocessedShader.m_Code,
                                                ShaderLanguage::GLSL,
                                                stage,
                                                shaderCPreprocessedSource,
                                                stageIncludes ) &&
                    ShaderCompiler::Compile( m_graphicsSystem,
                                             absoluteFilepath,
                                             shaderCPreprocessedSource,
                                             ShaderLanguage::GLSL,
                                             stage,
                                             stageCacheEntry.ByteCode ) &&
                    ShaderCompiler::Reflect( stage, preprocessedShader, stageCacheEntry.ByteCode, reflectionInfo ) ) {
                    entry.Shader->AddStage( m_graphicsSystem, stage, stageCacheEntry.ByteCode );
                } else {
                    // failed compiling early out
                    ONYX_LOG_ERROR( "Failed compiling shader stage {}. ({})",
                                    enums::toString< ShaderStage >( i ),
                                    shaderPath );
                    return false;
                }

                for( const String& includePath : stageIncludes ) {
                    FilePath mountPointPath = file_system::path::convertToMountPath( includePath );
                    uint64_t includePathHash = hash::fnV1aHash< uint64_t >( mountPointPath.generic_string() );
                    stageCacheEntry.IncludeHashes[ includePathHash ] = m_includesCache[ includePathHash ].ShaderHash;
                }
            }
        } else if( entry.Stages[ i ].Hash != 0 ) // remove stages that are not in the source anymore
        {
            ONYX_ASSERT( entry.Shader.isValid(), "Can't remove stage from invalid shader handle" );
            entry.Stages[ i ].Hash = 0;
            entry.Shader->RemoveStage( enums::toEnum< ShaderStage >( i ) );
        }
    }

    // if (isExistingShader == false)
    //{
    //	entry.Shader = m_Api.CreateShader(entry);
    // }

    // Create descriptors for shader stage
    entry.Shader->SetShaderHash( shaderHash );
    entry.Shader->setPath( shaderPath.generic_string() );
    entry.Shader->UpdateReflectionData( m_graphicsSystem, reflectionInfo );
    entry.ShaderHash = shaderHash;

    // save out to disk
    saveCacheToDisk( entry, diskShaderCachePath, reflectionInfo );

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

    Shader::PerStageByteCodes perStageByteCodes;
    for( uint8_t i = 0; i < MAX_SHADER_STAGES; ++i ) {
        ShaderStageCacheEntry& stageEntry = outEntry.Stages[ i ];
        stream.read( stageEntry.Hash );

        if( stageEntry.Hash == ShaderCacheEntry::InvalidShaderHash )
            continue;

        stream.readRaw( stageEntry.ByteCode );
        stream.readRaw( stageEntry.IncludeHashes );

        outEntry.Shader->AddStage( m_graphicsSystem, enums::toEnum< ShaderStage >( i ), stageEntry.ByteCode );
    }

    ShaderReflectionInfo reflectionInfo;
    stream.read( reflectionInfo );

    outEntry.Shader->SetShaderHash( outEntry.ShaderHash );
#if !ONYX_IS_RETAIL
    outEntry.Shader->setPath( shaderPath.generic_string() );
#endif

    outEntry.Shader->UpdateReflectionData( m_graphicsSystem, reflectionInfo );
    return true;
}

void ShaderCache::saveCacheToDisk( const ShaderCacheEntry& entry,
                                   const FilePath& diskShaderCachePath,
                                   const ShaderReflectionInfo& reflectionInfo ) {
    file_system::OnyxFile shaderDiskCacheFile = file_system::OnyxFile( diskShaderCachePath );
    file_system::FileStream stream = shaderDiskCacheFile.OpenStream( file_system::OpenMode::Binary |
                                                                     file_system::OpenMode::Write );

    stream.write( entry.ShaderHash );

    for( uint8_t i = 0; i < MAX_SHADER_STAGES; ++i ) {
        const ShaderStageCacheEntry& stageEntry = entry.Stages[ i ];
        stream.write( stageEntry.Hash );
        ONYX_ASSERT( ( ( stageEntry.Hash != 0 ) && ( stageEntry.ByteCode.empty() == false ) ) ||
                     ( ( stageEntry.Hash == 0 ) && ( stageEntry.ByteCode.empty() ) ) );

        if( stageEntry.Hash == ShaderCacheEntry::InvalidShaderHash )
            continue;

        stream.writeRaw( stageEntry.ByteCode );
        stream.writeRaw( stageEntry.IncludeHashes );
    }

    stream.write( reflectionInfo );
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

    for( const ShaderStageCacheEntry& stage : entry.Stages ) {
        if( stage.Hash == ShaderCacheEntry::InvalidShaderHash )
            continue;

        if( areIncludesUpToDate( stage.IncludeHashes ) == false )
            return false;
    }

    return true;
}

bool ShaderCache::areIncludesUpToDate( const HashMap< uint64_t, uint64_t >& includeHashes ) const {
    const auto predicate = [ & ]( const std::pair< uint64_t, uint64_t >& includeEntry ) {
        auto includeIt = m_includesCache.find( includeEntry.first );
        return includeIt != m_includesCache.end() && includeEntry.second == includeIt->second.ShaderHash;
    };

    return std::ranges::all_of( includeHashes, predicate );
}
} // namespace onyx::rhi
