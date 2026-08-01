#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan {
class PhysicalDevice;

class Device : public NonCopyable {
  public:
    explicit Device( const PhysicalDevice& physicalDevice,
                     const DynamicArray< const char* >& enabledExtensions,
                     VkPhysicalDeviceFeatures2& physicalFeatures,
                     VkQueueFlags requestedQueueTypes );
    ~Device() override;

    [[nodiscard]] VkQueue getGraphicsQueue() const { return m_graphicsQueue; }
    [[nodiscard]] VkQueue getComputeQueue() const { return m_computeQueue; }
    [[nodiscard]] VkQueue getTransferQueue() const { return m_transferQueue; }
    [[nodiscard]] VkQueue getPresentQueue() const { return m_presentQueue; }

  private:
    VULKAN_HANDLE( VkDevice, Device, nullptr );

    bool m_enableDebugMarkers = false;

    VkQueue m_graphicsQueue = nullptr;
    VkQueue m_computeQueue = nullptr;
    VkQueue m_transferQueue = nullptr;
    VkQueue m_presentQueue = nullptr;
};
} // namespace onyx::rhi::vulkan
