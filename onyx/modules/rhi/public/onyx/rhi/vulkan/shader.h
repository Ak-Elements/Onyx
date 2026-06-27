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
    VulkanGraphicsApi& m_api;
    DynamicArray< uint32_t > m_byteCode;
    VULKAN_HANDLE( VkShaderModule, Module, nullptr );
};

class Shader : public rhi::Shader {
    using Super = rhi::Shader;

  public:
    Shader() = default;
    ~Shader() override;

    bool addStage( GraphicsSystem& graphicsSystem,
                   ShaderStage stage,
                   const DynamicArray< uint32_t >& byteCode ) override;
    void removeStage( ShaderStage stage ) override;
    bool updateReflectionData( GraphicsSystem& graphicsSystem, ShaderReflectionInfo& reflectionInfo ) override;
    const ShaderReflectionInfo& getReflectionData() const override { return m_reflectionInfo; }

    uint64_t getShaderHash() const override { return m_shaderHash; }
    void setShaderHash( uint64_t hash ) override { m_shaderHash = hash; }

    const InplaceArray< UniquePtr< DescriptorSetLayout >, MAX_DESCRIPTOR_SET_LAYOUTS >& getDescriptorSetLayouts()
        const {
        return m_descriptorSetLayouts;
    }
    const Set< VertexInput >& getVertexInputs() const { return m_reflectionInfo.VertexInput.getInputs(); }
    const DynamicArray< PushConstantRange >& getPushConstantRanges() const {
        return m_reflectionInfo.PushConstantRanges;
    }

    bool isComputeShader() const override { return hasStage( ShaderStage::Compute ); }
    bool hasStage( ShaderStage stage ) const { return m_stages[ enums::toIntegral( stage ) ] != nullptr; }
    bool hasDescriptorSetLayout() const override { return m_descriptorSetLayouts.empty() == false; }

    const DynamicArray< VkPipelineShaderStageCreateInfo >& getPipelineShaderStageCreateInfos() const {
        return m_pipelineShaderStageCreateInfos;
    }

#if !ONYX_IS_RETAIL
  public:
    StringView getPath() const override { return m_path; }
    void setPath( const String& path ) override { m_path = path; }

  private:
    String m_path;
#endif

  private:
    uint64_t m_shaderHash = 0;

    DynamicArray< VkPipelineShaderStageCreateInfo > m_pipelineShaderStageCreateInfos;
    InplaceArray< UniquePtr< ShaderModule >, MAX_SHADER_STAGES > m_stages;

    ShaderReflectionInfo m_reflectionInfo;
    InplaceArray< UniquePtr< DescriptorSetLayout >, MAX_DESCRIPTOR_SET_LAYOUTS > m_descriptorSetLayouts;
};
} // namespace onyx::rhi::vulkan
