#include <onyx/graphics/vulkan/shader.h>
#include <onyx/graphics/vulkan/vulkan.h>

#include <onyx/graphics/shader/shadercompiler.h>
#include <onyx/graphics/vulkan/graphicsapi.h>
#include <onyx/graphics/vulkan/device.h>

namespace Onyx::Graphics::Vulkan
{
    ShaderModule::ShaderModule(VulkanGraphicsApi& api, const Shader::ByteCode& byteCode)
        :m_Api(api)
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

    Shader::Shader(VulkanGraphicsApi& api, const PerStageByteCodes& stageCode)
        : m_Api(api)
    {
        for (onyxU8 i = 0; i < stageCode.size(); ++i)
        {
            if (stageCode[i].empty())
                continue;

			AddStage(Enums::ToEnum<ShaderStage>(i), stageCode[i]);
        }
    }

    Shader::~Shader()
    {
        m_Stages.Clear();
    }

    bool Shader::AddStage(ShaderStage stage, const ByteCode& byteCode)
    {
		const onyxU8 stageIndex = Enums::ToIntegral(stage);
#if ONYX_ASSERTS_ENABLED
		if (stage == ShaderStage::Compute)
			ONYX_ASSERT((HasStage(ShaderStage::Vertex) == false) && (HasStage(ShaderStage::Fragment) == false), "Vertex/Fragment shader does not support compute stage.");
		else
			ONYX_ASSERT(IsComputeShader() == false, "Compute shader does not support %s shader stage", Enums::ToString(stage).data());
#endif
		UniquePtr<ShaderModule> module = MakeUnique<ShaderModule>(m_Api, byteCode);

        VkPipelineShaderStageCreateInfo& pipelineShaderStageCreateInfo = m_PipelineShaderStageCreateInfos.emplace_back();
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

    bool Shader::UpdateReflectionData(ShaderReflectionInfo& reflectionInfo)
    {
        m_ReflectionInfo = reflectionInfo;

        const onyxU8 descriptorSetCount = numeric_cast<onyxU8>(reflectionInfo.shaderDescriptorSets.size());
		for (onyxU8 i = 0; i < descriptorSetCount; ++i)
		{
			const ShaderDescriptorSet& shaderDescriptorSet = reflectionInfo.shaderDescriptorSets[i];
            m_DescriptorSetLayouts.Emplace(MakeUnique<DescriptorSetLayout>(m_Api.GetDevice(), shaderDescriptorSet));
		}

		return true;
    }
}
