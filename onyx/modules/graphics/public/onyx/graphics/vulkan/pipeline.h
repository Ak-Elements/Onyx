#pragma once

#include <onyx/graphics/pipeline.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx::Graphics::Vulkan
{
    class RenderContext;
    class Device;
    class PipelineLayout;
    class VulkanGraphicsApi;
    class Shader;

    class Pipeline : public Graphics::Pipeline
    {
    public:
        Pipeline(const VulkanGraphicsApi& api, const PipelineProperties& properties);
        ~Pipeline() override;

        const PipelineLayout& GetPipelineLayout() const { return *m_PipelineLayout; }

        VkPipelineBindPoint GetBindPoint() const { return m_BindPoint; }

    private:
        void CreatePipeline(const VulkanGraphicsApi& api, const PipelineProperties& properties, const Shader& shader);

    private:
        const Device& m_Device;

        VkPipelineBindPoint m_BindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;

        UniquePtr<PipelineLayout> m_PipelineLayout;
        VULKAN_HANDLE(VkPipeline, Pipeline, nullptr)
    };
}
