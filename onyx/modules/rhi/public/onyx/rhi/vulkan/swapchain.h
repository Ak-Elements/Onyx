#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/graphicshandles.h>
#include <onyx/rhi/vulkan/vulkan.h>

#include <onyx/platform/platformfwd.h>

namespace onyx::rhi::vulkan {
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

class SwapChain : public NonCopyable {
  public:
    SwapChain( VulkanGraphicsApi& graphicsApi, const Surface& surface, const platform::Window& window );
    ~SwapChain() override;

    bool BeginFrame( uint8_t frameIndex );
    bool Present( uint32_t imageIndex );

    bool HasValidBackBuffer() const { return ( m_CurrentImageIndex != std::numeric_limits< uint32_t >::max() ); }
    uint32_t GetMinImageCount() const { return m_MinImageCount; }
    uint32_t GetImageCount() const { return m_ImageCount; }

    const UniquePtr< Semaphore >& GetBackbufferAcquiredSemaphore( uint8_t frameIndex ) const {
        return m_ImageAcquiredSemaphores[ frameIndex ];
    }
    const UniquePtr< Semaphore >& GetRenderCompleteSemaphore( uint8_t imageIndex ) const {
        return m_RenderCompleteSemaphores[ imageIndex ];
    }
    // const UniquePtr<Fence>& GetRenderCompleteFence(uint8_t frameIndex) const { return
    // m_FrameSyncObjects[frameIndex].m_RenderCompleteFence; }

    const Vector2s32& GetExtent() const { return m_Extent; }
    TextureFormat GetFormat() const { return m_ColorFormat; }
    VkPresentModeKHR GetPresentMode() const { return m_PresentMode; }

    TextureHandle& GetAcquiredBackbuffer();
    const TextureHandle& GetAcquiredBackbuffer() const;
    uint32_t GetAcquiredBackbufferIndex() const { return m_CurrentImageIndex; }

  private:
    struct SupportDetails {
        VkSurfaceCapabilitiesKHR Capabilities{};
        DynamicArray< VkSurfaceFormatKHR > Formats;
        DynamicArray< VkPresentModeKHR > PresentModes;
    };

    void Init();

    SupportDetails QuerySwapChainSupport( const PhysicalDevice& physicalDevice, const Surface& surface ) const;
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat( const DynamicArray< VkSurfaceFormatKHR >& formats ) const;
    VkPresentModeKHR ChooseSwapPresentMode( const DynamicArray< VkPresentModeKHR >& presentModes,
                                            bool isVSyncEnabled ) const;
    VkExtent2D ChooseSwapExtent( const Vector2s32& windowSize, const VkSurfaceCapabilitiesKHR& capabilities ) const;
    uint32_t ChooseImageCount( const VkSurfaceCapabilitiesKHR& capabilities ) const;
    VkSurfaceTransformFlagBitsKHR ChoosePreTransform( const VkSurfaceCapabilitiesKHR& capabilities ) const;

    void OnWindowResize( Vector2s32 size );

  private:
    std::mutex mutex;
    VulkanGraphicsApi& m_GraphicsApi;
    const platform::Window* m_Window = nullptr;
    const Surface& m_Surface;
    const Device& m_Device;

    VULKAN_HANDLE( VkSwapchainKHR, SwapChain, nullptr );

    uint32_t m_ImageCount;
    uint32_t m_MinImageCount;
    VkPresentModeKHR m_PresentMode;
    TextureFormat m_ColorFormat;
    // VkColorSpaceKHR m_ColorSpace;
    Vector2s32 m_Extent;

    // 1 semaphore per swapchain image
    DynamicArray< UniquePtr< Semaphore > > m_RenderCompleteSemaphores;
    InplaceArray< UniquePtr< Semaphore >, MAX_FRAMES_IN_FLIGHT > m_ImageAcquiredSemaphores;

    // InplaceArray<SyncObject, MAX_FRAMES_IN_FLIGHT> m_FrameSyncObjects;
    DynamicArray< TextureHandle > m_SwapchainBuffers;

    uint32_t m_CurrentImageIndex = std::numeric_limits< uint32_t >::max();
    bool m_ShouldRecreateSwapchain = false;
};
} // namespace onyx::rhi::vulkan
