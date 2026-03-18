#pragma once

#include <onyx/rhi/pipeline.h>
#include <onyx/rhi/graphicshandles.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan
{
    class RenderContext;
    class Device;
    class PipelineLayout;
    class VulkanGraphicsApi;
    class Shader;

    class Pipeline : public rhi::Pipeline
    {
    public:
        Pipeline(const VulkanGraphicsApi& api, const PipelineProperties& properties, ShaderHandle& shader);
        ~Pipeline() override;

        const PipelineLayout& GetPipelineLayout() const { return *m_PipelineLayout; }
        VkPipelineBindPoint GetBindPoint() const { return m_BindPoint; }

    private:
        void CreatePipeline(const Shader& shader);
        void OnShaderLoaded(const ShaderHandle& shader);

    private:
        const VulkanGraphicsApi* m_Api;
  
        VkPipelineBindPoint m_BindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;

        UniquePtr<PipelineLayout> m_PipelineLayout;
        VULKAN_HANDLE(VkPipeline, Pipeline, nullptr)
    };
}
