#pragma once

#include <onyx/assets/asset.h>
#include <onyx/graphics/textureasset.h>
#include <onyx/geometry/rect2.h>

namespace Onyx::Input
{
    enum class Key : onyxU16;
}

namespace Onyx::Graphics
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
        onyxU32 KeyCode;
        onyxF32 Advance = 0.0f;

        Rect2f32 PlaneBounds;
        Rect2f32 AtlasBounds; // converted already to UV space
    };

    class SDFFont : public Assets::Asset<SDFFont>
    {
    public:
        static constexpr StringId32 TypeId{ "Onyx::Graphics::Assets::SDFFont" };
        StringId32 GetTypeId() const { return TypeId; }

        void SetTextureId(const Assets::AssetId& textureId) { m_FontTextureId = textureId; }
        Assets::AssetId GetTextureId() const { return m_FontTextureId; }

        void SetTexture(const Reference<TextureAsset>& fontTexture) { m_FontTexture = fontTexture; }
        const Reference<TextureAsset>& GetTexture() const { return m_FontTexture; }

        SDFFontMetrics& GetMetrics() { return m_Metrics; }
        const SDFFontMetrics& GetMetrics() const { return m_Metrics; }

        HashMap<onyxU32, SDFFontGlyphData>& GetGlyphs() { return m_Glyphs; }
        const HashMap<onyxU32, SDFFontGlyphData>& GetGlyphs() const { return m_Glyphs; }

    private:
        HashMap<onyxU32, SDFFontGlyphData> m_Glyphs;
        SDFFontMetrics m_Metrics;

        Assets::AssetId m_FontTextureId;
        Reference<TextureAsset> m_FontTexture;
    };
}
