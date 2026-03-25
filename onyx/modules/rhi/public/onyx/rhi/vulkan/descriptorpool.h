#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan {
class Device;

class DescriptorPool : public NonCopyable {
  public:
    DescriptorPool( const Device& device,
                    const DynamicArray< VkDescriptorPoolSize >& poolSizes,
                    VkDescriptorPoolCreateFlagBits poolCreationFlags,
                    uint32_t maxSets );
    ~DescriptorPool();

  private:
    const Device& m_Device;
    VULKAN_HANDLE( VkDescriptorPool, DescriptorPool, nullptr );
};
} // namespace onyx::rhi::vulkan
