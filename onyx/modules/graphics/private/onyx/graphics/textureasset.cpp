#include <onyx/graphics/textureasset.h>

namespace onyx::graphics {
void TextureAsset::setTexture( const rhi::TextureHandle& handle ) {
    m_texture = handle;
}
} // namespace onyx::graphics
