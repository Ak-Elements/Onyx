#pragma once

#include <onyx/rhi/graphicstypes.h>

namespace onyx::rhi {
class GraphicsSystem;
enum class ShaderStage : uint16_t;
struct ShaderReflectionInfo;
struct ShaderStageCacheEntry;
class Shader;
} // namespace onyx::rhi

namespace onyx::rhi::shader_compiler::slang {

bool init();
bool compile( const GraphicsSystem& graphicsSystem,
              const FilePath& shaderPath,
              const String& shaderCode,
              Shader& outShader );

} // namespace onyx::rhi::shader_compiler::slang
