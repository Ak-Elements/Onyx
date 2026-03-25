#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi {
struct ShaderDescriptorSet;

namespace vulkan {
class Device;

class DescriptorSetLayout : public NonCopyable {
  public:
    DescriptorSetLayout( const Device& device, uint8_t set, const VkDescriptorSetLayoutCreateInfo& createInfo );
    DescriptorSetLayout( const Device& device, const ShaderDescriptorSet& descriptorSet );
    ~DescriptorSetLayout();

    uint8_t GetSet() const { return m_Set; }
    const HashMap< String, VkWriteDescriptorSet >& GetWriteDescriptors() const { return m_WriteDescriptorSets; }

  private:
    const Device& m_Device;
    uint8_t m_Set = static_cast< uint8_t >( InvalidIndex8 );

    VULKAN_HANDLE( VkDescriptorSetLayout, Layout, nullptr );
    HashMap< String, VkWriteDescriptorSet > m_WriteDescriptorSets;
};
} // namespace vulkan
} // namespace onyx::rhi
