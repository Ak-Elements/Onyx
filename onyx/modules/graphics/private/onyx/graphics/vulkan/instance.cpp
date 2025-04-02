#include <onyx/graphics/vulkan/instance.h>
#include <onyx/graphics/vulkan/vulkan.h>

#include <onyx/graphics/window.h>
#include <onyx/graphics/vulkan/debugutilsmessenger.h>

namespace Onyx::Graphics::Vulkan
{
    Instance::Instance(const Window& window, const DynamicArray<const char*>& validationLayers)
        : m_Window(&window)
    {
		// Application
		VkApplicationInfo appInfo { VK_STRUCTURE_TYPE_APPLICATION_INFO };
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Onyx Samples";
		appInfo.pEngineName = "Onyx";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 2, 0);
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VULKAN_API_VERSION;
		appInfo.pNext = nullptr;
        
		std::vector<const char*> requiredExtensions;
		window.GetRequiredExtensions(requiredExtensions);

        if (m_EnableValidations)
			requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        if (VerifyExtensionSupport(requiredExtensions) == false)
		{
			ONYX_LOG_ERROR("Missing required extensions");
			return;
		}

		VkInstanceCreateInfo instanceCreateInfo { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext = nullptr;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		instanceCreateInfo.enabledExtensionCount = static_cast<onyxU32>(requiredExtensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{};
		if (m_EnableValidations)
		{
			if (VerifyValidationLayerSupport(validationLayers))
			{
				instanceCreateInfo.enabledLayerCount = static_cast<onyxU32>(validationLayers.size());
				instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
			}
			else
			{
				ONYX_LOG_ERROR("Missing required validation layers");
				return;
			}

			debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
			debugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugUtilsMessengerCreateInfo.pfnUserCallback = DebugUtilsMessenger::VulkanDebugCallback;
			instanceCreateInfo.pNext = static_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugUtilsMessengerCreateInfo);
		}
		else
		{
			instanceCreateInfo.enabledLayerCount = 0;
		}

		VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance));
    }

    Instance::~Instance()
    {
	    if (m_Instance != nullptr)
	    {
		    vkDestroyInstance(m_Instance, nullptr);
		    m_Instance = nullptr;
	    }
    }

    bool Instance::VerifyExtensionSupport(const std::vector<const char*> requiredExtensions) const
    {
		onyxU32 extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> supportedExtensions(extensionCount);
		VK_CHECK_RESULT_RETURN_ON_FAIL(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, supportedExtensions.data()));

		bool extensionsSupported = true;
		for (const char* extensionName : requiredExtensions)
		{
			extensionsSupported &= std::any_of(supportedExtensions.begin(), supportedExtensions.end(), [&](const VkExtensionProperties& extensionProperty)
				{
					if (strcmp(extensionProperty.extensionName, extensionName) == 0)
					{
						return true;
					}
					return false;
				});

			if (extensionsSupported == false)
			{
				ONYX_LOG_ERROR("Extension {} not supported", extensionName);
			}
		}

		return extensionsSupported;
    }

    bool Instance::VerifyValidationLayerSupport(const std::vector<const char*>& validationLayers) const
    {
		ONYX_ASSERT(m_EnableValidations);

		onyxU32 instanceLayerCount = 0;
		vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);

		std::vector<VkLayerProperties> supportedLayerProperties(instanceLayerCount);
		vkEnumerateInstanceLayerProperties(&instanceLayerCount, supportedLayerProperties.data());

		bool validationLayersSupported = true;
		for (const char* validationLayerName : validationLayers)
		{
			validationLayersSupported &= std::any_of(supportedLayerProperties.begin(), supportedLayerProperties.end(), [&](const VkLayerProperties& layerProperties)
				{
					if (strcmp(layerProperties.layerName, validationLayerName) == 0)
					{
						return true;
					}

					return false;
				});

			if (validationLayersSupported == false)
			{
				ONYX_LOG_ERROR("Validation layer not supported");
			}
		}

		return validationLayersSupported;
    }

	DynamicArray<VkPhysicalDevice> Instance::GetPhysicalDevices() const
    {
		DynamicArray<VkPhysicalDevice> outPhysicalDevices;

		onyxU32 physicalDevicesCount = 0;
	    vkEnumeratePhysicalDevices(m_Instance, &physicalDevicesCount, nullptr);

		outPhysicalDevices.resize(physicalDevicesCount);
	    vkEnumeratePhysicalDevices(m_Instance, &physicalDevicesCount, outPhysicalDevices.data());

		ONYX_ASSERT(!outPhysicalDevices.empty(), "Missing physical devices");
		return outPhysicalDevices;
    }

    bool Instance::CheckVulkanMinimumVersion(onyxU32 minVersion)
    {
	    onyxU32 version;
	    VK_CHECK_RESULT_RETURN_ON_FAIL(vkEnumerateInstanceVersion(&version));

	    return (minVersion > version);
    }

    bool Instance::CheckVulkanValidationLayerSupport(const std::vector<const char*>& validationLayers)
    {
	    onyxU32 instanceLayerCount = 0;
	    vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);

		DynamicArray<VkLayerProperties> availableLayers{ instanceLayerCount };
	    vkEnumerateInstanceLayerProperties(&instanceLayerCount, availableLayers.data());

	    for (const char* layer : validationLayers)
	    {
		    auto result = std::find_if(availableLayers.begin(), availableLayers.end(), [layer](const VkLayerProperties& layerProperties)
		    {
			    return strcmp(layer, layerProperties.layerName) == 0;
		    });

		    if (result == availableLayers.end())
		    {
			    ONYX_LOG_ERROR("could not find the requested validation layer: '{}'", layer);
			    return false;
		    }
	    }

	    return true;
    }
}
