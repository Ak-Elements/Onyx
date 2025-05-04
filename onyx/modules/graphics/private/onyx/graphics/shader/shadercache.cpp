#include <onyx/graphics/shader/shadercache.h>

#include <onyx/graphics/shader/shadercompiler.h>
#include <onyx/graphics/shader/shaderpreprocessor.h>
#include <onyx/graphics/vulkan/graphicsapi.h>

#include <onyx/hash.h>

namespace Onyx::Graphics
{
    ShaderCache::ShaderCache(GraphicsApi& api)
        : m_Api(api)
    {
        if (FileSystem::Path::TempFileExists(SHADER_CACHE_PATH) == false)
        {
            FileSystem::Path::CreateDirectory(FileSystem::Path::GetTempDirectory().append(SHADER_CACHE_PATH));
        }
    }

    bool ShaderCache::GetOrLoadShader(const FileSystem::Filepath& shaderPath, ShaderCacheEntry& outEntry)
    {
        onyxU64 fileHash = Hash::FNV1aHash<onyxU64>(shaderPath.string());
        ShaderCacheEntry& entry = m_Cache[fileHash];

        // we have that shader already cached
        FileSystem::Filepath absoluteFilepath = FileSystem::Path::GetFullPath(shaderPath);
        FileSystem::OnyxFile shaderSource = FileSystem::OnyxFile(absoluteFilepath);
        String shaderCode;
        if (FileSystem::OnyxFile::ReadAll(absoluteFilepath, shaderCode) == false)
        {
            ONYX_LOG_ERROR("Missing shader file. ({})", shaderPath);
            return false;
        }

        onyxU64 shaderHash = Hash::FNV1aHash<onyxU64>(shaderCode, fileHash);

        // cached version is still valid we can return it
        if (entry.m_ShaderHash == shaderHash)
        {
            outEntry = entry;
            return true;
        }

        // check shader disk cache before recompiling / reloading shaders
        const StringView& path = Format::Format("tmp:/{}/{:x}.ocache", SHADER_CACHE_PATH, shaderHash);
        const FileSystem::Filepath& diskShaderCachePath = FileSystem::Path::GetFullPath(path);
        if (FileSystem::Path::Exists(diskShaderCachePath))
        {
            ShaderCacheEntry diskCacheEntry;
            bool hasLoaded = LoadCacheFromDisk(diskShaderCachePath, diskCacheEntry);
            if (hasLoaded)
            {
                m_Cache[fileHash] = diskCacheEntry;
                outEntry = m_Cache[fileHash];
                return true;
            }
        }

        // re-load & recompile & reflection of shaders
        ShaderPreprocessor preprocessor;
        if (preprocessor.PreprocessShader(shaderCode) == false)
        {
            ONYX_LOG_ERROR("Failed preprocessing of shader. ({})", shaderPath);
            return false;
        }

        // TODO: remove shader stages that got removed from the file
        const InplaceArray<PreprocessedShader, MAX_SHADER_STAGES>& shaderStagesSource = preprocessor.GetStages();

	    Shader::PerStageByteCodes perStageByteCodes;
        ShaderReflectionInfo reflectionInfo;

        const bool isExistingShader = entry.m_Shader.IsValid();
        for (onyxU8 i = Enums::ToIntegral(ShaderStage::Vertex); i < Enums::ToIntegral(ShaderStage::Count); ++i)
        {
            ShaderStage stage = static_cast<ShaderStage>(i);
            const PreprocessedShader& preprocessedShader = shaderStagesSource[i];
            if (preprocessedShader.m_IsValid)
            {
                const onyxU64 stageHash = Hash::FNV1aHash<onyxU64>(preprocessedShader.m_Code, shaderHash);
                if (stageHash != entry.m_StageHashes[i])
                {
					Shader::ByteCode& stageByteCode = perStageByteCodes[i];
					entry.m_StageHashes[i] = stageHash;
                    if (ShaderCompiler::Compile(m_Api, absoluteFilepath, preprocessedShader.m_Code, stage, stageByteCode) &&
                        ShaderCompiler::Reflect(stage, preprocessedShader, stageByteCode, reflectionInfo))
                    {
                        if (isExistingShader)
                            entry.m_Shader->AddStage(stage, stageByteCode);
                    }
                    else
                    {
                        // failed compiling early out
                        ONYX_LOG_ERROR("Failed compiling shader stage {}. ({})", Enums::ToString<ShaderStage>(i), shaderPath);
                        return false;
                    }
                }
            }
            else if (entry.m_StageHashes[i] != 0) // remove stages that are not in the source anymore
            {
				ONYX_ASSERT(entry.m_Shader.IsValid(), "Can't remove stage from invalid shader handle");
                entry.m_StageHashes[i] = 0;
				entry.m_Shader->RemoveStage(Enums::ToEnum<ShaderStage>(i));
            }
        }

		if (isExistingShader == false)
		{
			entry.m_Shader = m_Api.CreateShader(perStageByteCodes);
		}

        // Create descriptors for shader stage
        entry.m_Shader->UpdateReflectionData(reflectionInfo);
        entry.m_Shader->SetShaderHash(shaderHash);
        entry.m_ShaderHash = shaderHash;

        // save out to disk
        SaveCacheToDisk(entry, diskShaderCachePath, perStageByteCodes, reflectionInfo);

        outEntry = entry;
        return true;

    }

