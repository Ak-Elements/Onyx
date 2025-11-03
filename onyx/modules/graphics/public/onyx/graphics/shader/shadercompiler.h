#pragma once

#include <onyx/graphics/shader/shader.h>
#include <onyx/filesystem/path.h>

namespace Onyx::Graphics
{
    class GraphicsApi;
    struct PreprocessedShader;
    class Shader;
    class ShaderInstance;
}
namespace Onyx::Graphics
{
    struct ShaderCompiliationUnit
    {
        ShaderStage m_Stage;
        DynamicArray<String> m_Includes;
        String m_Code;
    };

    namespace ShaderCompiler
    {
        bool Preprocess(const GraphicsApi& api, const FileSystem::Filepath& sourcePath, const String& code, ShaderLanguage language, ShaderStage stage, String& outPreprocessedCode, HashSet<String>& outIncludes);
        bool Compile(const GraphicsApi& api, const FileSystem::Filepath& sourcePath, const String& preprocessedCode, ShaderLanguage language, ShaderStage stage, DynamicArray<onyxU32>& outByteCode);
        bool Reflect(ShaderStage stage, const PreprocessedShader& preprocessedShader, const DynamicArray<onyxU32>& shaderByteCode, ShaderReflectionInfo& outReflectionInfo);

        bool IsReservedDescriptorSet(onyxU8 set);
        ShaderDescriptorSet& GetOrCreateShaderDescriptorSet(onyxU8 setIndex, ShaderReflectionInfo& reflectionInfo);

        bool ValidateCode(const GraphicsApi& graphicsApi, const String& shaderSourceCode);
    };
}

