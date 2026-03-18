#pragma once

#include <onyx/assets/asset.h>
#include <onyx/graphics/textureasset.h>
#include <onyx/geometry/rect2.h>

namespace onyx::input
{
    enum class Key : onyxU16;
}

namespace onyx::graphics
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

    class SDFFont : public assets::Asset<SDFFont>
    {
    public:
        static constexpr StringId32 TypeId{ "onyx::graphics::assets::SDFFont" };
        StringId32 GetTypeId() const { return TypeId; }

        void SetTexture(const assets::AssetHandle<TextureAsset>& fontTexture) { m_FontTexture = fontTexture; }
        const assets::AssetHandle<TextureAsset>& GetTexture() const { return m_FontTexture; }

        SDFFontMetrics& GetMetrics() { return m_Metrics; }
        const SDFFontMetrics& GetMetrics() const { return m_Metrics; }

        HashMap<onyxU32, SDFFontGlyphData>& GetGlyphs() { return m_Glyphs; }
        const HashMap<onyxU32, SDFFontGlyphData>& GetGlyphs() const { return m_Glyphs; }

    private:
        HashMap<onyxU32, SDFFontGlyphData> m_Glyphs;
        SDFFontMetrics m_Metrics;

        assets::AssetHandle<TextureAsset> m_FontTexture;
    };
}
