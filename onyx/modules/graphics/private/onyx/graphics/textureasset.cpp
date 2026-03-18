#include <onyx/graphics/textureasset.h>

namespace onyx::graphics
{
    void TextureAsset::SetTexture(const rhi::TextureHandle& handle)
    {
        m_Texture = handle;
    }
}
