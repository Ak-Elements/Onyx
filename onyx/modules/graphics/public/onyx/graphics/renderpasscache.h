#pragma once

#include <onyx/graphics/renderpass.h>
#include <onyx/graphics/graphicshandles.h>

namespace Onyx::Graphics
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
