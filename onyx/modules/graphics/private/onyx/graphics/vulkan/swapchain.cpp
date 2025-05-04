#include <onyx/graphics/vulkan/swapchain.h>

#include <onyx/graphics/vulkan/device.h>
#include <onyx/graphics/vulkan/surface.h>

#include <onyx/graphics/window.h>
#include <onyx/graphics/vulkan/commandbuffer.h>
#include <onyx/graphics/vulkan/commandpool.h>
#include <onyx/graphics/vulkan/fence.h>
#include <onyx/graphics/vulkan/framebuffer.h>
#include <onyx/graphics/vulkan/texture.h>
#include <onyx/graphics/vulkan/texturestorage.h>
#include <onyx/graphics/vulkan/instance.h>
#include <onyx/graphics/vulkan/semaphore.h>
#include <onyx/graphics/vulkan/physicaldevice.h>

#include <onyx/graphics/vulkan/graphicsapi.h>
#include <onyx/graphics/vulkan/swapchain.h>
#include <onyx/profiler/profiler.h>

#include <algorithm>

namespace Onyx::Graphics::Vulkan
{

SwapChain::SwapChain(VulkanGraphicsApi& api)
    : m_GraphicsApi(api)
	, m_Device(api.GetDevice())
{
	Init();
}

SwapChain::~SwapChain()
{
    m_FrameSyncObjects.Clear();
    m_SwapchainBuffers.clear();

    vkDestroySwapchainKHR(m_Device.GetHandle(), m_SwapChain, nullptr);
}

bool SwapChain::BeginFrame(onyxU8 frameIndex)
{
	ONYX_PROFILE_FUNCTION;

	const VkDevice device = m_Device.GetHandle();

	SyncObject& syncObject = m_FrameSyncObjects[frameIndex];
	if (m_GraphicsApi.IsTimelineSemaphoreEnabled() == false)
	{
		const VkFence waitFence = syncObject.m_RenderCompleteFence->GetHandle();
		VK_CHECK_RESULT(vkWaitForFences(device, 1, &waitFence, VK_TRUE, onyxMax_U64));
	}

	VkResult result;
	{
        std::lock_guard lock(mutex);
        ONYX_PROFILE_SECTION("WaitAcquire Image")
        result = vkAcquireNextImageKHR(device, m_SwapChain, UINT64_MAX, syncObject.m_ImageAcquired->GetHandle(), VK_NULL_HANDLE, &m_CurrentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			vkDeviceWaitIdle(device);
			Init();
			vkDeviceWaitIdle(device);
			m_CurrentImageIndex = onyxMax_U32;
			return false;
		}
	}

	ONYX_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR);
	if (m_GraphicsApi.IsTimelineSemaphoreEnabled() == false)
		VK_CHECK_RESULT(vkResetFences(device, 1, syncObject.m_RenderCompleteFence->GetHandlePtr()));

	return true;
}

bool SwapChain::Present(onyxU8 frameIndex, onyxU32& imageIndex)
{
	if (m_CurrentImageIndex == onyxMax_U32)
		return true;

	SyncObject& syncObject = m_FrameSyncObjects[frameIndex];

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
    presentInfo.pSwapchains = &m_SwapChain;
    presentInfo.swapchainCount = 1;
    presentInfo.pWaitSemaphores = syncObject.m_RenderComplete->GetHandlePtr();
    presentInfo.waitSemaphoreCount = 1;
	presentInfo.pImageIndices = &static_cast<onyxU32&>(imageIndex);
	
	{
		std::lock_guard lock(mutex);
		std::lock_guard queueLock = m_GraphicsApi.LockGraphicsQueue();
		VkResult result = vkQueuePresentKHR(m_Device.GetGraphicsQueue(), &presentInfo);
		if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR))
		{
			Init();
			return false;
		}
	}

	return true;
}

