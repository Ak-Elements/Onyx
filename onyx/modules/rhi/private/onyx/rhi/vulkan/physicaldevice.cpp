#include <onyx/rhi/vulkan/instance.h>
#include <onyx/rhi/vulkan/physicaldevice.h>
#include <onyx/rhi/vulkan/surface.h>

namespace onyx::rhi::vulkan {
PhysicalDevice::PhysicalDevice( const Instance& instance ) {
    selectPhysicalDevice( instance );

    retrieveSupportedExtensions();
    retrieveQueueFamilyProperties();
    retrieveQueueFamilyIndices();

    m_depthFormat = getSupportedDepthFormat( true );
    m_multiSamplingLevel = getMaxUsableSampleCount();
}

PhysicalDevice::~PhysicalDevice() {}

void PhysicalDevice::retrieveQueueFamilyProperties() {
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties( m_PhysicalDevice, &queueFamilyCount, nullptr );

    ONYX_ASSERT( queueFamilyCount > 0 );
    m_queueFamilyProperties.resize( queueFamilyCount );

    vkGetPhysicalDeviceQueueFamilyProperties( m_PhysicalDevice, &queueFamilyCount, m_queueFamilyProperties.data() );
}

int32_t PhysicalDevice::getPresentQueueIndex( const Surface& surface ) const {
    VkBool32 supportsPresent = false;
    vkGetPhysicalDeviceSurfaceSupportKHR( m_PhysicalDevice,
                                          m_queueFamilyIndices.Graphics,
                                          surface.GetHandle(),
                                          &supportsPresent );
    if ( supportsPresent ) {
        return m_queueFamilyIndices.Graphics;
    }

    return -1;
}

void PhysicalDevice::selectPhysicalDevice( const Instance& instance ) {
    // TODO: Add support for integrated GPUs
    const std::vector< VkPhysicalDevice >& physicalDevices = instance.GetPhysicalDevices();

    VkPhysicalDevice physicalDevice = nullptr;
    VkPhysicalDeviceProperties2 properties2;
    VkPhysicalDeviceMaintenance4Properties maintenanceProperties { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_PROPERTIES };
    maintenanceProperties.maxBufferSize = 0;
    properties2.pNext = &maintenanceProperties;

    for ( size_t i = 0; i < physicalDevices.size(); i++ ) {
        physicalDevice = physicalDevices[ i ];
        vkGetPhysicalDeviceProperties2( physicalDevice, &properties2 );

        if ( properties2.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ) {
            m_PhysicalDevice = physicalDevice;
            m_properties = properties2.properties;
            m_maxBufferSize = maintenanceProperties.maxBufferSize;
            break;
        }
    }

    ONYX_ASSERT( m_PhysicalDevice, "Missing physical device for graphics" );
    ONYX_ASSERT( m_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU );

    vkGetPhysicalDeviceFeatures( m_PhysicalDevice, &m_features );
    vkGetPhysicalDeviceMemoryProperties( m_PhysicalDevice, &m_memoryProperties );
}

void PhysicalDevice::retrieveSupportedExtensions() {
    // Get list of supported extensions
    uint32_t extenstionsCount = 0;
    vkEnumerateDeviceExtensionProperties( m_PhysicalDevice, nullptr, &extenstionsCount, nullptr );
    if ( extenstionsCount > 0 ) {
        std::vector< VkExtensionProperties > extensions( extenstionsCount );
        if ( vkEnumerateDeviceExtensionProperties( m_PhysicalDevice,
                                                   nullptr,
                                                   &extenstionsCount,
                                                   &extensions.front() ) == VK_SUCCESS ) {
            m_supportedExtensions.reserve( extenstionsCount );
            for ( const VkExtensionProperties& extension : extensions ) {
                m_supportedExtensions.emplace( extension.extensionName );
            }
        }
    }
}

uint32_t PhysicalDevice::getMemoryType( uint32_t typeBits,
                                        VkMemoryPropertyFlags requiredPropertyFlags,
                                        VkMemoryPropertyFlags /*preferredPropertyFlags*/ ) const {
    for ( uint32_t i = 0; i < m_memoryProperties.memoryTypeCount; i++ ) {
        if ( ( typeBits & 1 ) == 1 ) {
            if ( ( m_memoryProperties.memoryTypes[ i ].propertyFlags & requiredPropertyFlags ) ==
                 requiredPropertyFlags ) {
                return i;
            }
        }
        typeBits >>= 1;
    }

    ONYX_ASSERT( false, "Could not find a matching memory type" );
    return std::numeric_limits< uint32_t >::max();
}

void PhysicalDevice::retrieveQueueFamilyIndices() {
    int32_t GraphicsFlagsCount = VK_QUEUE_FLAG_BITS_MAX_ENUM;
    int32_t ComputeFlagsCount = VK_QUEUE_FLAG_BITS_MAX_ENUM;
    int32_t TransferFlagsCount = VK_QUEUE_FLAG_BITS_MAX_ENUM;

    for ( int32_t i = 0; i < static_cast< int32_t >( m_queueFamilyProperties.size() ); ++i ) {
        const VkQueueFamilyProperties& queueFamily = m_queueFamilyProperties[ i ];
        if ( queueFamily.queueCount == 0 )
            continue;

        const int32_t bitCount = std::popcount( queueFamily.queueFlags );
        if ( ( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT ) != 0 ) {
            if ( bitCount < GraphicsFlagsCount ) {
                GraphicsFlagsCount = bitCount;
                m_queueFamilyIndices.Graphics = i;
            }
        }

        if ( ( queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT ) != 0 ) {
            if ( bitCount < ComputeFlagsCount ) {
                ComputeFlagsCount = bitCount;
                m_queueFamilyIndices.Compute = i;
            }
        }

        if ( ( queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT ) != 0 ) {
            if ( bitCount < TransferFlagsCount ) {
                TransferFlagsCount = bitCount;
                m_queueFamilyIndices.Transfer = i;
            }
        }
    }
}

bool PhysicalDevice::isExtensionSupported( StringView extension ) const {
    return m_supportedExtensions.find( extension.data() ) != m_supportedExtensions.end();
}

VkFormat PhysicalDevice::getSupportedDepthFormat( bool checkSamplingSupport ) const {
    // All depth formats may be optional, so we need to find a suitable depth format to use
    std::vector< VkFormat > depthFormats = { VK_FORMAT_D32_SFLOAT_S8_UINT,
                                             VK_FORMAT_D32_SFLOAT,
                                             VK_FORMAT_D24_UNORM_S8_UINT,
                                             VK_FORMAT_D16_UNORM_S8_UINT,
                                             VK_FORMAT_D16_UNORM };
    for ( auto& format : depthFormats ) {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties( m_PhysicalDevice, format, &formatProperties );
        // Format must support depth stencil attachment for optimal tiling
        if ( formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ) {
            if ( checkSamplingSupport ) {
                if ( !( formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT ) ) {
                    continue;
                }
            }
            return format;
        }
    }

    ONYX_LOG_FATAL( "Could not find a matching depth format" );
    return VK_FORMAT_UNDEFINED;
}

VkSampleCountFlagBits PhysicalDevice::getMaxUsableSampleCount() const {
    VkSampleCountFlags counts = std::min< VkSampleCountFlags >( m_properties.limits.framebufferColorSampleCounts,
                                                                m_properties.limits.framebufferDepthSampleCounts );
    if ( counts & VK_SAMPLE_COUNT_64_BIT ) {
        return VK_SAMPLE_COUNT_64_BIT;
    }
    if ( counts & VK_SAMPLE_COUNT_32_BIT ) {
        return VK_SAMPLE_COUNT_32_BIT;
    }
    if ( counts & VK_SAMPLE_COUNT_16_BIT ) {
        return VK_SAMPLE_COUNT_16_BIT;
    }
    if ( counts & VK_SAMPLE_COUNT_8_BIT ) {
        return VK_SAMPLE_COUNT_8_BIT;
    }
    if ( counts & VK_SAMPLE_COUNT_4_BIT ) {
        return VK_SAMPLE_COUNT_4_BIT;
    }
    if ( counts & VK_SAMPLE_COUNT_2_BIT ) {
        return VK_SAMPLE_COUNT_2_BIT;
    }
    return VK_SAMPLE_COUNT_1_BIT;
}
}; // namespace onyx::rhi::vulkan
