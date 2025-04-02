#pragma once

#include <onyx/assets/asset.h>
#include <onyx/graphics/graphicstypes.h>
#include <onyx/graphics/textureasset.h>

namespace Onyx::GameCore
{
    struct SDFFontMetrics
    {
        onyxU32 GlpyhSize = 0;
        Vector2u32 AtlasSize;
        onyxU32 Size = 0;

        onyxF32 LineHeight = 0.0f;
        onyxF32 Ascender = 0.0f;
        onyxF32 Descender = 0.0f;
        onyxF32 UnderLineY = 0.0f;
        onyxF32 UnderlineThickness = 0.0f;
        onyxF32 Kerning = 0.0f;

    };

    struct SDFFontGlyphData
    {
        onyxF32 Advance = 0.0f;

        Graphics::Rect2Df32 PlaneBounds;
        Graphics::Rect2Df32 AtlasBounds; // converted already to UV space
    };

    class SDFFont : public Assets::Asset<SDFFont>
    {
    public:
        void SetTexture(const Reference<Graphics::TextureAsset>& fontTexture) { m_FontTexture = fontTexture; }
        const Reference<Graphics::TextureAsset>& GetTexture() const { return m_FontTexture; }

        SDFFontMetrics& GetMetrics() { return m_Metrics; }
        const SDFFontMetrics& GetMetrics() const { return m_Metrics; }

        HashMap<onyxU32, SDFFontGlyphData>& GetGlyphs() { return m_Glyphs; }
        const HashMap<onyxU32, SDFFontGlyphData>& GetGlyphs() const { return m_Glyphs; }

    private:
        Reference<Graphics::TextureAsset> m_FontTexture;

        SDFFontMetrics m_Metrics;
        HashMap<onyxU32, SDFFontGlyphData> m_Glyphs;
    };
}
