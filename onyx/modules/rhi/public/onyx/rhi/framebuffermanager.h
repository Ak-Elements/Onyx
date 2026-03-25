#pragma once

#include <onyx/noncopyable.h>
#include <onyx/onyx_types.h>

namespace onyx::rhi {
struct FramebufferSettings;
class Texture;
class Framebuffer;
class GraphicsApi;
class RenderPassCache;

class FramebufferCache : NonCopyable {
    using FramebufferKey = uint32_t;

  public:
    FramebufferCache( GraphicsApi& graphicsApi );

    const Framebuffer& GetFramebuffer( FramebufferKey& key );
    FramebufferKey GetFramebuffer( const FramebufferSettings& frameBufferSettings, Framebuffer& outFrameBuffer );

    void OnResize( uint32_t width, uint32_t height );

  private:
    GraphicsApi& m_GraphicsApi;

    HashMap< FramebufferKey, UniquePtr< Framebuffer > > m_Framebuffers;
    DynamicArray< UniquePtr< Framebuffer > > m_SwapchainFramebuffers;
};
} // namespace onyx::rhi
