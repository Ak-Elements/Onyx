#pragma once

#include <onyx/graphicscore/shader/shader.h>
#include <onyx/filesystem/path.h>

namespace Onyx::Graphics
{
    class GraphicsSystem;
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
        bool Preprocess(const GraphicsSystem& api, const FilePath& sourcePath, const String& code, ShaderLanguage language, ShaderStage stage, String& outPreprocessedCode, HashSet<String>& outIncludes);
        bool Compile(const GraphicsSystem& api, const FilePath& sourcePath, const String& preprocessedCode, ShaderLanguage language, ShaderStage stage, DynamicArray<onyxU32>& outByteCode);
        bool Reflect(ShaderStage stage, const PreprocessedShader& preprocessedShader, const DynamicArray<onyxU32>& shaderByteCode, ShaderReflectionInfo& outReflectionInfo);

        bool IsReservedDescriptorSet(onyxU8 set);
        ShaderDescriptorSet& GetOrCreateShaderDescriptorSet(onyxU8 setIndex, ShaderReflectionInfo& reflectionInfo);

        bool ValidateCode(const GraphicsSystem& graphicsSystem, const String& shaderSourceCode);
    };
}

