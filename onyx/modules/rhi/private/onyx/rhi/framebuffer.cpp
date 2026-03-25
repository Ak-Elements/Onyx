#include <onyx/rhi/framebuffer.h>

#include <onyx/rhi/texturestorage.h>

namespace onyx::rhi {
uint64_t FramebufferSettings::Hash() const {
    uint64_t hash = 0;
    hash = hash::fnV1aHash< uint64_t >( m_Width, hash );
    hash = hash::fnV1aHash< uint64_t >( m_Height, hash );
    hash = hash::fnV1aHash< uint64_t >( &m_LayerCount, hash );

    for ( auto colorTarget : m_ColorTargets )
        hash = hash::fnV1aHash< uint64_t >( colorTarget ? colorTarget.raw() : 0, hash );

    hash = hash::fnV1aHash< uint64_t >( m_ColorTargets.size(), hash );
    hash = hash::fnV1aHash< uint64_t >( m_DepthTarget ? m_DepthTarget.raw() : 0, hash );
    hash = hash::fnV1aHash< uint64_t >( m_RenderPass.raw(), hash );
    // include color and depth target in hash
    // hash::FNV1aHash64(reinterpret_cast<uint8_t*>(m_DepthTarget), sizeof(uint64_t), hash);

    return hash;
}

Framebuffer::Framebuffer( const FramebufferSettings& settings )
    : m_Settings( settings ) {}

Framebuffer::~Framebuffer() {}
} // namespace onyx::rhi
