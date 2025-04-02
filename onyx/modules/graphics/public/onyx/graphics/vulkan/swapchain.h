#pragma once

#include <onyx/noncopyable.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx::Graphics
{
    class Window;
}

namespace Onyx::Graphics::Vulkan
{
    class VulkanTextureStorage;
    class VulkanTexture;
    class VulkanGraphicsApi;
    class Framebuffer;
    class Fence;
    class Semaphore;
    class CommandBuffers;
    class CommandPool;
    class Device;
    class Image;
    class Instance;
    class Surface;
    class PhysicalDevice;

    class SwapChain : public NonCopyable
	{
	public:
        SwapChain(VulkanGraphicsApi& graphicsApi);
        ~SwapChain();

        bool BeginFrame(onyxU8 frameIndex);
        bool Present(onyxU8 frameIndex, onyxU32& imageIndex);

        bool HasValidBackBuffer() const { return (m_CurrentImageIndex != onyxMax_U32); }
        onyxU32 GetMinImageCount() const { return m_MinImageCount; }
        onyxU32 GetImageCount() const { return m_ImageCount; }

        const UniquePtr<Semaphore>& GetBackbufferAcquiredSemaphore(onyxU8 frameIndex) const { return m_FrameSyncObjects[frameIndex].m_ImageAcquired;  }
        const UniquePtr<Semaphore>& GetRenderCompleteSemaphore(onyxU8 frameIndex) const { return m_FrameSyncObjects[frameIndex].m_RenderComplete; }
        const UniquePtr<Fence>& GetRenderCompleteFence(onyxU8 frameIndex) const { return m_FrameSyncObjects[frameIndex].m_RenderCompleteFence; }

        const Vector2s32& GetExtent() const { return m_Extent; }
        TextureFormat GetFormat() const { return m_ColorFormat; }
        VkPresentModeKHR GetPresentMode() const { return m_PresentMode; }

        void OnWindowResize(onyxU32 width, onyxU32 height);
        void Init();
        const TextureHandle& GetAcquiredBackbuffer(onyxU8 i) const;
        onyxU32 GetAcquiredBackbufferIndex() const { return m_CurrentImageIndex; }

    private:
        struct SupportDetails
		{
			VkSurfaceCapabilitiesKHR Capabilities{};
			DynamicArray<VkSurfaceFormatKHR> Formats;
            DynamicArray<VkPresentModeKHR> PresentModes;
		};

        SupportDetails QuerySwapChainSupport(const PhysicalDevice& physicalDevice, const Surface& surface) const;
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const DynamicArray<VkSurfaceFormatKHR>& formats) const;
        VkPresentModeKHR ChooseSwapPresentMode(const DynamicArray<VkPresentModeKHR>& presentModes, bool isVSyncEnabled) const;
        VkExtent2D ChooseSwapExtent(const Vector2u32& windowSize, const VkSurfaceCapabilitiesKHR& capabilities) const;
        onyxU32 ChooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities) const;
        VkSurfaceTransformFlagBitsKHR ChoosePreTransform(const VkSurfaceCapabilitiesKHR& capabilities) const;

    private:
        std::mutex mutex;
        VulkanGraphicsApi& m_GraphicsApi;
        const Device& m_Device;

		VULKAN_HANDLE(VkSwapchainKHR, SwapChain, nullptr);

        onyxU32 m_ImageCount;
		onyxU32 m_MinImageCount;
		VkPresentModeKHR m_PresentMode;
		TextureFormat m_ColorFormat;
		VkColorSpaceKHR m_ColorSpace;
        Vector2s32 m_Extent;

        struct SyncObject
        {
            UniquePtr<Semaphore> m_ImageAcquired;
            UniquePtr<Semaphore> m_RenderComplete;

            // unused if timeline semaphores are used
            UniquePtr<Fence> m_RenderCompleteFence;
        };

        InplaceArray<SyncObject, MAX_FRAMES_IN_FLIGHT> m_FrameSyncObjects;
        DynamicArray<TextureHandle> m_SwapchainBuffers;

        onyxU32 m_CurrentImageIndex = onyxMax_U32;
	};
}
