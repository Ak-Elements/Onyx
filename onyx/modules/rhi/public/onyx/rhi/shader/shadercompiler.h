#pragma once

#include <onyx/filesystem/path.h>
#include <onyx/rhi/shader/shader.h>

namespace onyx::rhi {
class GraphicsSystem;
struct PreprocessedShader;
struct ShaderStageCacheEntry;
class Shader;
class ShaderInstance;
} // namespace onyx::rhi
namespace onyx::rhi {
struct ShaderCompiliationUnit {
    ShaderStage Stage;
    DynamicArray< String > Includes;
    String Code;
};

namespace shader_compiler {

bool compile( const GraphicsSystem& api, const FilePath& shaderPath, const String& shaderCode, Shader& outShader );

bool validateCode( const GraphicsSystem& graphicsSystem, const String& shaderSourceCode );
}; // namespace shader_compiler
} // namespace onyx::rhi
