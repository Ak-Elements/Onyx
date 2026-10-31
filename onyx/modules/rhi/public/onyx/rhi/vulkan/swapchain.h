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

    bool beginFrame( uint8_t frameIndex );
    bool present( uint32_t imageIndex );

    [[nodiscard]] bool hasValidBackBuffer() const {
        return ( m_currentImageIndex != std::numeric_limits< uint32_t >::max() );
    }
    [[nodiscard]] uint32_t getMinImageCount() const { return m_minImageCount; }
    [[nodiscard]] uint32_t getImageCount() const { return m_imageCount; }

    [[nodiscard]] const UniquePtr< Semaphore >& getBackbufferAcquiredSemaphore( uint8_t frameIndex ) const {
        return m_imageAcquiredSemaphores[ frameIndex ];
    }
    [[nodiscard]] const UniquePtr< Semaphore >& getRenderCompleteSemaphore( uint8_t imageIndex ) const {
        return m_renderCompleteSemaphores[ imageIndex ];
    }
    // const UniquePtr<Fence>& GetRenderCompleteFence(uint8_t frameIndex) const { return
    // m_FrameSyncObjects[frameIndex].m_RenderCompleteFence; }

    [[nodiscard]] const Vector2s32& getExtent() const { return m_extent; }
    [[nodiscard]] TextureFormat getFormat() const { return m_colorFormat; }
    [[nodiscard]] VkPresentModeKHR getPresentMode() const { return m_presentMode; }

    TextureHandle& getAcquiredBackbuffer();
    [[nodiscard]] const TextureHandle& getAcquiredBackbuffer() const;
    [[nodiscard]] uint32_t getAcquiredBackbufferIndex() const { return m_currentImageIndex; }

  private:
    struct SupportDetails {
        VkSurfaceCapabilitiesKHR Capabilities{};
        DynamicArray< VkSurfaceFormatKHR > Formats;
        DynamicArray< VkPresentModeKHR > PresentModes;
    };

    void init();

    [[nodiscard]] SupportDetails querySwapChainSupport( const PhysicalDevice& physicalDevice,
                                                        const Surface& surface ) const;
    [[nodiscard]] VkSurfaceFormatKHR chooseSwapSurfaceFormat( const DynamicArray< VkSurfaceFormatKHR >& formats ) const;
    [[nodiscard]] VkPresentModeKHR chooseSwapPresentMode( const DynamicArray< VkPresentModeKHR >& presentModes,
                                                          bool isVSyncEnabled ) const;
    [[nodiscard]] VkExtent2D chooseSwapExtent( const Vector2s32& windowSize,
                                               const VkSurfaceCapabilitiesKHR& capabilities ) const;
    [[nodiscard]] uint32_t chooseImageCount( const VkSurfaceCapabilitiesKHR& capabilities ) const;
    [[nodiscard]] VkSurfaceTransformFlagBitsKHR choosePreTransform(
        const VkSurfaceCapabilitiesKHR& capabilities ) const;

    void onWindowResize( Vector2s32 size );

  private:
    std::mutex m_mutex;
    VulkanGraphicsApi& m_graphicsApi;
    const platform::Window* m_window = nullptr;
    const Surface& m_surface;
    const Device& m_device;

    VULKAN_HANDLE( VkSwapchainKHR, swapChain, nullptr );

    uint32_t m_imageCount;
    uint32_t m_minImageCount;
    VkPresentModeKHR m_presentMode;
    TextureFormat m_colorFormat;
    // VkColorSpaceKHR m_ColorSpace;
    Vector2s32 m_extent;

    // 1 semaphore per swapchain image
    DynamicArray< UniquePtr< Semaphore > > m_renderCompleteSemaphores;
    InplaceArray< UniquePtr< Semaphore >, MaxFramesInFlight > m_imageAcquiredSemaphores;

    // InplaceArray<SyncObject, MAX_FRAMES_IN_FLIGHT> m_FrameSyncObjects;
    DynamicArray< TextureHandle > m_swapchainBuffers;

    uint32_t m_currentImageIndex = std::numeric_limits< uint32_t >::max();
    bool m_shouldRecreateSwapchain = false;
};
} // namespace onyx::rhi::vulkan
