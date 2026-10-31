#include <onyx/rhi/vulkan/swapchain.h>

#include <onyx/rhi/vulkan/device.h>
#include <onyx/rhi/vulkan/surface.h>

#include <onyx/rhi/vulkan/commandbuffer.h>
#include <onyx/rhi/vulkan/commandpool.h>
#include <onyx/rhi/vulkan/fence.h>
#include <onyx/rhi/vulkan/framebuffer.h>
#include <onyx/rhi/vulkan/instance.h>
#include <onyx/rhi/vulkan/physicaldevice.h>
#include <onyx/rhi/vulkan/semaphore.h>
#include <onyx/rhi/vulkan/texture.h>
#include <onyx/rhi/vulkan/texturestorage.h>

#include <onyx/profiler/profiler.h>
#include <onyx/rhi/vulkan/graphicsapi.h>
#include <onyx/rhi/vulkan/swapchain.h>

#include <algorithm>

namespace onyx::rhi::vulkan {
SwapChain::SwapChain( VulkanGraphicsApi& api, const Surface& surface, const platform::Window& window )
    : m_graphicsApi( api )
    , m_window( &window )
    , m_surface( surface )
    , m_device( api.getDevice() ) {
    m_window->onResize().connect< &SwapChain::onWindowResize >( this );
    init();
}

SwapChain::~SwapChain() {
    m_renderCompleteSemaphores.clear();
    m_imageAcquiredSemaphores.clear();
    m_swapchainBuffers.clear();

    vkDestroySwapchainKHR( m_device.getHandle(), m_swapChain, nullptr );
}

bool SwapChain::beginFrame( uint8_t frameIndex ) {
    ONYX_PROFILE_FUNCTION;

    if( m_shouldRecreateSwapchain ) {
        init();
    }

    VkDevice device = m_device.getHandle();

    VkResult result;
    {
        std::lock_guard lock( m_mutex );
        ONYX_PROFILE_SECTION( "WaitAcquire Image" )
        result = vkAcquireNextImageKHR( device,
                                        m_swapChain,
                                        UINT64_MAX,
                                        m_imageAcquiredSemaphores[ frameIndex ]->getHandle(),
                                        VK_NULL_HANDLE,
                                        &m_currentImageIndex );
        if( result == VK_ERROR_OUT_OF_DATE_KHR ) {
            m_shouldRecreateSwapchain = true;
            m_currentImageIndex = std::numeric_limits< uint32_t >::max();
            return false;
        }
    }

    return true;
}

bool SwapChain::present( uint32_t imageIndex ) {
    if( imageIndex == std::numeric_limits< uint32_t >::max() )
        return true;

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.pSwapchains = &m_swapChain;
    presentInfo.swapchainCount = 1;
    presentInfo.pWaitSemaphores = m_renderCompleteSemaphores[ imageIndex ]->getHandlePtr();
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &imageIndex;

    {
        std::lock_guard lock( m_mutex );
        std::lock_guard queueLock = m_graphicsApi.lockGraphicsQueue();
        VkResult result = vkQueuePresentKHR( m_device.getGraphicsQueue(), &presentInfo );
        if( result == VK_ERROR_OUT_OF_DATE_KHR ) {
            m_shouldRecreateSwapchain = true;
            return false;
        }
    }

    return true;
}

void SwapChain::init() {
    vkDeviceWaitIdle( m_device.getHandle() );

    m_shouldRecreateSwapchain = false;
    bool hasSwapchain = m_swapChain != nullptr;

    const PhysicalDevice& physicalDevice = m_graphicsApi.getPhysicalDevice();

    const SupportDetails details = querySwapChainSupport( physicalDevice, m_surface );
    ONYX_ASSERT( !details.Formats.empty(), "empty swap chain support" );
    ONYX_ASSERT( !details.PresentModes.empty(), "empty swap chain support" );

    const VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat( details.Formats );
    const VkPresentModeKHR actualPresentMode = chooseSwapPresentMode( details.PresentModes,
                                                                      m_window->isVSyncEnabled() );
    const VkExtent2D extent = chooseSwapExtent( m_window->getFrameBufferSize(), details.Capabilities );
    const uint32_t imageCount = chooseImageCount( details.Capabilities );
    const VkSurfaceTransformFlagBitsKHR preTransform = choosePreTransform( details.Capabilities );

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface.getHandle();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = preTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = actualPresentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = m_swapChain;

    // Enable transfer source on swap chain images if supported
    if( details.Capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) {
        createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }

    // Enable transfer destination on swap chain images if supported
    if( details.Capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT ) {
        createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    // uint32_t queueFamilyIndices[] = { static_cast<uint32_t>(physicalDevice.GetGraphicsQueueIndex()),
    // static_cast<uint32_t>(physicalDevice.GetPresentQueueIndex()) }; if (physicalDevice.GetGraphicsQueueIndex() !=
    // physicalDevice.GetPresentQueueIndex())
    //{
    //    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    //    createInfo.queueFamilyIndexCount = 2;
    //    createInfo.pQueueFamilyIndices = queueFamilyIndices;
    //}
    // else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;     // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.oldSwapchain = m_swapChain;
    VK_CHECK_RESULT( vkCreateSwapchainKHR( m_device.getHandle(), &createInfo, nullptr, &m_swapChain ) )

    // if (oldSwapchain)
    //     vkDestroySwapchainKHR(m_Device.GetHandle(), oldSwapchain, nullptr);

    m_minImageCount = (std::max)( 2u, details.Capabilities.minImageCount );
    m_presentMode = actualPresentMode;
    m_colorFormat = VulkanTextureStorage::getFormat( surfaceFormat.format );
    m_extent[ 0 ] = numericCast< int32_t >( extent.width );
    m_extent[ 1 ] = numericCast< int32_t >( extent.height );

    m_imageCount = 0;
    vkGetSwapchainImagesKHR( m_device.getHandle(), m_swapChain, &m_imageCount, nullptr );

    DynamicArray< VkImage > images;
    images.resize( m_imageCount );
    vkGetSwapchainImagesKHR( m_device.getHandle(), m_swapChain, &m_imageCount, images.data() );

    m_swapchainBuffers.resize( m_imageCount );

    TextureProperties textureProps;
    textureProps.Format = TextureFormat::BGRA_UNORM8;

    for( uint32_t i = 0; i < m_imageCount; ++i ) {
        TextureHandle& texture = m_swapchainBuffers[ i ];

        Reference< VulkanTextureStorage > textureStorage = Reference< VulkanTextureStorage >::create(
            m_graphicsApi,
            images[ i ],
            format::format( "Swapchain Image {}", i ) );

        texture.Storage = textureStorage;
        textureProps.DebugName = format::format( "Swapchain ImageView {}", i );

        texture.Texture = Reference< VulkanTexture, TextureDeleter >::create( m_graphicsApi,
                                                                              textureProps,
                                                                              textureStorage.raw() );
    }

    if( hasSwapchain == false ) {
        for( uint8_t i = 0; i < m_imageCount; ++i ) {
            m_renderCompleteSemaphores.emplace_back( makeUnique< Semaphore >( m_device ) );
        }

        for( uint8_t i = 0; i < MaxFramesInFlight; ++i ) {
            m_imageAcquiredSemaphores.emplace( makeUnique< Semaphore >( m_device ) );
        }
    }

    vkDeviceWaitIdle( m_device.getHandle() );
}

TextureHandle& SwapChain::getAcquiredBackbuffer() {
    return m_swapchainBuffers[ m_currentImageIndex ];
}

const TextureHandle& SwapChain::getAcquiredBackbuffer() const {
    return m_swapchainBuffers[ m_currentImageIndex ];
}

SwapChain::SupportDetails SwapChain::querySwapChainSupport( const PhysicalDevice& physicalDevice,
                                                            const Surface& surface ) const {
    VkPhysicalDevice physicalDeviceHandle = physicalDevice.getHandle();
    VkSurfaceKHR surfaceHandle = surface.getHandle();

    SupportDetails details;
    VK_CHECK_RESULT(
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physicalDeviceHandle, surfaceHandle, &details.Capabilities ) );

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physicalDeviceHandle, surfaceHandle, &capabilities );

