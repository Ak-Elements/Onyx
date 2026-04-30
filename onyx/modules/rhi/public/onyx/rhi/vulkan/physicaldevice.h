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
    ~PhysicalDevice() override;

    uint32_t getMemoryType( uint32_t typeBits,
                            VkMemoryPropertyFlags properties,
                            VkMemoryPropertyFlags preferredPropertyFlags ) const;
    VkQueue getQueue( int32_t queueIndex ) const;
    VkResult createLogicalDevice( VkPhysicalDeviceFeatures enabledFeatures,
                                  DynamicArray< const char* > enabledExtensions,
                                  void* pNextChain,
                                  bool useSwapChain = true,
                                  VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT );
    bool isExtensionSupported( StringView extension ) const;
    const QueueFamilyIndices& getQueueFamilyIndices() const { return m_queueFamilyIndices; }

    int32_t getGraphicsQueueIndex() const { return m_queueFamilyIndices.Graphics; }
    int32_t getComputeQueueIndex() const { return m_queueFamilyIndices.Compute; }

    VkFormat getDepthFormat() const { return m_depthFormat; }
    size_t getMaxBufferSize() const { return m_maxBufferSize; }

    template < typename T >
    void queryFeatures2Info( T& outFeature ) {
        vkGetPhysicalDeviceFeatures2( m_PhysicalDevice, &outFeature );
    }

    int32_t getPresentQueueIndex( const Surface& surface ) const;

  private:
    void selectPhysicalDevice( const Instance& instance );
    void retrieveSupportedExtensions();
    void retrieveQueueFamilyProperties();
    void retrieveQueueFamilyIndices();

    VkFormat getSupportedDepthFormat( bool checkSamplingSupport ) const;
    VkSampleCountFlagBits getMaxUsableSampleCount() const;

  private:
    VULKAN_HANDLE( VkPhysicalDevice, PhysicalDevice, nullptr );

    VkPhysicalDeviceProperties m_properties;
    VkPhysicalDeviceFeatures m_features;
    VkPhysicalDeviceMemoryProperties m_memoryProperties;

    DynamicArray< VkQueueFamilyProperties > m_queueFamilyProperties;
    HashSet< String > m_supportedExtensions;

    QueueFamilyIndices m_queueFamilyIndices;

    size_t m_maxBufferSize = 0;
    VkFormat m_depthFormat = VK_FORMAT_UNDEFINED;
    VkSampleCountFlagBits m_multiSamplingLevel = VK_SAMPLE_COUNT_1_BIT;
    bool m_enableDebugMarkers = false;
};
} // namespace onyx::rhi::vulkan
