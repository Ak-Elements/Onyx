#pragma once

#include <onyx/filesystem/filewatcher.h>
#include <onyx/filesystem/path.h>
#include <onyx/rhi/graphicstypes.h>

namespace onyx::assets {
template < typename T >
class AssetHandle;
}

namespace onyx::rhi {
class GraphicsSystem;
struct ShaderReflectionInfo;
class Shader;

struct ShaderStageCacheEntry {
    uint64_t Hash;
    HashMap< uint64_t, uint64_t > IncludeHashes; // path to include content hash
    DynamicArray< uint32_t > ByteCode;
};

struct ShaderCacheEntry {
    static constexpr uint64_t InvalidShaderHash = 0;

    Reference< Shader > Shader;

    // do we need this in release?
    uint64_t PathHash = InvalidShaderHash;
    uint64_t ShaderHash = InvalidShaderHash;
    InplaceArray< ShaderStageCacheEntry, MAX_SHADER_STAGES > Stages;

    bool operator==( const ShaderCacheEntry& other ) const noexcept { return ShaderHash == other.ShaderHash; }
};

struct ShaderIncludeCacheEntry {
    FilePath Path;
    uint64_t ShaderHash;

    bool operator==( const ShaderIncludeCacheEntry& other ) const noexcept { return ShaderHash == other.ShaderHash; }
};

class ShaderCache {
  public:
    // Shader cache path in temp directory
    static constexpr StringView ShaderCachePath = "tmp:/shaders/cache";

    ShaderCache( GraphicsSystem& graphicsSystem );
    bool getOrLoadShader( const FilePath& shaderPath, Reference< Shader >& outShader );
    void clear();

    // TODO: add logic to switch api type?
  private:
    bool loadCacheFromDisk( const FilePath& diskShaderCachePath,
                            [[maybe_unused]] const FilePath& shaderPath,
                            ShaderCacheEntry& outEntry );

    void saveCacheToDisk( const ShaderCacheEntry& entry,
                          const FilePath& diskShaderCachePath,
                          const ShaderReflectionInfo& reflectionInfo );

    void onFileChanged( const FilePath& path, file_system::FileWatcher::FileAction action );

    bool isEntryUpToDate( const ShaderCacheEntry& entry, uint64_t shaderHash ) const;
    bool areIncludesUpToDate( const HashMap< uint64_t, uint64_t >& includeHashes ) const;

  private:
    GraphicsSystem& m_graphicsSystem;
    HashMap< uint64_t, ShaderCacheEntry > m_cache;
    // stores a shader include path and the hashed content - used to identify if a shader has changed
    HashMap< uint64_t, ShaderIncludeCacheEntry > m_includesCache;

    file_system::FileWatcher m_directoryWatcher;
};
} // namespace onyx::rhi
