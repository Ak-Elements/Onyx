#include <onyx/graphics/textureasset.h>

namespace Onyx::Graphics
{
    void TextureAsset::SetTexture(const TextureHandle& handle)
    {
        m_Texture = handle;
    }
}
