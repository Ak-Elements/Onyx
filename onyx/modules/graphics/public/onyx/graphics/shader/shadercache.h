#pragma once

#include <onyx/graphics/graphicstypes.h>
#include <onyx/filesystem/path.h>


namespace Onyx::Graphics
{
    class GraphicsApi;
    struct ShaderReflectionInfo;
    class Shader;

    struct ShaderCacheEntry
    {
        static constexpr onyxU64 INVALID_SHADER_HASH = 0;
        
        onyxU64 m_ShaderHash = INVALID_SHADER_HASH;
        InplaceArray<onyxU64, MAX_SHADER_STAGES> m_StageHashes;
        Reference<Shader> m_Shader;

        bool operator==(const ShaderCacheEntry& other) const noexcept
        {
            return m_ShaderHash == other.m_ShaderHash;
        }
    };

    class ShaderCache
    {
    public:
        // Shader cache path in temp directory
        static constexpr StringView SHADER_CACHE_PATH = "shaders/cache";

        ShaderCache(GraphicsApi& apiType);
        bool GetOrLoadShader(const FileSystem::Filepath& shaderPath, ShaderCacheEntry& outEntry);
        void Clear();

        //TODO: add logic to switch api type?
    private:
        bool LoadCacheFromDisk(const FileSystem::Filepath& diskShaderCachePath, ShaderCacheEntry& outEntry);
        void SaveCacheToDisk(const ShaderCacheEntry& entry, const FileSystem::Filepath& diskShaderCachePath, const InplaceArray<DynamicArray<onyxU32>, MAX_SHADER_STAGES>& perStageByteCodes, const ShaderReflectionInfo& reflectionInfo);
    private:

        GraphicsApi& m_Api;
        HashMap<onyxU64, ShaderCacheEntry> m_Cache;
    };
}
