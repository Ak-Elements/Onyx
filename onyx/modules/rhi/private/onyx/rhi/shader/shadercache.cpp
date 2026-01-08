#include <onyx/rhi/shader/shadercache.h>

#include <onyx/hash.h>
#include <onyx/rhi/shader/shadercompiler.h>
#include <onyx/rhi/shader/shaderpreprocessor.h>
#include <onyx/rhi/vulkan/graphicsapi.h>

#include <onyx/filesystem/filestream.h>
#include <onyx/filesystem/onyxfile.h>

namespace Onyx::Graphics
{
    ShaderCache::ShaderCache(GraphicsSystem& graphicsSystem)
        : m_GraphicsSystem(graphicsSystem)
    {
#if !ONYX_IS_RETAIL
        const FilePath shaderCacheDirectory = FileSystem::Path::GetFullPath(SHADER_CACHE_PATH);
        if (FileSystem::Path::Exists(shaderCacheDirectory) == false)
        {
            FileSystem::Path::CreateDirectory(shaderCacheDirectory);
        }

        for (const FilePath& shaderDirectory : GetShaderDirectories())
        {
            m_DirectoryWatcher.AddPath(shaderDirectory, true);

            FileSystem::Path::EnumerateFiles(shaderDirectory, [&](const FilePath& path)
            {
                if (path.has_filename() == false)
                    return true;

                if (path.extension() != ".h")
                    return true;

                //TODO: enqueue in a different thread
                FilePath mountPointPath = FileSystem::Path::ConvertToMountPath(path);

                String content;
                if (FileSystem::OnyxFile::ReadAll(path, content) == false)
                {
                    ONYX_LOG_ERROR("Failed reading shader file. ({})", path);
                    return true;
                }

                onyxU64 fileHash = Hash::FNV1aHash<onyxU64>(mountPointPath.generic_string());
                onyxU64 shaderFileHash = Hash::FNV1aHash<onyxU64>(content, fileHash);

                m_IncludesCache[fileHash] = { .Path = mountPointPath, .ShaderHash = shaderFileHash, };
                return true;
            });
        }

        m_DirectoryWatcher.OnFileChanged.Connect<&ShaderCache::OnFileChanged>(this);
#endif
    }

