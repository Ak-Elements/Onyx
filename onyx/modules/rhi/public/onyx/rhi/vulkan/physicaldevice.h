#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan {
class Instance;
class Surface;

struct QueueFamilyIndices {
    int32_t Graphics = -1;
    int32_t Compute = -1;
    int32_t Transfer = -1;
};

enum class MemoryType : uint8_t { Gpu, CpuWrite, CpuRead, MemoryAccess, Count };

class PhysicalDevice : public NonCopyable {
  public:
    explicit PhysicalDevice( const Instance& instance );
    ~PhysicalDevice();

    uint32_t GetMemoryType( uint32_t typeBits,
                            VkMemoryPropertyFlags properties,
                            VkMemoryPropertyFlags preferredPropertyFlags ) const;
    VkQueue GetQueue( int32_t queueIndex ) const;
    VkResult CreateLogicalDevice( VkPhysicalDeviceFeatures enabledFeatures,
                                  DynamicArray< const char* > enabledExtensions,
                                  void* pNextChain,
                                  bool useSwapChain = true,
                                  VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT );
    bool IsExtensionSupported( StringView extension ) const;
    const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }

    int32_t GetGraphicsQueueIndex() const { return m_QueueFamilyIndices.Graphics; }
    int32_t GetComputeQueueIndex() const { return m_QueueFamilyIndices.Compute; }

    VkFormat GetDepthFormat() const { return m_DepthFormat; }

    template < typename T >
    void QueryFeatures2Info( T& outFeature ) {
        vkGetPhysicalDeviceFeatures2( m_PhysicalDevice, &outFeature );
    }

    int32_t GetPresentQueueIndex( const Surface& surface ) const;

  private:
    void SelectPhysicalDevice( const Instance& instance );
    void RetrieveSupportedExtensions();
    void RetrieveQueueFamilyProperties();
    void RetrieveQueueFamilyIndices();

    VkFormat GetSupportedDepthFormat( bool checkSamplingSupport ) const;
    VkSampleCountFlagBits GetMaxUsableSampleCount() const;

  private:
    VULKAN_HANDLE( VkPhysicalDevice, PhysicalDevice, nullptr );

    VkPhysicalDeviceProperties m_Properties;
    VkPhysicalDeviceFeatures m_Features;
    VkPhysicalDeviceMemoryProperties m_MemoryProperties;

    DynamicArray< VkQueueFamilyProperties > m_QueueFamilyProperties;
    HashSet< String > m_SupportedExtensions;

    QueueFamilyIndices m_QueueFamilyIndices;

    VkFormat m_DepthFormat = VK_FORMAT_UNDEFINED;
    VkSampleCountFlagBits m_MultiSamplingLevel = VK_SAMPLE_COUNT_1_BIT;
    bool m_EnableDebugMarkers = false;
};
} // namespace onyx::rhi::vulkan