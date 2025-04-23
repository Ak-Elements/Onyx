#include <onyx/graphics/vulkan/framebuffer.h>

#include <onyx/graphics/vulkan/device.h>
#include <onyx/graphics/vulkan/graphicsapi.h>
#include <onyx/graphics/vulkan/renderpass.h>
#include <onyx/graphics/vulkan/texture.h>

#include <onyx/graphics/graphicshandles.h>

namespace Onyx::Graphics::Vulkan
{
    void VulkanFramebuffer::Init(const VulkanGraphicsApi& api, const FramebufferSettings& settings)
    {
        m_Device = &api.GetDevice();
        m_Settings = settings;

        // Don't create a framebuffer if we use dynamic rendering
        if (api.IsDynamicRenderingEnabled())
            return;

        VkFramebufferCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = settings.m_RenderPass.As<VulkanRenderPass>().GetHandle();
        createInfo.width = settings.m_Width;
        createInfo.height = settings.m_Height;
        createInfo.layers = settings.m_LayerCount;
        createInfo.pNext = nullptr;

        InplaceArray<VkImageView, MAX_RENDERPASS_ATTACHMENTS> attachments;

        for (const TextureViewHandle& colorTargetHandle : settings.m_ColorTargets)
        {
            const VulkanTexture& colorTarget = colorTargetHandle.As<VulkanTexture>();
            attachments.Add(colorTarget.GetHandle());
        }

        if (settings.m_DepthTarget)
        {
            const VulkanTexture& depthStencilTarget = settings.m_DepthTarget.As<VulkanTexture>();
            attachments.Add(depthStencilTarget.GetHandle());
        }

        createInfo.attachmentCount = attachments.size();
        createInfo.pAttachments = attachments.data();

        VK_CHECK_RESULT(vkCreateFramebuffer(m_Device->GetHandle(), &createInfo, nullptr, &m_Framebuffer));
        //TODO: Set debug name
    }

    VulkanFramebuffer::~VulkanFramebuffer()
    {
        vkDestroyFramebuffer(m_Device->GetHandle(), m_Framebuffer, nullptr);
    }
}
