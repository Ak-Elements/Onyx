#include <onyx/graphics/texture.h>

namespace Onyx::Graphics
{
    Texture::Texture(const TextureProperties& properties, const TextureStorage* storage)
        : m_Properties(properties)
        , m_Storage(storage)
    {
    }
}
