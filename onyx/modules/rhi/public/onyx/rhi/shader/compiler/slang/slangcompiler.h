#pragma once

#include <onyx/rhi/graphicstypes.h>

namespace onyx::rhi {
class GraphicsSystem;
struct ShaderCacheEntry;
class Shader;
} // namespace onyx::rhi

namespace onyx::rhi::shader_compiler::slang {

bool init();
bool compile( const GraphicsSystem& graphicsSystem,
              const FilePath& shaderPath,
              const String& shaderCode,
              ShaderCacheEntry& outShaderCacheEntry );

} // namespace onyx::rhi::shader_compiler::slang
