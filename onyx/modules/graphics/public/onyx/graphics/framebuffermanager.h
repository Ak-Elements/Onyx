#pragma once

#include <onyx/noncopyable.h>
#include <onyx/onyx_types.h>

namespace Onyx::Graphics
{
    struct FramebufferSettings;
    class Texture;
    class Framebuffer;
    class GraphicsApi;
    class RenderPassCache;

    class FramebufferCache : NonCopyable
    {
        using FramebufferKey = onyxU32;
    public:
        FramebufferCache(GraphicsApi& graphicsApi);

        const Framebuffer& GetFramebuffer(FramebufferKey& key);
        FramebufferKey GetFramebuffer(const FramebufferSettings& frameBufferSettings, Framebuffer& outFrameBuffer);

        void OnResize(onyxU32 width, onyxU32 height);

    private:
        GraphicsApi& m_GraphicsApi;

        HashMap<FramebufferKey, UniquePtr<Framebuffer>> m_Framebuffers;
        DynamicArray<UniquePtr<Framebuffer>> m_SwapchainFramebuffers;
    };
}
