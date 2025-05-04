#include <onyx/graphics/shader/shaderpreprocessor.h>

#include <onyx/graphics/shader/shadercompiler.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/log/logger.h>
#include <onyx/stream/stream.h>
#include <onyx/stream/stringstream.h>

#include <regex>

namespace Onyx::Graphics
{
    bool ShaderPreprocessor::PreprocessShader(const String& shaderCode)
    {
        // split the shader file into stages and preprocess defines/macros and includes
        using namespace FileSystem;
        StringStream reader(shaderCode);

        m_ShaderCode.clear();
        m_ShaderCode.reserve(shaderCode.length());

        StringView currentLine;
        ShaderStage shaderStage = ShaderStage::Invalid;

        PreprocessedShader preprocessShader;
        
        while (reader.IsEof() == false)
        {
            // global scope
            if (shaderStage == ShaderStage::Invalid)
            {
                if (IsShaderStage(reader, shaderStage))
                {
                    PreprocessedShader& shader = m_PreprocessedShaderStages[static_cast<onyxU8>(shaderStage)];
                    if (shader.m_IsValid == false)
                    {
                        shader.m_IsValid = true;
                        if (m_ShaderCode.empty() == false)
                        {
                            shader.m_Code = m_ShaderCode;
                        }
                    }
                }
                else
                {
                    reader.ReadLine(currentLine);
                    m_ShaderCode.append(currentLine);
                }
            }
            else 
            {
                if (ParseStageCode(reader, shaderStage, m_PreprocessedShaderStages[static_cast<onyxU8>(shaderStage)]) == false)
                {
                    ONYX_LOG_ERROR("Failed parsing shader stage ({}) for shader", Enums::ToString(shaderStage));
                    return false;
                }

                shaderStage = ShaderStage::Invalid;
            }
        }

        return true;
    }

    bool ShaderPreprocessor::ParseGlobalScope(const StringView& line, String& outShaderCode)
    {
        if (line.starts_with("#pragma"))
        {
            // parse pragma
            outShaderCode += line;
            return true;

        }
        else if (line.starts_with("#define"))
        {
            // parse define
            outShaderCode += line;
            return true;
        }

        return false;
    }

    bool ShaderPreprocessor::IsShaderStage(StringStream& reader, ShaderStage& outStage) const
    {
        for (onyxU8 i = static_cast<onyxU8>(ShaderStage::Vertex); i < static_cast<onyxU8>(ShaderStage::Count); ++i)
        {
            outStage = static_cast<ShaderStage>(i);
            if (reader.ReadConditional(Enums::ToString(outStage)))
            {
                return true;
            }
        }

        outStage = ShaderStage::Invalid;
        return false;
    }

    bool ShaderPreprocessor::ParseStageCode(StringStream& reader, ShaderStage stage, PreprocessedShader& preprocessShader)
    {
        StringView currentLine;
        // check if the current unfinished line still contains the opening {

        bool hasShaderStageScopeBegun = false;
        onyxS32 bracesCount = 0;
        while (reader.IsEof() == false)
        {
            reader.ReadLine(currentLine);

            if (hasShaderStageScopeBegun == false)
            {
                StringView::size_type firstCharIndex = currentLine.find_first_not_of(" \n\r\t\f\v");
                if (firstCharIndex == StringView::npos)
                    continue;

                if (currentLine[firstCharIndex] != '{')
                {
                    ONYX_LOG_ERROR("Expected {{ got {} in shader stage {}.", currentLine[firstCharIndex], Enums::ToString(stage));
                    return false;
                }

                hasShaderStageScopeBegun = true;
                currentLine = currentLine.substr(1);
                bracesCount = 1;
            }

            bracesCount += static_cast<onyxS32>(std::ranges::count(currentLine, '{'));
            bracesCount -= static_cast<onyxS32>(std::ranges::count(currentLine, '}'));
            if (bracesCount < 0)
            {
                ONYX_LOG_ERROR("Missing '{{' or '}}' bracket in shader stage {}.", Enums::ToString(stage));
                return false;
            }

            if (bracesCount == 1)
            {
                StringView::size_type customFormatStart = currentLine.find_first_of('<');
                StringView::size_type customFormatEnd = currentLine.find_first_of('>');

                if (((customFormatStart != StringView::npos) && (customFormatEnd == StringView::npos)) ||
                    ((customFormatStart == StringView::npos) && (customFormatEnd != StringView::npos)))
                {
                    ONYX_LOG_ERROR("Missing '{{}}' bracket in shader stage {}.", (customFormatStart == StringView::npos) ? '<' : '>', Enums::ToString(stage));
                    return false;
                }

                if ((customFormatStart != StringView::npos) && (customFormatEnd != StringView::npos))
                {
                    TextureFormat attachmentFormat = Enums::FromString<TextureFormat>(currentLine.substr(customFormatStart + 1, (customFormatEnd - customFormatStart) - 1));

                    StringView::size_type attachmentNameStart = currentLine.find_first_not_of(" \n\r\t\f\v", customFormatEnd + 1);
                    StringView::size_type attachmentNameEnd = currentLine.find_first_of(';', customFormatEnd + 1);

                    if (attachmentNameEnd == StringView::npos)
                    {
                        ONYX_LOG_ERROR("Missing ';' in shader stage {}.", Enums::ToString(stage));
                        return false;
                    }


                    StringView attachmentName = currentLine.substr(attachmentNameStart, (attachmentNameEnd - attachmentNameStart));

                    StringId32 attachmentId(attachmentName);
                    ONYX_ASSERT(preprocessShader.m_Formats.contains(attachmentId) == false, "Format for this attachment is already defined.");
                    preprocessShader.m_Formats[attachmentId] = attachmentFormat;

                    preprocessShader.m_Code += currentLine.substr(0, customFormatStart);

                    preprocessShader.m_Code += currentLine.substr(customFormatEnd + 1, (attachmentNameEnd - customFormatEnd));
                    currentLine = currentLine.substr(attachmentNameEnd + 1);
                }
            }
            else if(bracesCount == 0)
            {

                StringView::size_type lastBracesIndex = currentLine.find_last_of('}');
                preprocessShader.m_Code += currentLine.substr(0, lastBracesIndex);

                if ((lastBracesIndex + 1) != currentLine.size())
                    preprocessShader.m_Code += currentLine.substr(lastBracesIndex + 1);

                break;
                
            }
            
            preprocessShader.m_Code += currentLine;
        }

        return true;

    }

    void ShaderPreprocessor::Serialize(Stream& outStream, const ShaderPreprocessor& preprocessor)
    {
        outStream.Write<onyxU64>(preprocessor.m_PreprocessedShaderStages.size());

        for (const PreprocessedShader& stage : preprocessor.m_PreprocessedShaderStages)
        {
            outStream.Write<onyxU64>(stage.m_Includes.size());
            for (const String& includeStr : stage.m_Includes)
            {
                outStream.Write<onyxU64>(includeStr.length());
                outStream.Write(includeStr);
            }

            outStream.Write<onyxU64>(stage.m_Code.length());
            outStream.Write(stage.m_Code);
        }
    }
}