    bool ShaderCache::GetOrLoadShader(const FilePath& shaderPath, ShaderHandle& outShader)
    {
        // TODO: Hash should be hash of properties not just the path
        onyxU64 fileHash = Hash::FNV1aHash<onyxU64>(shaderPath.generic_string());

        auto entryIt = m_Cache.find(fileHash);
        bool hasEntry = entryIt != m_Cache.end();

        if (hasEntry)
        {
            outShader = entryIt->second.Shader;
        }
        else
        {
            entryIt = m_Cache.emplace(fileHash, outShader).first;
        }

        ShaderCacheEntry& entry = entryIt->second;

        // we have that shader already cached
        FilePath absoluteFilepath = FileSystem::Path::GetFullPath(shaderPath);
        FileSystem::OnyxFile shaderSource = FileSystem::OnyxFile(absoluteFilepath);
        String shaderCode;
        if (FileSystem::OnyxFile::ReadAll(absoluteFilepath, shaderCode) == false)
        {
            ONYX_LOG_ERROR("Missing shader file. ({})", shaderPath);
            return false;
        }

        onyxU64 shaderHash = Hash::FNV1aHash<onyxU64>(shaderCode, fileHash);

        // cached version is still valid we can return it
        // TODO: This is not correct when doing a reload from a header change
        StringView path = Format::Format("{}/{:x}.ocache", SHADER_CACHE_PATH, shaderHash);
        const FilePath& diskShaderCachePath = FileSystem::Path::GetFullPath(path);
        if (hasEntry)
        {
            if (IsEntryUpToDate(entry, shaderHash))
            {
                //outEntry = entry;
                outShader = entry.Shader;
                return true;
            }
        }
        else // get from shader cache
        {
            // check shader disk cache before recompiling / reloading shaders
            if (FileSystem::Path::Exists(diskShaderCachePath))
            {
                bool hasLoaded = LoadCacheFromDisk(diskShaderCachePath, entry);
                if (hasLoaded && IsEntryUpToDate(entry, shaderHash))
                {
                    m_Cache[fileHash] = entry;
                    outShader = m_Cache[fileHash].Shader;
                    return true;
                }
            }
        }

        // TODO: remove shader stages that got removed from the file
        // re-load & recompile & reflection of shaders
        ShaderPreprocessor preprocessor;
        if (preprocessor.PreprocessShader(shaderCode) == false)
        {
            ONYX_LOG_ERROR("Failed preprocessing of shader. ({})", shaderPath);
            return false;
        }

        HashSet<String> stageIncludes;
        const InplaceArray<PreprocessedShader, MAX_SHADER_STAGES>& shaderStagesSource = preprocessor.GetStages();

        ShaderReflectionInfo reflectionInfo;
        
        //const bool isExistingShader = entry.Shader.IsValid();
        for (onyxU8 i = Enums::ToIntegral(ShaderStage::Vertex); i < Enums::ToIntegral(ShaderStage::Count); ++i)
        {
            ShaderStage stage = static_cast<ShaderStage>(i);
            const PreprocessedShader& preprocessedShader = shaderStagesSource[i];
            if (preprocessedShader.m_IsValid)
            {
                ShaderStageCacheEntry& stageCacheEntry = entry.Stages[i];
                const onyxU64 stageHash = Hash::FNV1aHash<onyxU64>(preprocessedShader.m_Code, shaderHash);
                if ((stageCacheEntry.Hash != stageHash) ||
                    (AreIncludesUpToDate(stageCacheEntry.IncludeHashes) == false))
                {
                    stageCacheEntry.Hash = stageHash;
                    stageCacheEntry.ByteCode.clear();
                    stageCacheEntry.IncludeHashes.clear();
                    stageIncludes.clear();
                    
                    String shaderCPreprocessedSource;
                    if (ShaderCompiler::Preprocess(m_GraphicsSystem, absoluteFilepath, preprocessedShader.m_Code, ShaderLanguage::GLSL, stage, shaderCPreprocessedSource, stageIncludes) &&
                        ShaderCompiler::Compile(m_GraphicsSystem, absoluteFilepath, shaderCPreprocessedSource, ShaderLanguage::GLSL, stage, stageCacheEntry.ByteCode) &&
                        ShaderCompiler::Reflect(stage, preprocessedShader, stageCacheEntry.ByteCode, reflectionInfo))
                    {
                        entry.Shader->AddStage(m_GraphicsSystem, stage, stageCacheEntry.ByteCode);
                    }
                    else
                    {
                        // failed compiling early out
                        ONYX_LOG_ERROR("Failed compiling shader stage {}. ({})", Enums::ToString<ShaderStage>(i), shaderPath);
                        return false;
                    }
                        
                    for (const String& includePath : stageIncludes)
                    {
                       FilePath mountPointPath = FileSystem::Path::ConvertToMountPath(includePath);
                       onyxU64 includePathHash = Hash::FNV1aHash<onyxU64>(mountPointPath.generic_string());
                       stageCacheEntry.IncludeHashes[includePathHash] = m_IncludesCache[includePathHash].ShaderHash;
                    }
                }
            }
            else if (entry.Stages[i].Hash != 0) // remove stages that are not in the source anymore
            {
				ONYX_ASSERT(entry.Shader.IsValid(), "Can't remove stage from invalid shader handle");
                entry.Stages[i].Hash = 0;
				entry.Shader->RemoveStage(Enums::ToEnum<ShaderStage>(i));
            }
        }

		//if (isExistingShader == false)
		//{
		//	entry.Shader = m_Api.CreateShader(entry);
		//}

        // Create descriptors for shader stage
        entry.Shader->UpdateReflectionData(m_GraphicsSystem, reflectionInfo);
        entry.Shader->SetShaderHash(shaderHash);
        entry.ShaderHash = shaderHash;

        // save out to disk
        SaveCacheToDisk(entry, diskShaderCachePath, reflectionInfo);

        outShader = entry.Shader;
        return true;

    }

    void ShaderCache::Clear()
    {
        m_Cache.clear();
    }

