#include <onyx/rhi/vulkan/framebuffer.h>

#include <onyx/rhi/vulkan/device.h>
#include <onyx/rhi/vulkan/graphicsapi.h>
#include <onyx/rhi/vulkan/renderpass.h>
#include <onyx/rhi/vulkan/texture.h>

#include <onyx/rhi/graphicshandles.h>

namespace onyx::rhi::vulkan {
void VulkanFramebuffer::Init( const VulkanGraphicsApi& api, const FramebufferSettings& settings ) {
    m_Device = &api.getDevice();
    m_Settings = settings;

    // Don't create a framebuffer if we use dynamic rendering
    if ( api.isDynamicRenderingEnabled() )
        return;

    VkFramebufferCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = settings.m_RenderPass.as< VulkanRenderPass >().GetHandle();
    createInfo.width = settings.m_Width;
    createInfo.height = settings.m_Height;
    createInfo.layers = settings.m_LayerCount;
    createInfo.pNext = nullptr;

    InplaceArray< VkImageView, MAX_RENDERPASS_ATTACHMENTS > attachments;

    for ( const TextureViewHandle& colorTargetHandle : settings.m_ColorTargets ) {
        const VulkanTexture& colorTarget = colorTargetHandle.as< VulkanTexture >();
        attachments.add( colorTarget.GetHandle() );
    }

    if ( settings.m_DepthTarget ) {
        const VulkanTexture& depthStencilTarget = settings.m_DepthTarget.as< VulkanTexture >();
        attachments.add( depthStencilTarget.GetHandle() );
    }

    createInfo.attachmentCount = attachments.size();
    createInfo.pAttachments = attachments.data();

    VK_CHECK_RESULT( vkCreateFramebuffer( m_Device->GetHandle(), &createInfo, nullptr, &m_Framebuffer ) );
    // TODO: Set debug name
}

VulkanFramebuffer::~VulkanFramebuffer() {
    vkDestroyFramebuffer( m_Device->GetHandle(), m_Framebuffer, nullptr );
}
} // namespace onyx::rhi::vulkan
