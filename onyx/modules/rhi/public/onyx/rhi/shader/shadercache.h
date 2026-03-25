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
    static constexpr uint64_t INVALID_SHADER_HASH = 0;

    Reference< Shader > Shader;

    // do we need this in release?
    uint64_t PathHash = INVALID_SHADER_HASH;
    uint64_t ShaderHash = INVALID_SHADER_HASH;
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
    static constexpr StringView SHADER_CACHE_PATH = "tmp:/shaders/cache";

    ShaderCache( GraphicsSystem& graphicsSystem );
    bool GetOrLoadShader( const FilePath& shaderPath, Reference< Shader >& outShader );
    void Clear();

    // TODO: add logic to switch api type?
  private:
    bool LoadCacheFromDisk( const FilePath& diskShaderCachePath, ShaderCacheEntry& outEntry );
    void SaveCacheToDisk( const ShaderCacheEntry& entry,
                          const FilePath& diskShaderCachePath,
                          const ShaderReflectionInfo& reflectionInfo );

    void OnFileChanged( const FilePath& path, file_system::FileWatcher::FileAction action );

    bool IsEntryUpToDate( const ShaderCacheEntry& entry, uint64_t shaderHash ) const;
    bool AreIncludesUpToDate( const HashMap< uint64_t, uint64_t >& includeHashes ) const;

  private:
    GraphicsSystem& m_GraphicsSystem;
    HashMap< uint64_t, ShaderCacheEntry > m_Cache;
    // stores a shader include path and the hashed content - used to identify if a shader has changed
    HashMap< uint64_t, ShaderIncludeCacheEntry > m_IncludesCache;

    file_system::FileWatcher m_DirectoryWatcher;
};
} // namespace onyx::rhi
