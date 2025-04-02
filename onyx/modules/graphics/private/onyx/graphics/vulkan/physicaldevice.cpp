#include <onyx/graphics/vulkan/physicaldevice.h>
#include <onyx/graphics/vulkan/instance.h>
#include <onyx/graphics/vulkan/surface.h>

namespace Onyx::Graphics::Vulkan
{
	PhysicalDevice::PhysicalDevice(const Instance& instance, const Surface& surface)
    {
		SelectPhysicalDevice(instance);

        RetrieveSupportedExtensions();
        RetrieveQueueFamilyProperties();
        RetrieveQueueFamilyIndices(surface);

        m_DepthFormat = GetSupportedDepthFormat(true);
        m_MultiSamplingLevel = GetMaxUsableSampleCount();
    }

	PhysicalDevice::~PhysicalDevice()
	{
	}

    void PhysicalDevice::RetrieveQueueFamilyProperties()
    {
        onyxU32 queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);

        ONYX_ASSERT(queueFamilyCount > 0);
        m_QueueFamilyProperties.resize(queueFamilyCount);

        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, m_QueueFamilyProperties.data());
    }

	void PhysicalDevice::SelectPhysicalDevice(const Instance& instance)
	{
		// TODO: Add support for integrated GPUs
		const std::vector<VkPhysicalDevice>& physicalDevices = instance.GetPhysicalDevices();

		VkPhysicalDevice physicalDevice = nullptr;
		VkPhysicalDeviceProperties properties;
		for (size_t i = 0; i < physicalDevices.size(); i++)
		{
			physicalDevice = physicalDevices[i];
			vkGetPhysicalDeviceProperties(physicalDevice, &properties);

			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				m_PhysicalDevice = physicalDevice;
				m_Properties = properties;
				break;
			}
		}
		
		ONYX_ASSERT(m_PhysicalDevice, "Missing physical device for graphics");
		ONYX_ASSERT(m_Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);

		vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_Features);
		vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_MemoryProperties);
	}

	void PhysicalDevice::RetrieveSupportedExtensions()
	{
		// Get list of supported extensions
		onyxU32 extenstionsCount = 0;
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extenstionsCount, nullptr);
		if (extenstionsCount > 0)
		{
			std::vector<VkExtensionProperties> extensions(extenstionsCount);
			if (vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extenstionsCount, &extensions.front()) == VK_SUCCESS)
			{
				m_SupportedExtensions.reserve(extenstionsCount);
				for (const VkExtensionProperties& extension : extensions)
				{
					m_SupportedExtensions.emplace(extension.extensionName);
				}
			}
		}
	}

	onyxU32 PhysicalDevice::GetMemoryType(onyxU32 typeBits, VkMemoryPropertyFlags requiredPropertyFlags, VkMemoryPropertyFlags /*preferredPropertyFlags*/) const
	{
		for (onyxU32 i = 0; i < m_MemoryProperties.memoryTypeCount; i++)
		{
			if ((typeBits & 1) == 1)
			{
				if ((m_MemoryProperties.memoryTypes[i].propertyFlags & requiredPropertyFlags) == requiredPropertyFlags)
				{
					return i;
				}
			}
			typeBits >>= 1;
		}

		ONYX_ASSERT(false, "Could not find a matching memory type");
		return onyxMax_U32;
	}

	void PhysicalDevice::RetrieveQueueFamilyIndices(const Surface& surface)
	{
		onyxS32 GraphicsFlagsCount = VK_QUEUE_FLAG_BITS_MAX_ENUM;
		onyxS32 ComputeFlagsCount = VK_QUEUE_FLAG_BITS_MAX_ENUM;
		onyxS32 TransferFlagsCount = VK_QUEUE_FLAG_BITS_MAX_ENUM;
		onyxS32 PresentFlagsCount = VK_QUEUE_FLAG_BITS_MAX_ENUM;

		VkBool32 supportsPresenting = false;
		for (onyxS32 i = 0; i < m_QueueFamilyProperties.size(); ++i)
		{
			const VkQueueFamilyProperties& queueFamily = m_QueueFamilyProperties[i];
			if (queueFamily.queueCount == 0)
				continue;

			const onyxS32 bitCount = std::popcount(queueFamily.queueFlags);
			if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
		    {
				vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, surface.GetHandle(), &supportsPresenting);
				if (supportsPresenting && (bitCount < PresentFlagsCount))
				{
					PresentFlagsCount = bitCount;
					m_QueueFamilyIndices.Present = i;
				}

				if (bitCount < GraphicsFlagsCount)
				{
					GraphicsFlagsCount = bitCount;
					m_QueueFamilyIndices.Graphics = i;
				}
		    }

			if ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
			{
				if (bitCount < ComputeFlagsCount)
				{
					ComputeFlagsCount = bitCount;
					m_QueueFamilyIndices.Compute = i;
				}
			}

			if ((queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)
			{
				if (bitCount < TransferFlagsCount)
				{
					TransferFlagsCount = bitCount;
					m_QueueFamilyIndices.Transfer = i;
				}
			}
		}

		//TODO verify we found all queues that were requested
		//ONYX_ASSERT()
	}

	bool PhysicalDevice::IsExtensionSupported(const StringView& extension) const
	{
		return m_SupportedExtensions.find(extension.data()) != m_SupportedExtensions.end();
	}

	VkFormat PhysicalDevice::GetSupportedDepthFormat(bool checkSamplingSupport) const
	{
		// All depth formats may be optional, so we need to find a suitable depth format to use
		std::vector<VkFormat> depthFormats = { VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D16_UNORM };
		for (auto& format : depthFormats)
		{
			VkFormatProperties formatProperties;
			vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &formatProperties);
			// Format must support depth stencil attachment for optimal tiling
			if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				if (checkSamplingSupport)
				{
					if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)) 
					{
						continue;
					}
				}
				return format;
			}
		}

		ONYX_LOG_FATAL("Could not find a matching depth format");
		return VK_FORMAT_UNDEFINED;
	}

	VkSampleCountFlagBits PhysicalDevice::GetMaxUsableSampleCount() const
	{
		VkSampleCountFlags counts = std::min<VkSampleCountFlags>(m_Properties.limits.framebufferColorSampleCounts, m_Properties.limits.framebufferDepthSampleCounts);
		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
		if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }
		return VK_SAMPLE_COUNT_1_BIT;
	}
};
