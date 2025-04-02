#pragma once

#include <onyx/graphics/sampler.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx::Graphics::Vulkan
{
    class Device;

    class Sampler : public Graphics::Sampler
    {
    public:
        Sampler(const Device& device, const SamplerProperties& properties);
        ~Sampler() override;

    private:
        const Device& m_Device;
        VULKAN_HANDLE(VkSampler, Sampler, nullptr);
    };
}
