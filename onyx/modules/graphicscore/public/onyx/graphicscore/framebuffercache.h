#pragma once

#include <onyx/noncopyable.h>

#include <onyx/graphicscore/graphicshandles.h>

namespace Onyx::Graphics
{
    struct FramebufferSettings;
    class GraphicsSystem;
    class RenderPassCache;

    class FramebufferCache : public NonCopyable
    {
    public:
        FramebufferCache(GraphicsSystem& graphicsSystem);

        void Clear();

        FramebufferHandle GetOrCreateFramebuffer(const FramebufferSettings& settings);

    private:
        GraphicsSystem& m_GraphicsSystem;

        HashMap<onyxU64, FramebufferHandle> m_Cache;
    };
}
