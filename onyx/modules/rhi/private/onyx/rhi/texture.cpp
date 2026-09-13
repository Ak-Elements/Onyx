#include <onyx/rhi/texture.h>

namespace onyx::rhi {
Texture::Texture( const TextureProperties& properties, const TextureStorage* storage )
    : m_properties( properties )
    , m_storage( storage ) {}
} // namespace onyx::rhi
