#pragma once

#include <onyx/rhi/renderpass.h>
#include <onyx/rhi/graphicshandles.h>

namespace onyx::rhi
{
    class GraphicsSystem;

    class RenderPassCache
    {
    public:
        RenderPassCache(GraphicsSystem& graphicsSystem);
        void Clear();

        RenderPassHandle GetOrCreateRenderPass(const RenderPassSettings& settings);

    private:
        GraphicsSystem& m_GraphicsSystem;

        HashMap<onyxU64, RenderPassHandle> m_Cache;
    };
}
