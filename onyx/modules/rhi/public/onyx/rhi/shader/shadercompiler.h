#pragma once

#include <onyx/filesystem/path.h>
#include <onyx/rhi/shader/shader.h>

namespace onyx::rhi {
class GraphicsSystem;
struct ShaderCacheEntry;
} // namespace onyx::rhi
namespace onyx::rhi {

namespace shader_compiler {

bool compile( const GraphicsSystem& api,
              const FilePath& shaderPath,
              const String& shaderCode,
              ShaderCacheEntry& outShaderCacheEntry );

bool validateCode( const GraphicsSystem& graphicsSystem, const String& shaderSourceCode );
}; // namespace shader_compiler
} // namespace onyx::rhi
