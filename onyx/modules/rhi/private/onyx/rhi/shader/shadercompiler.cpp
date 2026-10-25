#include <onyx/rhi/shader/shadercompiler.h>

#include <onyx/filesystem/file.h>
#include <onyx/log/logger.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/shader/compiler/slang/slangcompiler.h>

namespace onyx::rhi::shader_compiler {

bool compile( const GraphicsSystem& api,
              const FilePath& shaderPath,
              const String& shaderCode,
              ShaderCacheEntry& outShaderCacheEntry ) {
    ShaderLanguage language = ShaderLanguage::Slang;
    switch( language ) {
    case ShaderLanguage::Slang: {
        if( slang::init() == false ) {
            return false;
        }
        return slang::compile( api, shaderPath, shaderCode, outShaderCacheEntry );
    }
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
