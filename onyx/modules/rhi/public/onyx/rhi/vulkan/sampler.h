#pragma once

#include <onyx/rhi/sampler.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan {
class Device;

class Sampler : public rhi::Sampler {
  public:
    Sampler( const Device& device, const SamplerProperties& properties );
    ~Sampler() override;

    [[nodiscard]] const VkDescriptorImageInfo& getDescriptorInfo() const { return m_descriptorInfo; }

  private:
    const Device& m_device;
    VULKAN_HANDLE( VkSampler, Sampler, nullptr );

    VkDescriptorImageInfo m_descriptorInfo;
};
} // namespace onyx::rhi::vulkan
