#include <onyx/rhi/texture.h>

namespace onyx::rhi
{
    Texture::Texture(const TextureProperties& properties, const TextureStorage* storage)
        : m_Properties(properties)
        , m_Storage(storage)
    {
    }
}
