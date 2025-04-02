#pragma once

#include <onyx/noncopyable.h>

#include <onyx/graphics/graphicshandles.h>

namespace Onyx::Graphics
{
    struct FramebufferSettings;
    class GraphicsApi;
    class RenderPassCache;

    class FramebufferCache : public NonCopyable
    {
    public:
        FramebufferCache(GraphicsApi& graphicsApi);

        void Clear();

        FramebufferHandle GetOrCreateFramebuffer(const FramebufferSettings& settings);

    private:
        GraphicsApi& m_GraphicsApi;

        HashMap<onyxU64, FramebufferHandle> m_Cache;
    };
}
