#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx {
class Engine;
}

namespace onyx::rhi {
struct GraphicSettings;
class Window;
} // namespace onyx::rhi

namespace onyx::rhi::vulkan {
class Instance final : public NonCopyable {
  public:
    static constexpr uint32_t VULKAN_API_VERSION = VK_API_VERSION_1_3;

    Instance( const GraphicSettings& settings, const std::vector< const char* >& validationLayers );
    ~Instance() override;

    const DynamicArray< VkExtensionProperties >& GetExtensions() const { return m_Extensions; }
    const DynamicArray< VkLayerProperties >& GetLayers() const { return m_Layers; }
    const DynamicArray< const char* >& GetValidationLayers() const { return m_ValidationLayers; }
    DynamicArray< VkPhysicalDevice > GetPhysicalDevices() const;

    bool GetValidationsEnabled() const { return m_EnableValidations; }

  private:
    bool VerifyExtensionSupport( std::vector< const char* > requiredExtensions ) const;
    bool VerifyValidationLayerSupport( const std::vector< const char* >& validationLayers ) const;

    static bool CheckVulkanMinimumVersion( uint32_t minVersion );
    static bool CheckVulkanValidationLayerSupport( const std::vector< const char* >& validationLayers );

    bool m_EnableValidations = true;

    VULKAN_HANDLE( VkInstance, Instance, nullptr );

    DynamicArray< const char* > m_ValidationLayers;
    DynamicArray< VkExtensionProperties > m_Extensions;
    DynamicArray< VkLayerProperties > m_Layers;
};
} // namespace onyx::rhi::vulkan