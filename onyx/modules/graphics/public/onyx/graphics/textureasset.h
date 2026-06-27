#pragma once
#include <onyx/assets/asset.h>

#include <onyx/rhi/graphicshandles.h>

namespace onyx::graphics {
class TextureAsset : public assets::Asset< TextureAsset > {
  public:
    static constexpr StringId32 TypeId{ "onyx::graphics::assets::Texture" };
    static StringId32 getTypeId() { return TypeId; }

    const rhi::TextureHandle& getTextureHandle() const { return m_texture; }

  private:
    friend struct TextureSerializer;
    void setTexture( const rhi::TextureHandle& handle );

  private:
    rhi::TextureHandle m_texture;
};
} // namespace onyx::graphics
