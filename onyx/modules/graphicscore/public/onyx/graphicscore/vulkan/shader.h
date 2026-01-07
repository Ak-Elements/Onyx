#pragma once

#include <onyx/graphicscore/shader/shader.h>
#include <onyx/graphicscore/vulkan/descriptorsetlayout.h>
#include <onyx/graphicscore/vulkan/vulkan.h>

namespace Onyx::Graphics
{
    struct ShaderCacheEntry;
}

namespace Onyx::Graphics::Vulkan
{
	class VulkanGraphicsApi;

	class ShaderModule
    {
	public:
		ShaderModule(VulkanGraphicsApi& api, DynamicArray<onyxU32> byteCode);
		~ShaderModule();

	private:
		VulkanGraphicsApi& m_Api;
        DynamicArray<onyxU32> m_ByteCode;
		VULKAN_HANDLE(VkShaderModule, Module, nullptr);
	};

	class Shader : public Graphics::Shader
	{
		using Super = Graphics::Shader;
	public:
		Shader() = default;
		~Shader() override;

		bool AddStage(GraphicsSystem& graphicsSystem, ShaderStage stage, const DynamicArray<onyxU32>& byteCode) override;
		void RemoveStage(ShaderStage stage) override;
		bool UpdateReflectionData(GraphicsSystem& graphicsSystem, ShaderReflectionInfo& reflectionInfo) override;
		const ShaderReflectionInfo& GetReflectionData() const override { return m_ReflectionInfo; }

		onyxU64 GetShaderHash() const override { return m_ShaderHash; }
		void SetShaderHash(onyxU64 hash) override { m_ShaderHash = hash; }

		const InplaceArray<UniquePtr<DescriptorSetLayout>, MAX_DESCRIPTOR_SET_LAYOUTS>& GetDescriptorSetLayouts() const { return m_DescriptorSetLayouts; }
		const Set<VertexInput>& GetVertexInputs() const { return m_ReflectionInfo.vertexInput.GetInputs(); }
		const DynamicArray<PushConstantRange>& GetPushConstantRanges() const { return m_ReflectionInfo.pushConstantRanges; }

		bool IsComputeShader() const override { return HasStage(ShaderStage::Compute); }
		bool HasStage(ShaderStage stage) const { return m_Stages[Enums::ToIntegral(stage)] != nullptr; }
		bool HasDescriptorSetLayout() const override { return m_DescriptorSetLayouts.empty() == false; }

		const DynamicArray<VkPipelineShaderStageCreateInfo>& GetPipelineShaderStageCreateInfos() const { return m_PipelineShaderStageCreateInfos; }
	private:
		onyxU64 m_ShaderHash = 0;

	    DynamicArray<VkPipelineShaderStageCreateInfo> m_PipelineShaderStageCreateInfos;
		InplaceArray<UniquePtr<ShaderModule>, MAX_SHADER_STAGES> m_Stages;

		ShaderReflectionInfo m_ReflectionInfo;
		InplaceArray<UniquePtr<DescriptorSetLayout>, MAX_DESCRIPTOR_SET_LAYOUTS> m_DescriptorSetLayouts;
	};
}
