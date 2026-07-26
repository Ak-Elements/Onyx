#include <onyx/rhi/shader/shadercompiler.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/log/logger.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/shader/compiler/slang/slangcompiler.h>

namespace onyx::rhi::shader_compiler {
// TODO: Move this to a vulkan shader compiler implementation
namespace {} // namespace

bool compile( const GraphicsSystem& api, const FilePath& shaderPath, const String& shaderCode, Shader& outShader ) {
    ShaderLanguage language = ShaderLanguage::Slang;
    switch( language ) {
    case ShaderLanguage::Slang:
        std::ignore = slang::init();
        return slang::compile( api, shaderPath, shaderCode, outShader );
    case ShaderLanguage::Invalid:
    case ShaderLanguage::Glsl:
    case ShaderLanguage::Hlsl:
    case ShaderLanguage::Count:
        break;
    }

    ONYX_ASSERT( false, "Shader compiler not implemented for language ({}).", enums::toString( language ) );
    return false;
}

bool validateCode( [[maybe_unused]] const GraphicsSystem& graphicsSystem,
                   [[maybe_unused]] const String& shaderSourceCode ) {
    return true; // glsl::validateCode( graphicsSystem, shaderSourceCode );
}

} // namespace onyx::rhi::shader_compiler
