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

    ONYX_NO_DISCARD VkQueue getGraphicsQueue() const { return m_graphicsQueue; }
    ONYX_NO_DISCARD VkQueue getComputeQueue() const { return m_computeQueue; }
    ONYX_NO_DISCARD VkQueue getTransferQueue() const { return m_transferQueue; }
    ONYX_NO_DISCARD VkQueue getPresentQueue() const { return m_presentQueue; }

  private:
    VULKAN_HANDLE( VkDevice, Device, nullptr );

    bool m_enableDebugMarkers = false;

    VkQueue m_graphicsQueue = nullptr;
    VkQueue m_computeQueue = nullptr;
    VkQueue m_transferQueue = nullptr;
    VkQueue m_presentQueue = nullptr;
};
} // namespace onyx::rhi::vulkan
