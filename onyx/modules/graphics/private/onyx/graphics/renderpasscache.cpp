#include <onyx/graphics/renderpasscache.h>
#include <onyx/graphics/graphicsapi.h>

namespace Onyx::Graphics
{
    RenderPassCache::RenderPassCache(GraphicsApi& graphicsApi)
        : m_GraphicsApi(graphicsApi)
    {
    }

    void RenderPassCache::Clear()
    {
        m_Cache.clear();
    }

    RenderPassHandle RenderPassCache::GetOrCreateRenderPass(const RenderPassSettings& settings)
    {
        onyxU64 renderPassHash = Hash::FNV1aHash64(reinterpret_cast<const onyxU8*>(&settings), sizeof(RenderPassSettings));

        auto it = m_Cache.find(renderPassHash);
        if (it != m_Cache.end())
            return it->second;

        RenderPassHandle renderPassHandle = m_GraphicsApi.CreateRenderPass(settings);
        m_Cache[renderPassHash] = renderPassHandle;
        return renderPassHandle;
    }
}