    bool ShaderCache::LoadCacheFromDisk(const FilePath& diskShaderCachePath, ShaderCacheEntry& outEntry)
    {
        FileSystem::OnyxFile shaderDiskCacheFile = FileSystem::OnyxFile(diskShaderCachePath);
        FileSystem::FileStream stream = shaderDiskCacheFile.OpenStream(FileSystem::OpenMode::Binary | FileSystem::OpenMode::Read);

        if (stream.IsValid() == false)
            return false;

        stream.Read(outEntry.ShaderHash);

        Shader::PerStageByteCodes perStageByteCodes;
        for (onyxU8 i = 0; i < MAX_SHADER_STAGES; ++i)
        {
            ShaderStageCacheEntry& stageEntry = outEntry.Stages[i];
            stream.Read(stageEntry.Hash);

            if (stageEntry.Hash == ShaderCacheEntry::INVALID_SHADER_HASH)
                continue;

            stream.ReadRaw(stageEntry.ByteCode);
            stream.ReadRaw(stageEntry.IncludeHashes);

            outEntry.Shader->AddStage(m_GraphicsSystem, Enums::ToEnum<ShaderStage>(i), stageEntry.ByteCode);
        }

        ShaderReflectionInfo reflectionInfo;
        stream.Read(reflectionInfo);

        
        outEntry.Shader->SetShaderHash(outEntry.ShaderHash);
        outEntry.Shader->UpdateReflectionData(m_GraphicsSystem, reflectionInfo);

        return true;
    }

    void ShaderCache::SaveCacheToDisk(const ShaderCacheEntry& entry, const FilePath& diskShaderCachePath, const ShaderReflectionInfo& reflectionInfo)
    {
        FileSystem::OnyxFile shaderDiskCacheFile = FileSystem::OnyxFile(diskShaderCachePath);
        FileSystem::FileStream stream = shaderDiskCacheFile.OpenStream(FileSystem::OpenMode::Binary | FileSystem::OpenMode::Write);

        stream.Write(entry.ShaderHash);
        
        for (onyxU8 i = 0; i < MAX_SHADER_STAGES; ++i)
        {
            const ShaderStageCacheEntry& stageEntry =  entry.Stages[i];
            stream.Write(stageEntry.Hash);
            ONYX_ASSERT(((stageEntry.Hash != 0) && (stageEntry.ByteCode.empty() == false))
                    || ((stageEntry.Hash == 0) && (stageEntry.ByteCode.empty())));

            if (stageEntry.Hash == ShaderCacheEntry::INVALID_SHADER_HASH)
                continue;

            stream.WriteRaw(stageEntry.ByteCode);
            stream.WriteRaw(stageEntry.IncludeHashes);
        }

        stream.Write(reflectionInfo);
    }

    void ShaderCache::OnFileChanged(const FilePath& path, FileSystem::FileWatcher::FileAction /*action*/)
    {
        // handled by the asset system
        if (path.extension() == "oshader")
            return;

        //FilePath mountPointPath = FileSystem::Path::ConvertToMountPath(path);
        ////String genericPath = mountPointPath.generic_string();
        //onyxU64 pathHash = Hash::FNV1aHash<onyxU64>(mountPointPath.generic_string());
        //for (const ShaderCacheEntry& entry : m_Cache | std::views::values)
        //{ 
        //   //if (entry.ShaderPathHash == pathHash)
        //   //{
        //   //    ONYX_LOG_INFO("shader changed");
        //   //}

        //    //if (entry.Includes.contains(pathHash))
        //    //{
        //    //    ONYX_LOG_INFO("shader header changed");
        //    //}
        //}
    }

    bool ShaderCache::IsEntryUpToDate(const ShaderCacheEntry& entry, onyxU64 shaderHash) const
    {
        if (entry.ShaderHash != shaderHash)
            return false;

        for (const ShaderStageCacheEntry& stage : entry.Stages)
        {
            if (stage.Hash == ShaderCacheEntry::INVALID_SHADER_HASH)
                continue;

            if (AreIncludesUpToDate(stage.IncludeHashes) == false)
                return false;
        }

        return true;
    }

    bool ShaderCache::AreIncludesUpToDate(const HashMap<onyxU64, onyxU64>& includeHashes) const
    {
        const auto predicate = [&](const std::pair<onyxU64, onyxU64>& includeEntry)
        {
            auto includeIt = m_IncludesCache.find(includeEntry.first);
            return includeIt != m_IncludesCache.end() && includeEntry.second == includeIt->second.ShaderHash;
        };

        return std::ranges::all_of(includeHashes, predicate);
    }
}
