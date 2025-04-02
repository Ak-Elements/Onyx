#include <onyx/graphics/vulkan/pipelinelayout.h>

#include <onyx/graphics/vulkan/device.h>
#include <onyx/graphics/vulkan/descriptorsetlayout.h>
#include <onyx/graphics/vulkan/graphicsapi.h>
#include <onyx/graphics/vulkan/shader.h>

namespace Onyx::Graphics::Vulkan
{
    PipelineLayout::PipelineLayout(const VulkanGraphicsApi& api, const DescriptorSetLayout& descriptorSetLayout)
        : m_Device(api.GetDevice())
    {
        VkDescriptorSetLayout descriptorSetLayouts[] = {descriptorSetLayout.GetHandle()};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts;
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        VK_CHECK_RESULT(vkCreatePipelineLayout(m_Device.GetHandle(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout));
    }

    PipelineLayout::PipelineLayout(const VulkanGraphicsApi& api, const Shader& shader)
        : m_Device(api.GetDevice())
    {
        //// Descriptor set layouts
        const auto& descriptorSetLayouts = shader.GetDescriptorSetLayouts();
        const onyxU32 descriptorSetLayoutsCount = static_cast<onyxU32>(descriptorSetLayouts.size());

        // + 1 for bindless
        DynamicArray<VkDescriptorSetLayout> vkDescriptorSetLayouts;
        vkDescriptorSetLayouts.reserve(descriptorSetLayoutsCount + 1);

        if (api.IsBindless())
            vkDescriptorSetLayouts.push_back(api.GetBindlessDescriptorSetLayout().GetHandle());

        for (const UniquePtr<DescriptorSetLayout>& descriptorSetLayout : descriptorSetLayouts)
        {
            if (descriptorSetLayout == nullptr)
                continue;

            vkDescriptorSetLayouts.push_back(descriptorSetLayout->GetHandle());
        }
        //// Push constant ranges
        const DynamicArray<PushConstantRange>& pushConstantRanges = shader.GetPushConstantRanges();
        const onyxU32 pushConstantRangesCount = static_cast<onyxU32>(pushConstantRanges.size());

        DynamicArray<VkPushConstantRange> vkPushConstantRanges;
        for (const auto& pushConstantRange : pushConstantRanges)
            vkPushConstantRanges.emplace_back(ToVulkanStage(pushConstantRange.Stage), pushConstantRange.Offset, pushConstantRange.Size);

        //// Create layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<onyxU32>(vkDescriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = vkDescriptorSetLayouts.data();

        pipelineLayoutInfo.pushConstantRangeCount = pushConstantRangesCount;
        pipelineLayoutInfo.pPushConstantRanges = vkPushConstantRanges.data();

        VK_CHECK_RESULT(vkCreatePipelineLayout(m_Device.GetHandle(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout))
    }

    PipelineLayout::~PipelineLayout()
    {
        if (m_PipelineLayout != nullptr)
        {
            vkDestroyPipelineLayout(m_Device.GetHandle(), m_PipelineLayout, nullptr);
            m_PipelineLayout = nullptr;
        }
    }
}
