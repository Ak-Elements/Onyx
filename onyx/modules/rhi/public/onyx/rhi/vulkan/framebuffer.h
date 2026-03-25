#pragma once

#include <onyx/rhi/framebuffer.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan {
class VulkanGraphicsApi;
class Device;

class VulkanFramebuffer : public rhi::Framebuffer {
  public:
    ~VulkanFramebuffer() override;

    void Init( const VulkanGraphicsApi& api, const FramebufferSettings& settings );

    // uint32_t AddAttachment(AttachmentCreateInfo createinfo, VkImage* existingImage);
    // VkResult CreateSampler(VkFilter magFilter, VkFilter minFilter, VkSamplerAddressMode adressMode);
  private:
    const Device* m_Device;

    VULKAN_HANDLE( VkFramebuffer, Framebuffer, nullptr )
};
} // namespace onyx::rhi::vulkan
