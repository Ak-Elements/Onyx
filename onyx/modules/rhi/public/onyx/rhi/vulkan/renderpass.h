#pragma once

#include <onyx/rhi/renderpass.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace Onyx::Graphics::Vulkan
{
    class Device;
    class DepthBuffer;
    class SwapChain;
    class VulkanGraphicsApi;

    class VulkanRenderPass : public Graphics::RenderPass
    {
    public:
        ~VulkanRenderPass() override;

        void Init(const VulkanGraphicsApi& api, const RenderPassSettings& settings);

    private:
        const Device* m_Device = nullptr;

        void CreateRenderPass();
        void CreateLegacyRenderPass();

        VULKAN_HANDLE(VkRenderPass, RenderPass, nullptr)
    };
}