void SwapChain::Init()
{
	const Window& window = m_GraphicsApi.GetWindow();
	if (window.IsMinimized())
		return;

	const bool isVSyncEnabled = window.IsVSyncEnabled();
	const PhysicalDevice& physicalDevice = m_GraphicsApi.GetPhysicalDevice();

	const SupportDetails details = QuerySwapChainSupport(physicalDevice, m_GraphicsApi.GetSurface());
	ONYX_ASSERT(!details.Formats.empty(), "empty swap chain support");
	ONYX_ASSERT(!details.PresentModes.empty(), "empty swap chain support");

	const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(details.Formats);
	const VkPresentModeKHR actualPresentMode = ChooseSwapPresentMode(details.PresentModes, isVSyncEnabled);
	const VkExtent2D extent = ChooseSwapExtent(window.GetFrameBufferSize(), details.Capabilities);
	const onyxU32 imageCount = ChooseImageCount(details.Capabilities);
	const VkSurfaceTransformFlagBitsKHR preTransform = ChoosePreTransform(details.Capabilities);

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_GraphicsApi.GetSurface().GetHandle();
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
	createInfo.oldSwapchain = m_SwapChain;

	// Enable transfer source on swap chain images if supported
	if (details.Capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
	{
		createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}

	// Enable transfer destination on swap chain images if supported
	if (details.Capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	{
		createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	onyxU32 queueFamilyIndices[] = { static_cast<onyxU32>(physicalDevice.GetGraphicsQueueIndex()), static_cast<onyxU32>(physicalDevice.GetPresentQueueIndex()) };
	if (physicalDevice.GetGraphicsQueueIndex() != physicalDevice.GetPresentQueueIndex())
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.oldSwapchain = m_SwapChain;
	VK_CHECK_RESULT(vkCreateSwapchainKHR(m_Device.GetHandle(), &createInfo, nullptr, &m_SwapChain))

	//if (oldSwapchain)
	//    vkDestroySwapchainKHR(m_Device.GetHandle(), oldSwapchain, nullptr);

	m_MinImageCount = (std::max)(2u, details.Capabilities.minImageCount);
	m_PresentMode = actualPresentMode;
	m_ColorFormat = VulkanTextureStorage::GetFormat(surfaceFormat.format);
	m_Extent[0] = extent.width;
    m_Extent[1] = extent.height;

	m_ImageCount = 0;
	vkGetSwapchainImagesKHR(m_Device.GetHandle(), m_SwapChain, &m_ImageCount, nullptr);

	DynamicArray<VkImage> images;
	images.resize(m_ImageCount);
	vkGetSwapchainImagesKHR(m_Device.GetHandle(), m_SwapChain, &m_ImageCount, images.data());

	m_SwapchainBuffers.resize(m_ImageCount);

	TextureProperties textureProps;
	textureProps.m_Format = TextureFormat::BGRA_UNORM8;

	for (onyxU32 i = 0; i < m_ImageCount; ++i)
	{
		TextureHandle& texture = m_SwapchainBuffers[i];

		Reference<VulkanTextureStorage> textureStorage = Reference<VulkanTextureStorage>::Create(m_GraphicsApi, images[i], Format::Format("Swapchain Image {}", i));

		texture.Storage = textureStorage;
		textureProps.m_DebugName = Format::Format("Swapchain ImageView {}", i);

		texture.Texture = Reference<VulkanTexture, TextureDeleter>::Create(m_GraphicsApi, textureProps, textureStorage.Raw());
	}

	for (onyxU8 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		SyncObject& syncObj = m_FrameSyncObjects[i];
		syncObj.m_ImageAcquired = MakeUnique<Semaphore>(m_Device);
		syncObj.m_RenderComplete = MakeUnique<Semaphore>(m_Device);

		if (m_GraphicsApi.IsTimelineSemaphoreEnabled() == false)
		{
			syncObj.m_RenderCompleteFence = MakeUnique<Fence>(m_Device, true);
		}
	}
}

const TextureHandle& SwapChain::GetAcquiredBackbuffer(onyxU8) const
{
    return m_SwapchainBuffers[m_CurrentImageIndex];
}

SwapChain::SupportDetails SwapChain::QuerySwapChainSupport(const PhysicalDevice& physicalDevice, const Surface& surface) const
{
	VkPhysicalDevice physicalDeviceHandle = physicalDevice.GetHandle();
	VkSurfaceKHR surfaceHandle = surface.GetHandle();

	SupportDetails details;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDeviceHandle, surfaceHandle, &details.Capabilities));

	VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDeviceHandle, surfaceHandle, &capabilities);

	onyxU32 formatsCount = 0;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDeviceHandle, surfaceHandle, &formatsCount, nullptr));
	details.Formats.resize(formatsCount);
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDeviceHandle, surfaceHandle, &formatsCount, details.Formats.data()));

	onyxU32 presentCount = 0;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDeviceHandle, surfaceHandle, &presentCount, nullptr));
	details.PresentModes.resize(presentCount);
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDeviceHandle, surfaceHandle, &presentCount, details.PresentModes.data()));

	return details;
}

VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) const
{
	if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) 
	{
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const VkSurfaceFormatKHR& format : formats)
	{
		if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return format;
		}
	}

	ONYX_LOG_WARNING("Found no suitable surface format");
	return formats.front();
}

VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes, bool isVSyncEnabled) const
{
	if (isVSyncEnabled == false)
	{
	    for (const VkPresentModeKHR& presentMode : presentModes)
	    {
		    if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		    {
			    return VK_PRESENT_MODE_MAILBOX_KHR;
		    }

			if (presentMode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
			{
				return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
			}

		    if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		    {
			    return VK_PRESENT_MODE_IMMEDIATE_KHR;
		    }
	    }
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::ChooseSwapExtent(const Vector2u32& windowSize, const VkSurfaceCapabilitiesKHR& capabilities) const
{
	// Vulkan tells us to match the resolution of the window by setting the width and height in the currentExtent member.
	// However, some window managers do allow us to differ here and this is indicated by setting the width and height in
	// currentExtent to a special value: the maximum value of onyxU32. In that case we'll pick the resolution that best 
	// matches the window within the minImageExtent and maxImageExtent bounds.
	if (capabilities.currentExtent.width != onyxMax_U32)
	{
		return capabilities.currentExtent;
	}

	VkExtent2D actualExtent { windowSize[0], windowSize[1] };
	actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
	actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

	return actualExtent;
}

onyxU32 SwapChain::ChooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities) const
{
	onyxU32 imageCount = std::max<onyxU32>(2u, capabilities.minImageCount + 1);
	
	if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
	{
		imageCount = capabilities.maxImageCount;
	}

	return imageCount;
}

VkSurfaceTransformFlagBitsKHR SwapChain::ChoosePreTransform(const VkSurfaceCapabilitiesKHR& capabilities) const
{
	if (capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}

	return capabilities.currentTransform;
}

}
