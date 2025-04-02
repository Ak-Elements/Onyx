#include <onyx/encoding.h>
#include <onyx/gamecore/components/graphics/textcomponent.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/graphicsapi.h>

namespace Onyx::GameCore
{
    void TextComponent::Serialize(Stream& outStream) const
    {
        outStream.Write(Text);
    }

    void TextComponent::Deserialize(const Stream& inStream)
    {
        inStream.Read(Text);
        UpdateVertexBuffers();
    }

    void TextComponent::SerializeJson(FileSystem::JsonValue& outStream) const
    {
        outStream.Set("Text", Text);
    }

    void TextComponent::DeserializeJson(const FileSystem::JsonValue& inStream)
    {
        inStream.Get("Text", Text);
        inStream.Get("Size", Size);
        UpdateVertexBuffers();
    }

    void TextComponent::SetFont(Reference<SDFFont>& font)
    {
        Font = font;
        UpdateVertexBuffers();
    }

    void TextComponent::UpdateVertexBuffers()
    {
        m_Vertices.clear();

        if (Font.IsValid() == false)
            return;

        const String_U32 textU32 = Encoding::utf8_to_utf32(Text.data());

        const SDFFontMetrics& fontMetrics = Font->GetMetrics();
        const HashMap<onyxU32, SDFFontGlyphData>& glyphs = Font->GetGlyphs();

        const Vector2f advanceDirection(1.0f, 0.0f);
        const Vector2f verticalDirection(0.0f, -1.0f);

        const onyxF32 verticalOffset = fontMetrics.LineHeight * Size;

        onyxF32 textWidth = GetTextWidth(textU32);

        Vector2f cursorPosition{ -textWidth / 2.0f, -verticalOffset };
        cursorPosition -= verticalDirection * Size * fontMetrics.Ascender;
        const Vector2f scaledAdvance = advanceDirection * Size;
        Vector2f verticalAdvance(-scaledAdvance[1], scaledAdvance[0]);

        const onyxU32 textLength = static_cast<onyxU32>(textU32.size());
        onyxU32 color = 0xFFFFFFFF;
        for (onyxU32 i = 0; i < textLength; ++i)
        {
            char32_t character = textU32[i];
            const SDFFontGlyphData& glyph = glyphs.at(character);

            if (character != ' ')
            {
                const onyxF32 planeBoundLeft = glyph.PlaneBounds.GetLeft();
                const onyxF32 planeBoundRight = glyph.PlaneBounds.GetRight();
                const onyxF32 planeBoundTop = glyph.PlaneBounds.GetTop();
                const onyxF32 planeBoundBottom = glyph.PlaneBounds.GetBottom();

                const onyxF32 atlasBoundLeft = glyph.AtlasBounds.GetLeft();
                const onyxF32 atlasBoundRight = glyph.AtlasBounds.GetRight();
                const onyxF32 atlasBoundTop = glyph.AtlasBounds.GetTop();
                const onyxF32 atlasBoundBottom = glyph.AtlasBounds.GetBottom();

                const Vector2f vertexPositionLeft = scaledAdvance * planeBoundLeft;
                const Vector2f vertexPositionRight = scaledAdvance * planeBoundRight;
                const Vector2f vertexPositionTop = verticalAdvance * planeBoundTop;
                const Vector2f vertexPositionBottom = verticalAdvance * planeBoundBottom;

                m_Vertices.emplace_back(Vector3f(cursorPosition + vertexPositionTop + vertexPositionLeft, 0), Vector2f{ atlasBoundLeft, atlasBoundTop }, color);
                m_Vertices.emplace_back(Vector3f(cursorPosition + vertexPositionTop + vertexPositionRight, 0), Vector2f{ atlasBoundRight, atlasBoundTop }, color);
                m_Vertices.emplace_back(Vector3f(cursorPosition + vertexPositionBottom + vertexPositionRight, 0), Vector2f{ atlasBoundRight, atlasBoundBottom }, color);
                m_Vertices.emplace_back(Vector3f(cursorPosition + vertexPositionBottom + vertexPositionLeft, 0), Vector2f{ atlasBoundLeft, atlasBoundBottom }, color);
            }

            Vector2f advance = advanceDirection * glyph.Advance * Size;
            if (i < (textLength - 1))
            {
                advance += advanceDirection * fontMetrics.Kerning * Size;
            }

            cursorPosition += advance;
        }
    }

    onyxF32 TextComponent::GetTextWidth(const String_U32& text)
    {
        const SDFFontMetrics& fontMetrics = Font->GetMetrics();
        const HashMap<onyxU32, SDFFontGlyphData>& glyphs = Font->GetGlyphs();

        const Vector2f advanceDirection(1.0f, 0.0f);

        onyxF32 totalWidth = 0.0f;
        const onyxU32 textLength = static_cast<onyxU32>(text.size());
        for (onyxU32 i = 0; i < textLength; ++i)
        {
            char32_t character = text[i];

            // handle multiline

            const SDFFontGlyphData& glyph = glyphs.at(character);

            Vector2f advance = advanceDirection * glyph.Advance * Size;
            if (i < (textLength - 1))
            {
                advance += advanceDirection * fontMetrics.Kerning * Size;
            }

            totalWidth += advance[0];
        }

        return totalWidth;
    }
}
