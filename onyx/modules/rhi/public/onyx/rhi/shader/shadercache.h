#pragma once

#include <onyx/filesystem/filewatcher.h>
#include <onyx/rhi/graphicstypes.h>
#include <onyx/filesystem/path.h>

namespace onyx::assets
{
    template <typename T>
    class AssetHandle;
}

namespace onyx::rhi
{
    class GraphicsSystem;
    struct ShaderReflectionInfo;
    class Shader;

    struct ShaderStageCacheEntry
    {
        onyxU64 Hash;
        HashMap<onyxU64, onyxU64> IncludeHashes; // path to include content hash
        DynamicArray<onyxU32> ByteCode;
    };

    struct ShaderCacheEntry
    {
        static constexpr onyxU64 INVALID_SHADER_HASH = 0;

        Reference<Shader> Shader;

        // do we need this in release?
        onyxU64 PathHash = INVALID_SHADER_HASH;
        onyxU64 ShaderHash = INVALID_SHADER_HASH;
        InplaceArray<ShaderStageCacheEntry, MAX_SHADER_STAGES> Stages;

        bool operator==(const ShaderCacheEntry& other) const noexcept
        {
            return ShaderHash == other.ShaderHash;
        }
    };

    struct ShaderIncludeCacheEntry
    {
        FilePath Path;
        onyxU64 ShaderHash;

        bool operator==(const ShaderIncludeCacheEntry& other) const noexcept
        {
            return ShaderHash == other.ShaderHash;
        }
    };

    class ShaderCache
    {
    public:
        // Shader cache path in temp directory
        static constexpr StringView SHADER_CACHE_PATH = "tmp:/shaders/cache";

        ShaderCache(GraphicsSystem& graphicsSystem);
        bool GetOrLoadShader(const FilePath& shaderPath, Reference<Shader>& outShader);
        void Clear();

        //TODO: add logic to switch api type?
    private:
        bool LoadCacheFromDisk(const FilePath& diskShaderCachePath, ShaderCacheEntry& outEntry);
        void SaveCacheToDisk(const ShaderCacheEntry& entry, const FilePath& diskShaderCachePath, const ShaderReflectionInfo& reflectionInfo);

        void OnFileChanged(const FilePath& path, file_system::FileWatcher::FileAction action);

        bool IsEntryUpToDate(const ShaderCacheEntry& entry, onyxU64 shaderHash) const;
        bool AreIncludesUpToDate(const HashMap<onyxU64, onyxU64>& includeHashes) const;

    private:
        GraphicsSystem& m_GraphicsSystem;
        HashMap<onyxU64, ShaderCacheEntry> m_Cache;
        // stores a shader include path and the hashed content - used to identify if a shader has changed
        HashMap<onyxU64, ShaderIncludeCacheEntry> m_IncludesCache;

        file_system::FileWatcher m_DirectoryWatcher;
    };
}
