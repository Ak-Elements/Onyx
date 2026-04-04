#pragma once

#include <onyx/rhi/shader/shader.h>
#include <onyx/rhi/vulkan/descriptorsetlayout.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi {
struct ShaderCacheEntry;
}

namespace onyx::rhi::vulkan {
class VulkanGraphicsApi;

class ShaderModule {
  public:
    ShaderModule( VulkanGraphicsApi& api, DynamicArray< uint32_t > byteCode );
    ~ShaderModule();

  private:
    VulkanGraphicsApi& m_Api;
    DynamicArray< uint32_t > m_ByteCode;
    VULKAN_HANDLE( VkShaderModule, Module, nullptr );
};

class Shader : public rhi::Shader {
    using Super = rhi::Shader;

  public:
    Shader() = default;
    ~Shader() override;

    bool AddStage( GraphicsSystem& graphicsSystem,
                   ShaderStage stage,
                   const DynamicArray< uint32_t >& byteCode ) override;
    void RemoveStage( ShaderStage stage ) override;
    bool UpdateReflectionData( GraphicsSystem& graphicsSystem, ShaderReflectionInfo& reflectionInfo ) override;
    const ShaderReflectionInfo& GetReflectionData() const override { return m_ReflectionInfo; }

    uint64_t GetShaderHash() const override { return m_ShaderHash; }
    void SetShaderHash( uint64_t hash ) override { m_ShaderHash = hash; }

    const InplaceArray< UniquePtr< DescriptorSetLayout >, MAX_DESCRIPTOR_SET_LAYOUTS >& GetDescriptorSetLayouts()
        const {
        return m_DescriptorSetLayouts;
    }
    const Set< VertexInput >& GetVertexInputs() const { return m_ReflectionInfo.vertexInput.GetInputs(); }
    const DynamicArray< PushConstantRange >& GetPushConstantRanges() const {
        return m_ReflectionInfo.pushConstantRanges;
    }

    bool IsComputeShader() const override { return HasStage( ShaderStage::Compute ); }
    bool HasStage( ShaderStage stage ) const { return m_Stages[ enums::toIntegral( stage ) ] != nullptr; }
    bool HasDescriptorSetLayout() const override { return m_DescriptorSetLayouts.empty() == false; }

    const DynamicArray< VkPipelineShaderStageCreateInfo >& GetPipelineShaderStageCreateInfos() const {
        return m_PipelineShaderStageCreateInfos;
    }

#if !ONYX_IS_RETAIL
  public:
    StringView getPath() const override { return m_path; }
    void setPath( const String& path ) override { m_path = path; }

  private:
    String m_path;
#endif

  private:
    uint64_t m_ShaderHash = 0;

    DynamicArray< VkPipelineShaderStageCreateInfo > m_PipelineShaderStageCreateInfos;
    InplaceArray< UniquePtr< ShaderModule >, MAX_SHADER_STAGES > m_Stages;

    ShaderReflectionInfo m_ReflectionInfo;
    InplaceArray< UniquePtr< DescriptorSetLayout >, MAX_DESCRIPTOR_SET_LAYOUTS > m_DescriptorSetLayouts;
};
} // namespace onyx::rhi::vulkan
