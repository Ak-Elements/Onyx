#pragma once
#include <onyx/graphics/shader/shadereffect.h>

namespace Onyx
{
    class StringStream;
    class Stream;
}

namespace Onyx::Graphics
{
    class Shader;
    class ShaderEffect;

    struct PreprocessedShader
    {
        bool m_IsValid = false;
        DynamicArray<String> m_Includes;
        String m_Code;

        HashMap<StringId32, TextureFormat> m_Formats;
    };

    class ShaderPreprocessor
    {
    public:
        bool PreprocessShader(const String& shaderCode);

        static void Serialize(Stream& outStream, const ShaderPreprocessor& preprocessor);

        const String& GetGeneralShaderCode() const { return m_ShaderCode; }
        const InplaceArray<PreprocessedShader, MAX_SHADER_STAGES>& GetStages() { return m_PreprocessedShaderStages; }
    private:
        bool ParseGlobalScope(const StringView& line, String& outShaderCode);
        bool IsShaderStage(StringStream& reader, ShaderStage& outStage) const;

        bool ParseStageCode(StringStream& reader, ShaderStage stage, PreprocessedShader& preprocessShader);

    private:
        String m_ShaderCode;
        InplaceArray<PreprocessedShader, MAX_SHADER_STAGES> m_PreprocessedShaderStages;
    };

}
