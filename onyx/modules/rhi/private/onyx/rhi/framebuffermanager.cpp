#include <onyx/graphics/framebuffermanager.h>
#include <onyx/graphics/vulkan/renderpass.h>
#include <onyx/graphics/vulkan/swapchain.h>
#include <onyx/graphics/vulkan/texture.h>
#include <onyx/hash.h>

#include <vulkan/vulkan.h>

namespace onyx::rhi {
FramebufferCache::FramebufferCache( GraphicsApi& graphicsApi )
    : m_GraphicsApi( graphicsApi ) {}

const Framebuffer& FramebufferCache::GetFramebuffer( FramebufferKey& key ) {
    return *m_Framebuffers[ key ];
}

FramebufferCache::FramebufferKey FramebufferCache::GetFramebuffer( const FramebufferSettings& frameBufferSettings,
                                                                   Framebuffer& /*outFrameBuffer*/ ) {
    // add custom hash function faster than std::hash

    FramebufferKey key = hash::FNV1aHash32( reinterpret_cast< const uint8_t* >( &frameBufferSettings ),
                                            sizeof( FramebufferSettings ) );
    if ( m_Framebuffers.contains( key ) == false ) {
        m_Framebuffers[ key ] = m_GraphicsApi.CreateFramebuffer( frameBufferSettings );
        // m_Framebuffers[key] = Framebuffer(colorTargets, depthTarget);
    }

    // outFrameBuffer = *m_Framebuffers[key];
    return key;
}

void FramebufferCache::OnResize( uint32_t width, uint32_t height ) {
    m_SwapchainFramebuffers.clear();

    FramebufferSettings settings;
    settings.m_Height = height;
    settings.m_Width = width;

    // TODO: Reimplement

    for ( uint32_t i = 0; i < m_GraphicsApi.GetSwapchainImageCount(); i++ ) {
        // m_SwapchainFramebuffers.push_back(m_GraphicsApi.CreateFramebuffer(settings));

        // VKUtils::SetDebugUtilsObjectName(m_Device->GetVulkanDevice(), VK_OBJECT_TYPE_FRAMEBUFFER,
        // fmt::format("Swapchain framebuffer (Frame in flight: {})", i), m_Framebuffers[i]);
    }
}
} // namespace onyx::rhi
