#pragma once

#include <onyx/noncopyable.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx::Graphics::Vulkan
{
    class VulkanGraphicsApi;
    class DescriptorSetLayout;
    class Device;
    class Shader;

    class PipelineLayout : public NonCopyable
    {
    public:
        PipelineLayout(const VulkanGraphicsApi& api, const DescriptorSetLayout& descriptorSetLayout);
        PipelineLayout(const VulkanGraphicsApi& api, const Shader& shader);

        ~PipelineLayout();

    private:
        const Device& m_Device;
        VULKAN_HANDLE(VkPipelineLayout, PipelineLayout, nullptr)
    };
}
