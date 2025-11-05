#include <onyx/gamecore/components/graphics/textcomponent.h>
#include <onyx/encoding.h>

#include <onyx/graphics/font/sdffont.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{

namespace GameCore
{
    void TextComponent::SetFont(Reference<Graphics::SDFFont>& font)
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

        const Graphics::SDFFontMetrics& fontMetrics = Font->GetMetrics();
        const HashMap<onyxU32, Graphics::SDFFontGlyphData>& glyphs = Font->GetGlyphs();

        const Vector2f32 advanceDirection(1.0f, 0.0f);
        const Vector2f32 verticalDirection(0.0f, -1.0f);

        const onyxF32 verticalOffset = fontMetrics.LineHeight * Size;

        onyxF32 textWidth = GetTextWidth(textU32);

        Vector2f32 cursorPosition{ -textWidth / 2.0f, -verticalOffset };
        cursorPosition -= verticalDirection * Size * fontMetrics.Ascender;
        const Vector2f32 scaledAdvance = advanceDirection * Size;
        Vector2f32 verticalAdvance(-scaledAdvance[1], scaledAdvance[0]);

        const onyxU32 textLength = static_cast<onyxU32>(textU32.size());
        onyxU32 color = 0xFFFFFFFF;
        for (onyxU32 i = 0; i < textLength; ++i)
        {
            char32_t character = textU32[i];
            const Graphics::SDFFontGlyphData& glyph = glyphs.at(character);

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

                const Vector2f32 vertexPositionLeft = scaledAdvance * planeBoundLeft;
                const Vector2f32 vertexPositionRight = scaledAdvance * planeBoundRight;
                const Vector2f32 vertexPositionTop = verticalAdvance * planeBoundTop;
                const Vector2f32 vertexPositionBottom = verticalAdvance * planeBoundBottom;

                m_Vertices.emplace_back(Vector3f32(cursorPosition + vertexPositionTop + vertexPositionLeft, 0), Vector2f32{ atlasBoundLeft, atlasBoundTop }, color);
                m_Vertices.emplace_back(Vector3f32(cursorPosition + vertexPositionTop + vertexPositionRight, 0), Vector2f32{ atlasBoundRight, atlasBoundTop }, color);
                m_Vertices.emplace_back(Vector3f32(cursorPosition + vertexPositionBottom + vertexPositionRight, 0), Vector2f32{ atlasBoundRight, atlasBoundBottom }, color);
                m_Vertices.emplace_back(Vector3f32(cursorPosition + vertexPositionBottom + vertexPositionLeft, 0), Vector2f32{ atlasBoundLeft, atlasBoundBottom }, color);
            }

            Vector2f32 advance = advanceDirection * glyph.Advance * Size;
            if (i < (textLength - 1))
            {
                advance += advanceDirection * fontMetrics.Kerning * Size;
            }

            cursorPosition += advance;
        }
    }

    onyxF32 TextComponent::GetTextWidth(const String_U32& text)
    {
        const Graphics::SDFFontMetrics& fontMetrics = Font->GetMetrics();
        const HashMap<onyxU32, Graphics::SDFFontGlyphData>& glyphs = Font->GetGlyphs();

        const Vector2f32 advanceDirection(1.0f, 0.0f);

        onyxF32 totalWidth = 0.0f;
        const onyxU32 textLength = static_cast<onyxU32>(text.size());
        for (onyxU32 i = 0; i < textLength; ++i)
        {
            char32_t character = text[i];

            // handle multiline
            const Graphics::SDFFontGlyphData& glyph = glyphs.at(character);

            Vector2f32 advance = advanceDirection * glyph.Advance * Size;
            if (i < (textLength - 1))
            {
                advance += advanceDirection * fontMetrics.Kerning * Size;
            }

            totalWidth += advance[0];
        }

        return totalWidth;
    }
}

bool Serialization<GameCore::TextComponent>::Serialize(Serializer& serializer, const GameCore::TextComponent& textComponent)
{
    serializer.Write<"text">(textComponent.Text);
    serializer.Write<"size">(textComponent.Size);
    serializer.Write<"font">(textComponent.FontId);
    return false;
}

bool Serialization<GameCore::TextComponent>::Deserialize(const Deserializer& deserializer, GameCore::TextComponent& textComponent)
{
    deserializer.Read<"text">(textComponent.Text);
    deserializer.Read<"size">(textComponent.Size);

    onyxU64 fontId;
    deserializer.Read<"font">(fontId);
    textComponent.FontId = fontId;

    return false;
}

}
