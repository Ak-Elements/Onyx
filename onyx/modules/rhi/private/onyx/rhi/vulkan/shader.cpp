#include <onyx/rhi/vulkan/shader.h>
#include <onyx/rhi/vulkan/vulkan.h>

#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/vulkan/device.h>
#include <onyx/rhi/vulkan/graphicsapi.h>

namespace onyx::rhi::vulkan {
ShaderModule::ShaderModule( const VulkanGraphicsApi& api, Span< const uint8_t > byteCode )
    : m_api( api )
    , m_byteCode( byteCode.size(), byteCode.data() ) {
    VkShaderModuleCreateInfo moduleCreateInfo{};
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.codeSize = byteCode.size();
    moduleCreateInfo.pCode = std::bit_cast< uint32_t* >( byteCode.data() );
    moduleCreateInfo.pNext = nullptr;

    VK_CHECK_RESULT( vkCreateShaderModule( api.getDevice().GetHandle(), &moduleCreateInfo, nullptr, &m_Module ) )
}

ShaderModule::~ShaderModule() {
    if( m_Module != nullptr ) {
        vkDestroyShaderModule( m_api.getDevice().GetHandle(), m_Module, nullptr );
    }
}

Shader::~Shader() = default;

// bool Shader::addStage( GraphicsSystem& graphicsSystem, ShaderStage stage, const ByteCode& byteCode ) {
//     VulkanGraphicsApi& vulkanApi = graphicsSystem.getApi< VulkanGraphicsApi >();
//
//     const uint8_t stageIndex = enums::toIntegral( stage );
// #if ONYX_ASSERT_ENABLED
//     if( stage == ShaderStage::Compute )
//         ONYX_ASSERT( ( hasStage( ShaderStage::Vertex ) == false ) && ( hasStage( ShaderStage::Fragment ) == false ),
//                      "Vertex/Fragment shader does not support compute stage." );
//     else
//         ONYX_ASSERT( isComputeShader() == false,
//                      "Compute shader does not support %s shader stage",
//                      enums::toString( stage ) );
// #endif
//     UniquePtr< ShaderModule > module = makeUnique< ShaderModule >( vulkanApi, byteCode );
//
//     VkShaderStageFlagBits vulkanStage = ToVulkanStage( stage );
//     auto it = std::ranges::find_if(
//         m_pipelineShaderStageCreateInfos,
//         [ & ]( const VkPipelineShaderStageCreateInfo& info ) { return info.stage == vulkanStage; } );
//
//     VkPipelineShaderStageCreateInfo& pipelineShaderStageCreateInfo = it == m_pipelineShaderStageCreateInfos.end()
//                                                                          ? m_pipelineShaderStageCreateInfos
//                                                                                .emplace_back()
//                                                                          : *it;
//     pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//     pipelineShaderStageCreateInfo.stage = ToVulkanStage( stage );
//     pipelineShaderStageCreateInfo.module = module->GetHandle();
//     pipelineShaderStageCreateInfo.pName = "main";
//
//     m_stages[ stageIndex ] = std::move( module );
//     return true;
// }

void Shader::create( const GraphicsSystem& graphicsSystem,
                     Span< const uint8_t > byteCode,
                     ShaderReflectionInfo& reflectionInfo ) {
    const VulkanGraphicsApi& vulkanApi = graphicsSystem.getApi< VulkanGraphicsApi >();

    m_module = makeUnique< ShaderModule >( vulkanApi, byteCode );
    m_reflectionInfo = reflectionInfo;

    const uint8_t descriptorSetCount = numericCast< uint8_t >( reflectionInfo.ShaderDescriptorSets.size() );
    for( uint8_t i = 0; i < descriptorSetCount; ++i ) {
        const ShaderDescriptorSet& shaderDescriptorSet = reflectionInfo.ShaderDescriptorSets[ i ];

#if ONYX_IS_RETAIL
        auto layout = makeUnique< DescriptorSetLayout >( vulkanApi.GetDevice(), shaderDescriptorSet );
#else
        String name = file_system::path::getFileName( getPath() );
        auto layout = makeUnique< DescriptorSetLayout >( vulkanApi.getDevice(), shaderDescriptorSet, name );
#endif
        m_descriptorSetLayouts.emplace( std::move( layout ) );
    }
}

DynamicArray< VkPipelineShaderStageCreateInfo > Shader::createPipelineShaderStageCreateInfos() const {
    DynamicArray< VkPipelineShaderStageCreateInfo > pipelineCreateInfos;

    for( uint16_t i = 0; i < enums::toIntegral( ShaderStage::Count ); ++i ) {
        ShaderStage stage = enums::toEnum< ShaderStage >( 1 << i );
        if( stage == ShaderStage::Invalid )
            continue;

        if( enums::isSet( m_reflectionInfo.Stages, stage ) == false )
            continue;

        VkPipelineShaderStageCreateInfo& createInfo = pipelineCreateInfos.emplace_back();
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        createInfo.stage = ToVulkanStage( stage );
        createInfo.module = m_module->GetHandle();
        createInfo.pName = stage == ShaderStage::Vertex ? "vertexMain" : "fragmentMain";
    }

    return pipelineCreateInfos;
}

bool Shader::loadFromDisk( GraphicsSystem& graphicsSystem, Stream& stream ) {
    VulkanGraphicsApi& vulkanApi = graphicsSystem.getApi< VulkanGraphicsApi >();
    DynamicArray< uint8_t > byteCode;
    stream.read( byteCode );

    Span< const uint8_t > span( byteCode.data(), byteCode.size() );
    m_module = makeUnique< ShaderModule >( vulkanApi, span );
    return true;
}
bool Shader::write( Stream& stream ) const {
    stream.write( m_module->getByteCode() );
    return true;
}
} // namespace onyx::rhi::vulkan
