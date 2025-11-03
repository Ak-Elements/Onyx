#include <onyx/graphics/vulkan/shader.h>
#include <onyx/graphics/vulkan/vulkan.h>

#include <onyx/graphics/shader/shadercompiler.h>
#include <onyx/graphics/vulkan/graphicsapi.h>
#include <onyx/graphics/vulkan/device.h>

namespace Onyx::Graphics::Vulkan
{
    ShaderModule::ShaderModule(VulkanGraphicsApi& api, const Shader::ByteCode& byteCode)
        : m_Api(api)
        , m_ByteCode(byteCode)
    {
        VkShaderModuleCreateInfo moduleCreateInfo{};
        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.codeSize = m_ByteCode.size() * sizeof(onyxU32);
        moduleCreateInfo.pCode = m_ByteCode.data();
        moduleCreateInfo.pNext = nullptr;

        VK_CHECK_RESULT(vkCreateShaderModule(api.GetDevice().GetHandle(), &moduleCreateInfo, NULL, &m_Module))
    }

    ShaderModule::~ShaderModule()
    {
        if (m_Module != nullptr)
        {
            vkDestroyShaderModule(m_Api.GetDevice().GetHandle(), m_Module, nullptr);
        }
    }

    Shader::~Shader()
    {
        m_Stages.Clear();
    }

    bool Shader::AddStage(GraphicsApi& api, ShaderStage stage, const ByteCode& byteCode)
    {
        VulkanGraphicsApi& vulkanApi = api.GetApi<VulkanGraphicsApi>();

		const onyxU8 stageIndex = Enums::ToIntegral(stage);
#if ONYX_ASSERTS_ENABLED
		if (stage == ShaderStage::Compute)
			ONYX_ASSERT((HasStage(ShaderStage::Vertex) == false) && (HasStage(ShaderStage::Fragment) == false), "Vertex/Fragment shader does not support compute stage.");
		else
			ONYX_ASSERT(IsComputeShader() == false, "Compute shader does not support %s shader stage", Enums::ToString(stage).data());
#endif
		UniquePtr<ShaderModule> module = MakeUnique<ShaderModule>(vulkanApi, byteCode);

        VkShaderStageFlagBits vulkanStage = ToVulkanStage(stage);
        auto it = std::ranges::find_if(m_PipelineShaderStageCreateInfos, [&](const VkPipelineShaderStageCreateInfo& info)
        {
            return info.stage == vulkanStage;
        });

        VkPipelineShaderStageCreateInfo& pipelineShaderStageCreateInfo = it == m_PipelineShaderStageCreateInfos.end() ? m_PipelineShaderStageCreateInfos.emplace_back() : *it;
		pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfo.stage = ToVulkanStage(stage);
		pipelineShaderStageCreateInfo.module = module->GetHandle();
		pipelineShaderStageCreateInfo.pName = "main";

		m_Stages[stageIndex] = std::move(module);
		return true;
    }

    void Shader::RemoveStage(ShaderStage stage)
    {
        m_Stages[Enums::ToIntegral(stage)].reset();
    }

    bool Shader::UpdateReflectionData(GraphicsApi& api, ShaderReflectionInfo& reflectionInfo)
    {
        VulkanGraphicsApi& vulkanApi = api.GetApi<VulkanGraphicsApi>();

        m_ReflectionInfo = reflectionInfo;

        const onyxU8 descriptorSetCount = numeric_cast<onyxU8>(reflectionInfo.shaderDescriptorSets.size());
		for (onyxU8 i = 0; i < descriptorSetCount; ++i)
		{
			const ShaderDescriptorSet& shaderDescriptorSet = reflectionInfo.shaderDescriptorSets[i];
            m_DescriptorSetLayouts.Emplace(MakeUnique<DescriptorSetLayout>(vulkanApi.GetDevice(), shaderDescriptorSet));
		}

		return true;
    }
}

