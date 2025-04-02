#pragma once

#include <onyx/graphics/shader/shadermodule.h>
#include <onyx/filesystem/path.h>

namespace Onyx::Graphics
{
    class GraphicsApi;
    struct PreprocessedShader;
    class ShaderCache;
    class Shader;
    class ShaderEffect;
}
namespace Onyx::Graphics
{
    struct ShaderCompiliationUnit
    {
        ShaderStage m_Stage;
        DynamicArray<String> m_Includes;
        String m_Code;
    };

    class ShaderCompiler
    {
    public:
        static bool Compile(const GraphicsApi& api, const FileSystem::Filepath& sourcePath, const String& code, ShaderStage stage, DynamicArray<onyxU32>& outByteCode);
        static bool Reflect(ShaderStage stage, const PreprocessedShader& preprocessedShader, const DynamicArray<onyxU32>& shaderByteCode, ShaderReflectionInfo& outReflectionInfo);

    private:
        static bool IsReservedDescriptorSet(onyxU8 set);
        static ShaderDescriptorSet& GetOrCreateShaderDescriptorSet(onyxU8 setIndex, ShaderReflectionInfo& reflectionInfo);
    };
}

