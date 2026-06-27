#pragma once

#include <onyx/assets/asset.h>
#include <onyx/geometry/rect2.h>
#include <onyx/graphics/textureasset.h>

namespace onyx::input {
enum class Key : uint16_t;
}

namespace onyx::graphics {
struct SDFFontMetrics {
    uint32_t GlpyhSize = 0;
    Vector2u32 AtlasSize;
    uint32_t Size = 0;

    float32 LineHeight = 0.0f;
    float32 Ascender = 0.0f;
    float32 Descender = 0.0f;
    float32 UnderLineY = 0.0f;
    float32 UnderlineThickness = 0.0f;
    float32 Kerning = 0.0f;
};

struct SDFFontGlyphData {
    uint32_t KeyCode = 0;
    float32 Advance = 0.0f;

    Rect2f32 PlaneBounds;
    Rect2f32 AtlasBounds; // converted already to UV space
};

class SDFFont : public assets::Asset< SDFFont > {
  public:
    static constexpr StringId32 TypeId{ "onyx::graphics::assets::SDFFont" };
    static StringId32 getTypeId() { return TypeId; }

    void setTexture( const assets::AssetHandle< TextureAsset >& fontTexture ) { m_fontTexture = fontTexture; }
    const assets::AssetHandle< TextureAsset >& getTexture() const { return m_fontTexture; }

    SDFFontMetrics& getMetrics() { return m_metrics; }
    const SDFFontMetrics& getMetrics() const { return m_metrics; }

    HashMap< uint32_t, SDFFontGlyphData >& getGlyphs() { return m_glyphs; }
    const HashMap< uint32_t, SDFFontGlyphData >& getGlyphs() const { return m_glyphs; }

  private:
    HashMap< uint32_t, SDFFontGlyphData > m_glyphs;
    SDFFontMetrics m_metrics;

    assets::AssetHandle< TextureAsset > m_fontTexture;
};
} // namespace onyx::graphics
