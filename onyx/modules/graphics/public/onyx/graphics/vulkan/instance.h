#pragma once

#include <onyx/noncopyable.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx
{
	class Engine;
}

namespace Onyx::Graphics
{
	class Window;
}

namespace Onyx::Graphics::Vulkan
{
	class Instance final : public NonCopyable
	{
	public:
		static constexpr onyxU32 VULKAN_API_VERSION = VK_API_VERSION_1_3;

		Instance(const Window& window, const std::vector<const char*>& validationLayers);
		~Instance();

		const Window* GetWindow() const { return m_Window; }

		const DynamicArray<VkExtensionProperties>& GetExtensions() const { return m_Extensions; }
		const DynamicArray<VkLayerProperties>& GetLayers() const { return m_Layers; }
		const DynamicArray<const char*>& GetValidationLayers() const { return m_ValidationLayers; }
		DynamicArray<VkPhysicalDevice> GetPhysicalDevices() const;

		bool GetValidationsEnabled() const { return m_EnableValidations; }

	private:
		bool VerifyExtensionSupport(std::vector<const char*> requiredExtensions) const;
		bool VerifyValidationLayerSupport(const std::vector<const char*>& validationLayers) const;

		static bool CheckVulkanMinimumVersion(onyxU32 minVersion);
		static bool CheckVulkanValidationLayerSupport(const std::vector<const char*>& validationLayers);

		bool m_EnableValidations = true;

		const Window* m_Window = nullptr;
		VULKAN_HANDLE(VkInstance, Instance, nullptr);

		DynamicArray<const char*> m_ValidationLayers;
		DynamicArray<VkExtensionProperties> m_Extensions;
		DynamicArray<VkLayerProperties> m_Layers;
	};
}