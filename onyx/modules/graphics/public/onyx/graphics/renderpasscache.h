#pragma once

#include <onyx/graphics/renderpass.h>
#include <onyx/graphics/graphicshandles.h>

namespace Onyx::Graphics
{
    class GraphicsApi;

    class RenderPassCache
    {
    public:
        RenderPassCache(GraphicsApi& graphicsApi);
        void Clear();

        RenderPassHandle GetOrCreateRenderPass(const RenderPassSettings& settings);

    private:
        GraphicsApi& m_GraphicsApi;

        HashMap<onyxU64, RenderPassHandle> m_Cache;
    };
}
