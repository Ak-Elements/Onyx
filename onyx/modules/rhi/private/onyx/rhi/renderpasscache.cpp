#include <onyx/rhi/renderpasscache.h>
#include <onyx/rhi/graphicssystem.h>

namespace onyx::rhi
{
    RenderPassCache::RenderPassCache(GraphicsSystem& graphicsSystem)
        : m_GraphicsSystem(graphicsSystem)
    {
    }

    void RenderPassCache::Clear()
    {
        m_Cache.clear();
    }

    RenderPassHandle RenderPassCache::GetOrCreateRenderPass(const RenderPassSettings& settings)
    {
        onyxU64 renderPassHash = hash::FNV1aHash<onyxU64>(settings);

        auto it = m_Cache.find(renderPassHash);
        if (it != m_Cache.end())
            return it->second;

        RenderPassHandle renderPassHandle = m_GraphicsSystem.CreateRenderPass(settings);
        m_Cache[renderPassHash] = renderPassHandle;
        return renderPassHandle;
    }
}


