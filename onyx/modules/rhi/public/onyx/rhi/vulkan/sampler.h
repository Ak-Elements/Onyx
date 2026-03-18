#pragma once

#include <onyx/rhi/sampler.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan
{
    class Device;

    class Sampler : public rhi::Sampler
    {
    public:
        Sampler(const Device& device, const SamplerProperties& properties);
        ~Sampler() override;

    private:
        const Device& m_Device;
        VULKAN_HANDLE(VkSampler, Sampler, nullptr);
    };
}
