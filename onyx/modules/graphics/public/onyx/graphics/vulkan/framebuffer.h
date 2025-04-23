#pragma once

#include <onyx/graphics/framebuffer.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx::Graphics::Vulkan
{
    class VulkanGraphicsApi;
    class Device;
	
	class VulkanFramebuffer : public Graphics::Framebuffer
	{
	public:
	    ~VulkanFramebuffer() override;

		void Init(const VulkanGraphicsApi& api, const FramebufferSettings& settings);

        //onyxU32 AddAttachment(AttachmentCreateInfo createinfo, VkImage* existingImage);
        //VkResult CreateSampler(VkFilter magFilter, VkFilter minFilter, VkSamplerAddressMode adressMode);
	private:
        const Device* m_Device;

        VULKAN_HANDLE(VkFramebuffer, Framebuffer, nullptr)
	};
}
