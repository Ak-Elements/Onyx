#pragma once

#include <onyx/filesystem/path.h>
#include <onyx/rhi/shader/shader.h>

namespace onyx::rhi {
class GraphicsSystem;
struct PreprocessedShader;
class Shader;
class ShaderInstance;
} // namespace onyx::rhi
namespace onyx::rhi {
struct ShaderCompiliationUnit {
    ShaderStage m_Stage;
    DynamicArray< String > m_Includes;
    String m_Code;
};

namespace ShaderCompiler {
bool Preprocess( const GraphicsSystem& api,
                 const FilePath& sourcePath,
                 const String& code,
                 ShaderLanguage language,
                 ShaderStage stage,
                 String& outPreprocessedCode,
                 HashSet< String >& outIncludes );
bool Compile( const GraphicsSystem& api,
              const FilePath& sourcePath,
              const String& preprocessedCode,
              ShaderLanguage language,
              ShaderStage stage,
              DynamicArray< uint32_t >& outByteCode );
bool Reflect( ShaderStage stage,
              const PreprocessedShader& preprocessedShader,
              const DynamicArray< uint32_t >& shaderByteCode,
              ShaderReflectionInfo& outReflectionInfo );

bool IsReservedDescriptorSet( uint8_t set );
ShaderDescriptorSet& GetOrCreateShaderDescriptorSet( uint8_t setIndex, ShaderReflectionInfo& reflectionInfo );

bool ValidateCode( const GraphicsSystem& graphicsSystem, const String& shaderSourceCode );
}; // namespace ShaderCompiler
} // namespace onyx::rhi