    void ShaderCache::Clear()
    {
        m_Cache.clear();
    }

    bool ShaderCache::LoadCacheFromDisk(const FileSystem::Filepath& diskShaderCachePath, ShaderCacheEntry& outEntry)
    {
        FileSystem::OnyxFile shaderDiskCacheFile = FileSystem::OnyxFile(diskShaderCachePath);
        FileSystem::FileStream stream = shaderDiskCacheFile.OpenStream(FileSystem::OpenMode::Binary | FileSystem::OpenMode::Read);

        if (stream.IsValid() == false)
            return false;

        stream.Read(outEntry.m_ShaderHash);

        Shader::PerStageByteCodes perStageByteCodes;
        for (onyxU8 i = 0; i < MAX_SHADER_STAGES; ++i)
        {
            stream.Read(outEntry.m_StageHashes[i]);

            if (outEntry.m_StageHashes[i] != 0)
                stream.ReadRaw(perStageByteCodes[i]);
        }

        ShaderReflectionInfo reflectionInfo;
        stream.Read(reflectionInfo);

        outEntry.m_Shader = m_Api.CreateShader(perStageByteCodes);
        outEntry.m_Shader->SetShaderHash(outEntry.m_ShaderHash);
        outEntry.m_Shader->UpdateReflectionData(reflectionInfo);

        return true;
    }

    void ShaderCache::SaveCacheToDisk(const ShaderCacheEntry& entry, const FileSystem::Filepath& diskShaderCachePath, const Shader::PerStageByteCodes& perStageByteCodes, const ShaderReflectionInfo& reflectionInfo)
    {
        FileSystem::OnyxFile shaderDiskCacheFile = FileSystem::OnyxFile(diskShaderCachePath);
        FileSystem::FileStream stream = shaderDiskCacheFile.OpenStream(FileSystem::OpenMode::Binary | FileSystem::OpenMode::Write);

        stream.Write(entry.m_ShaderHash);

        for (onyxU8 i = 0; i < MAX_SHADER_STAGES; ++i)
        {
            stream.Write(entry.m_StageHashes[i]);
            ONYX_ASSERT(((entry.m_StageHashes[i] != 0) && (perStageByteCodes[i].empty() == false))
                    || ((entry.m_StageHashes[i] == 0) && (perStageByteCodes[i].empty())));

            if (entry.m_StageHashes[i] != 0)
                stream.WriteRaw(perStageByteCodes[i]);
        }

        stream.Write(reflectionInfo);
    }

   
}
