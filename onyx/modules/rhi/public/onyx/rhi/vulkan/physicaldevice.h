#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace Onyx::Graphics::Vulkan
{
    class Instance;
    class Surface;

    struct QueueFamilyIndices
    {
        onyxS32 Graphics = -1;
        onyxS32 Compute = -1;
        onyxS32 Transfer = -1;
    };

    enum class MemoryType : onyxU8
    {
        Gpu,
        CpuWrite,
        CpuRead,
        MemoryAccess,
        Count
    };

    class PhysicalDevice : public NonCopyable
    {
    public:
        explicit PhysicalDevice(const Instance& instance);
        ~PhysicalDevice();

        onyxU32 GetMemoryType(onyxU32 typeBits, VkMemoryPropertyFlags properties, VkMemoryPropertyFlags preferredPropertyFlags) const;
        VkQueue GetQueue(onyxS32 queueIndex) const;
        VkResult CreateLogicalDevice(VkPhysicalDeviceFeatures enabledFeatures, DynamicArray<const char*> enabledExtensions, void* pNextChain, bool useSwapChain = true, VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
        bool IsExtensionSupported(StringView extension) const;
        const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }

        onyxS32 GetGraphicsQueueIndex() const { return m_QueueFamilyIndices.Graphics; }
        onyxS32 GetComputeQueueIndex() const { return m_QueueFamilyIndices.Compute; }

        VkFormat GetDepthFormat() const { return m_DepthFormat; }

        template <typename T>
        void QueryFeatures2Info(T& outFeature)
        {
            vkGetPhysicalDeviceFeatures2(m_PhysicalDevice, &outFeature);
        }

        onyxS32 GetPresentQueueIndex(const Surface& surface) const;

    private:
        void SelectPhysicalDevice(const Instance& instance);
        void RetrieveSupportedExtensions();
        void RetrieveQueueFamilyProperties();
        void RetrieveQueueFamilyIndices();

        VkFormat GetSupportedDepthFormat(bool checkSamplingSupport) const;
        VkSampleCountFlagBits GetMaxUsableSampleCount() const;
    private:
        VULKAN_HANDLE(VkPhysicalDevice, PhysicalDevice, nullptr);

        VkPhysicalDeviceProperties m_Properties;
        VkPhysicalDeviceFeatures m_Features;
        VkPhysicalDeviceMemoryProperties m_MemoryProperties;

        DynamicArray<VkQueueFamilyProperties> m_QueueFamilyProperties;
        HashSet<String> m_SupportedExtensions;

        QueueFamilyIndices m_QueueFamilyIndices;

        VkFormat m_DepthFormat = VK_FORMAT_UNDEFINED;
        VkSampleCountFlagBits m_MultiSamplingLevel = VK_SAMPLE_COUNT_1_BIT;
        bool m_EnableDebugMarkers = false;
    };
}