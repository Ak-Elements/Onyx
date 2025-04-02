#pragma once

#include <onyx/noncopyable.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx::Graphics::Vulkan
{
    class PhysicalDevice;

    class Device : public NonCopyable
    {
    public:
        explicit Device(const PhysicalDevice& physicalDevice, const DynamicArray<const char*>& enabledExtensions, VkPhysicalDeviceFeatures2& physicalFeatures, VkQueueFlags requestedQueueTypes);
        ~Device();

        VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
        VkQueue GetComputeQueue() const { return m_ComputeQueue; }
        VkQueue GetTransferQueue() const { return m_TransferQueue; }
        VkQueue GetPresentQueue() const { return m_PresentQueue; }

    private:
        VULKAN_HANDLE(VkDevice, Device, nullptr);

        bool m_EnableDebugMarkers = false;

        VkQueue m_GraphicsQueue = nullptr;
        VkQueue m_ComputeQueue = nullptr;
        VkQueue m_TransferQueue = nullptr;
        VkQueue m_PresentQueue = nullptr;
    };
}
