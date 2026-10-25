#pragma once

#include <onyx/rhi/graphicshandles.h>
#include <onyx/rhi/pipeline.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan {
class RenderContext;
class Device;
class PipelineLayout;
class VulkanGraphicsApi;
class Shader;

class Pipeline : public rhi::Pipeline {
  public:
    Pipeline( const VulkanGraphicsApi& api, const PipelineProperties& properties, ShaderHandle& shader );
    ~Pipeline() override;

    const PipelineLayout& getPipelineLayout() const { return *m_pipelineLayout; }
    VkPipelineBindPoint getBindPoint() const { return m_bindPoint; }

  private:
    void createPipeline( const Shader& shader );
    void onShaderLoaded( const ShaderHandle& shader );

  private:
    const VulkanGraphicsApi* m_api;
    rhi::Shader* m_shader = nullptr;
    VkPipelineBindPoint m_bindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;

    UniquePtr< PipelineLayout > m_pipelineLayout;
    VULKAN_HANDLE( VkPipeline, pipeline, nullptr )
};
} // namespace onyx::rhi::vulkan
