#include <onyx/rhi/vulkan/pipelinelayout.h>

#include <onyx/rhi/vulkan/descriptorsetlayout.h>
#include <onyx/rhi/vulkan/device.h>
#include <onyx/rhi/vulkan/graphicsapi.h>
#include <onyx/rhi/vulkan/shader.h>

namespace onyx::rhi::vulkan {
PipelineLayout::PipelineLayout( const VulkanGraphicsApi& api, const DescriptorSetLayout& descriptorSetLayout )
    : m_Device( api.getDevice() ) {
    VkDescriptorSetLayout descriptorSetLayouts[] = { descriptorSetLayout.GetHandle() };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts;
    pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    VK_CHECK_RESULT( vkCreatePipelineLayout( m_Device.GetHandle(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout ) );
}

PipelineLayout::PipelineLayout( const VulkanGraphicsApi& api, const Shader& shader )
    : m_Device( api.getDevice() ) {
    //// Descriptor set layouts
    const auto& descriptorSetLayouts = shader.GetDescriptorSetLayouts();
    const uint32_t descriptorSetLayoutsCount = static_cast< uint32_t >( descriptorSetLayouts.size() );

    // + 1 for bindless
    DynamicArray< VkDescriptorSetLayout > vkDescriptorSetLayouts;
    vkDescriptorSetLayouts.reserve( descriptorSetLayoutsCount + 1 );

    if( api.isBindless() )
        vkDescriptorSetLayouts.push_back( api.getBindlessDescriptorSetLayout().GetHandle() );

    for( const UniquePtr< DescriptorSetLayout >& descriptorSetLayout : descriptorSetLayouts ) {
        if( descriptorSetLayout == nullptr )
            continue;

        vkDescriptorSetLayouts.push_back( descriptorSetLayout->GetHandle() );
    }
    //// Push constant ranges
    const DynamicArray< PushConstantRange >& pushConstantRanges = shader.GetPushConstantRanges();
    const uint32_t pushConstantRangesCount = static_cast< uint32_t >( pushConstantRanges.size() );

    DynamicArray< VkPushConstantRange > vkPushConstantRanges;
    for( const auto& pushConstantRange : pushConstantRanges )
        vkPushConstantRanges.emplace_back( ToVulkanStage( pushConstantRange.Stage ),
                                           pushConstantRange.Offset,
                                           pushConstantRange.Size );

    //// Create layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast< uint32_t >( vkDescriptorSetLayouts.size() );
    pipelineLayoutInfo.pSetLayouts = vkDescriptorSetLayouts.data();

    pipelineLayoutInfo.pushConstantRangeCount = pushConstantRangesCount;
    pipelineLayoutInfo.pPushConstantRanges = vkPushConstantRanges.data();

    VK_CHECK_RESULT( vkCreatePipelineLayout( m_Device.GetHandle(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout ) )
}

PipelineLayout::~PipelineLayout() {
    if( m_PipelineLayout != nullptr ) {
        vkDestroyPipelineLayout( m_Device.GetHandle(), m_PipelineLayout, nullptr );
        m_PipelineLayout = nullptr;
    }
}
} // namespace onyx::rhi::vulkan