    uint32_t formatsCount = 0;
    VK_CHECK_RESULT(
        vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDeviceHandle, surfaceHandle, &formatsCount, nullptr ) );
    details.Formats.resize( formatsCount );
    VK_CHECK_RESULT( vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDeviceHandle,
                                                           surfaceHandle,
                                                           &formatsCount,
                                                           details.Formats.data() ) );

    uint32_t presentCount = 0;
    VK_CHECK_RESULT(
        vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDeviceHandle, surfaceHandle, &presentCount, nullptr ) );
    details.PresentModes.resize( presentCount );
    VK_CHECK_RESULT( vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDeviceHandle,
                                                                surfaceHandle,
                                                                &presentCount,
                                                                details.PresentModes.data() ) );

    return details;
}

VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat( const std::vector< VkSurfaceFormatKHR >& formats ) const {
    if( formats.size() == 1 && formats[ 0 ].format == VK_FORMAT_UNDEFINED ) {
        return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    }

    for( const VkSurfaceFormatKHR& format : formats ) {
        if( format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ) {
            return format;
        }
    }

    ONYX_LOG_WARNING( "Found no suitable surface format" );
    return formats.front();
}

VkPresentModeKHR SwapChain::chooseSwapPresentMode( const std::vector< VkPresentModeKHR >& presentModes,
                                                   bool isVSyncEnabled ) const {
    if( isVSyncEnabled == false ) {
        for( const VkPresentModeKHR& presentMode : presentModes ) {
            if( presentMode == VK_PRESENT_MODE_MAILBOX_KHR ) {
                return VK_PRESENT_MODE_MAILBOX_KHR;
            }

            if( presentMode == VK_PRESENT_MODE_FIFO_RELAXED_KHR ) {
                return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
            }

            if( presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR ) {
                return VK_PRESENT_MODE_IMMEDIATE_KHR;
            }
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::chooseSwapExtent( const Vector2s32& windowSize,
                                        const VkSurfaceCapabilitiesKHR& capabilities ) const {
    // Vulkan tells us to match the resolution of the window by setting the width and height in the currentExtent
    // member. However, some window managers do allow us to differ here and this is indicated by setting the width and
    // height in currentExtent to a special value: the maximum value of uint32_t. In that case we'll pick the resolution
    // that best matches the window within the minImageExtent and maxImageExtent bounds.
    if( capabilities.currentExtent.width != std::numeric_limits< uint32_t >::max() ) {
        return capabilities.currentExtent;
    }

    VkExtent2D actualExtent{ static_cast< uint32_t >( windowSize.X ), static_cast< uint32_t >( windowSize.Y ) };
    actualExtent.width = std::max( capabilities.minImageExtent.width,
                                   std::min( capabilities.maxImageExtent.width, actualExtent.width ) );
    actualExtent.height = std::max( capabilities.minImageExtent.height,
                                    std::min( capabilities.maxImageExtent.height, actualExtent.height ) );
    return actualExtent;
}

uint32_t SwapChain::chooseImageCount( const VkSurfaceCapabilitiesKHR& capabilities ) const {
    uint32_t imageCount = std::max< uint32_t >( 2u, capabilities.minImageCount + 1 );

    if( capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount ) {
        imageCount = capabilities.maxImageCount;
    }

    return imageCount;
}

VkSurfaceTransformFlagBitsKHR SwapChain::choosePreTransform( const VkSurfaceCapabilitiesKHR& capabilities ) const {
    if( capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ) {
        return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }

    return capabilities.currentTransform;
}

void SwapChain::onWindowResize( Vector2s32 size ) {
    if( m_extent != size ) {
        m_extent = size;
        m_shouldRecreateSwapchain = true;
    }
}

} // namespace onyx::rhi::vulkan
