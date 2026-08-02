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
    ShaderModule( const VulkanGraphicsApi& api, Span< const uint8_t > byteCode );
    ~ShaderModule();

    // TODO: Return span instead of dynamic array
    [[nodiscard]] const auto& getByteCode() const { return m_byteCode; };

  private:
    const VulkanGraphicsApi& m_api;
    DynamicArray< uint8_t > m_byteCode;
    VULKAN_HANDLE( VkShaderModule, Module, nullptr );
};

class Shader : public rhi::Shader {
    using Super = rhi::Shader;

  public:
    Shader() = default;
    ~Shader() override;

    void create( const GraphicsSystem& graphicsSystem,
                 Span< const uint8_t > byteCode,
                 ShaderReflectionInfo& reflectionInfo );
    const ShaderReflectionInfo& getReflectionData() const override { return m_reflectionInfo; }

    uint64_t getShaderHash() const override { return m_shaderHash; }
    void setShaderHash( uint64_t hash ) override { m_shaderHash = hash; }

    const InplaceArray< UniquePtr< DescriptorSetLayout >, MaxDescriptorSetLayouts >& getDescriptorSetLayouts() const {
        return m_descriptorSetLayouts;
    }
    const Set< VertexInput >& getVertexInputs() const { return m_reflectionInfo.VertexInput.getInputs(); }
    const DynamicArray< PushConstantRange >& getPushConstantRanges() const {
        return m_reflectionInfo.PushConstantRanges;
    }

    bool isComputeShader() const override { return enums::isSet( m_reflectionInfo.Stages, ShaderStage::Compute ); }
    bool hasDescriptorSetLayout() const override { return m_descriptorSetLayouts.empty() == false; }

    DynamicArray< VkPipelineShaderStageCreateInfo > createPipelineShaderStageCreateInfos() const;

    [[nodiscard]] bool loadFromDisk( GraphicsSystem& graphicsSystem, Stream& stream ) override;
    [[nodiscard]] bool write( Stream& stream ) const override;

#if !ONYX_IS_RETAIL
  public:
    StringView getPath() const override { return m_path; }
    void setPath( const String& path ) override { m_path = path; }

  private:
    String m_path;
#endif

  private:
    uint64_t m_shaderHash = 0;

    UniquePtr< ShaderModule > m_module = nullptr;
    ShaderReflectionInfo m_reflectionInfo;

    InplaceArray< UniquePtr< DescriptorSetLayout >, MaxDescriptorSetLayouts > m_descriptorSetLayouts;
};
} // namespace onyx::rhi::vulkan
