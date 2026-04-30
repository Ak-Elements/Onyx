#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/renderpasscache.h>

namespace onyx::rhi {
RenderPassCache::RenderPassCache( GraphicsSystem& graphicsSystem )
    : m_GraphicsSystem( graphicsSystem ) {}

void RenderPassCache::Clear() {
    m_Cache.clear();
}

RenderPassHandle RenderPassCache::GetOrCreateRenderPass( const RenderPassSettings& settings ) {
    uint64_t renderPassHash = hash::fnV1aHash< uint64_t >( settings );

    auto it = m_Cache.find( renderPassHash );
    if ( it != m_Cache.end() )
        return it->second;

    RenderPassHandle renderPassHandle = m_GraphicsSystem.createRenderPass( settings );
    m_Cache[ renderPassHash ] = renderPassHandle;
    return renderPassHandle;
}
} // namespace onyx::rhi
