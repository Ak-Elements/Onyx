#include <onyx/graphics/vulkan/device.h>
#include <onyx/graphics/vulkan/physicaldevice.h>

namespace Onyx::Graphics::Vulkan
{
	Device::Device(const PhysicalDevice& physicalDevice, const DynamicArray<const char*>& enabledExtensions, VkPhysicalDeviceFeatures2& physicalFeatures, VkQueueFlags requestedQueueTypes)
	{
		// Find the queues.
		const QueueFamilyIndices& queueFamilyIndices = physicalDevice.GetQueueFamilyIndices();

		// Queues can be the same
		HashSet<onyxS32> uniqueQueueFamilies;
		if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
			uniqueQueueFamilies.emplace(queueFamilyIndices.Graphics);
		if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
			uniqueQueueFamilies.emplace(queueFamilyIndices.Compute);
		if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT)
			uniqueQueueFamilies.emplace(queueFamilyIndices.Transfer);

		// Create queues
		DynamicArray<VkDeviceQueueCreateInfo> queueCreateInfos;
		queueCreateInfos.reserve(uniqueQueueFamilies.size());

		const float defaultQueuePriority = 1.0f;
		for (onyxS32 queueFamilyIndex : uniqueQueueFamilies)
		{
			if (queueFamilyIndex == INVALID_INDEX_32)
				continue;

			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &defaultQueuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Enable all features: just pass the physical features 2 struct.
		vkGetPhysicalDeviceFeatures2(physicalDevice.GetHandle(), &physicalFeatures);

		/*VkPhysicalDeviceFeatures enabledFeatures = {};
		enabledFeatures.samplerAnisotropy = true;
		enabledFeatures.wideLines = true;
		enabledFeatures.fillModeNonSolid = true;
		enabledFeatures.independentBlend = true;
		enabledFeatures.pipelineStatisticsQuery = true;*/

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<onyxU32>(queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.pEnabledFeatures = nullptr;
		deviceCreateInfo.pNext = &physicalFeatures;

		// Create the logical device representation
		DynamicArray<const char*> deviceExtensions(enabledExtensions);

		// Enable the debug marker extension if it is present (likely meaning a debugging tool is present)
		if (physicalDevice.IsExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
		{
			deviceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			m_EnableDebugMarkers = true;
		}

		// enable extensions
		if (deviceExtensions.empty() == false)
		{
			for (const char* enabledExtension : deviceExtensions)
			{
				if (physicalDevice.IsExtensionSupported(enabledExtension) == false)
					ONYX_LOG_WARNING("Enabled device extension {} is not supported.", enabledExtension);
				else
					ONYX_LOG_INFO("Enabled device extension: {}", enabledExtension);
			}

			deviceCreateInfo.enabledExtensionCount = static_cast<onyxU32>(deviceExtensions.size());
			deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		}

		VK_CHECK_RESULT(vkCreateDevice(physicalDevice.GetHandle(), &deviceCreateInfo, nullptr, &m_Device))

		vkGetDeviceQueue(m_Device, queueFamilyIndices.Graphics, 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, queueFamilyIndices.Compute, 0, &m_ComputeQueue);
		vkGetDeviceQueue(m_Device, queueFamilyIndices.Transfer, 0, &m_TransferQueue);
		vkGetDeviceQueue(m_Device, queueFamilyIndices.Present, 0, &m_PresentQueue);
	}

	Device::~Device()
	{
	    vkDestroyDevice(m_Device, nullptr);
		m_Device = nullptr;
	}
};
