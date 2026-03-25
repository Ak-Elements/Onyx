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
    uint32_t KeyCode;
    float32 Advance = 0.0f;

    Rect2f32 PlaneBounds;
    Rect2f32 AtlasBounds; // converted already to UV space
};

class SDFFont : public assets::Asset< SDFFont > {
  public:
    static constexpr StringId32 TypeId{ "onyx::graphics::assets::SDFFont" };
    StringId32 GetTypeId() const { return TypeId; }

    void SetTexture( const assets::AssetHandle< TextureAsset >& fontTexture ) { m_FontTexture = fontTexture; }
    const assets::AssetHandle< TextureAsset >& GetTexture() const { return m_FontTexture; }

    SDFFontMetrics& GetMetrics() { return m_Metrics; }
    const SDFFontMetrics& GetMetrics() const { return m_Metrics; }

    HashMap< uint32_t, SDFFontGlyphData >& GetGlyphs() { return m_Glyphs; }
    const HashMap< uint32_t, SDFFontGlyphData >& GetGlyphs() const { return m_Glyphs; }

  private:
    HashMap< uint32_t, SDFFontGlyphData > m_Glyphs;
    SDFFontMetrics m_Metrics;

    assets::AssetHandle< TextureAsset > m_FontTexture;
};
} // namespace onyx::graphics
