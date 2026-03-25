#include <onyx/rhi/vulkan/shader.h>
#include <onyx/rhi/vulkan/vulkan.h>

#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/vulkan/device.h>
#include <onyx/rhi/vulkan/graphicsapi.h>

namespace onyx::rhi::vulkan {
ShaderModule::ShaderModule( VulkanGraphicsApi& api, Shader::ByteCode byteCode )
    : m_Api( api )
    , m_ByteCode( std::move( byteCode ) ) {
    VkShaderModuleCreateInfo moduleCreateInfo{};
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.codeSize = m_ByteCode.size() * sizeof( uint32_t );
    moduleCreateInfo.pCode = m_ByteCode.data();
    moduleCreateInfo.pNext = nullptr;

    VK_CHECK_RESULT( vkCreateShaderModule( api.GetDevice().GetHandle(), &moduleCreateInfo, NULL, &m_Module ) )
}

ShaderModule::~ShaderModule() {
    if ( m_Module != nullptr ) {
        vkDestroyShaderModule( m_Api.GetDevice().GetHandle(), m_Module, nullptr );
    }
}

Shader::~Shader() {
    m_Stages.clear();
}

bool Shader::AddStage( GraphicsSystem& graphicsSystem, ShaderStage stage, const ByteCode& byteCode ) {
    VulkanGraphicsApi& vulkanApi = graphicsSystem.GetApi< VulkanGraphicsApi >();

    const uint8_t stageIndex = enums::toIntegral( stage );
#if ONYX_ASSERT_ENABLED
    if ( stage == ShaderStage::Compute )
        ONYX_ASSERT( ( HasStage( ShaderStage::Vertex ) == false ) && ( HasStage( ShaderStage::Fragment ) == false ),
                     "Vertex/Fragment shader does not support compute stage." );
    else
        ONYX_ASSERT( IsComputeShader() == false,
                     "Compute shader does not support %s shader stage",
                     enums::toString( stage ) );
#endif
    UniquePtr< ShaderModule > module = makeUnique< ShaderModule >( vulkanApi, byteCode );

    VkShaderStageFlagBits vulkanStage = ToVulkanStage( stage );
    auto it = std::ranges::find_if(
        m_PipelineShaderStageCreateInfos,
        [ & ]( const VkPipelineShaderStageCreateInfo& info ) { return info.stage == vulkanStage; } );

    VkPipelineShaderStageCreateInfo& pipelineShaderStageCreateInfo = it == m_PipelineShaderStageCreateInfos.end()
                                                                         ? m_PipelineShaderStageCreateInfos
                                                                               .emplace_back()
                                                                         : *it;
    pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineShaderStageCreateInfo.stage = ToVulkanStage( stage );
    pipelineShaderStageCreateInfo.module = module->GetHandle();
    pipelineShaderStageCreateInfo.pName = "main";

    m_Stages[ stageIndex ] = std::move( module );
    return true;
}

void Shader::RemoveStage( ShaderStage stage ) {
    m_Stages[ enums::toIntegral( stage ) ].reset();
}

bool Shader::UpdateReflectionData( GraphicsSystem& graphicsSystem, ShaderReflectionInfo& reflectionInfo ) {
    VulkanGraphicsApi& vulkanApi = graphicsSystem.GetApi< VulkanGraphicsApi >();

    m_ReflectionInfo = reflectionInfo;

    const uint8_t descriptorSetCount = numericCast< uint8_t >( reflectionInfo.shaderDescriptorSets.size() );
    for ( uint8_t i = 0; i < descriptorSetCount; ++i ) {
        const ShaderDescriptorSet& shaderDescriptorSet = reflectionInfo.shaderDescriptorSets[ i ];
        m_DescriptorSetLayouts.emplace(
            makeUnique< DescriptorSetLayout >( vulkanApi.GetDevice(), shaderDescriptorSet ) );
    }

    return true;
}
} // namespace onyx::rhi::vulkan
