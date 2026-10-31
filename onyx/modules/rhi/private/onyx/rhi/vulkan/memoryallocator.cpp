#include <onyx/rhi/vulkan/device.h>
#include <onyx/rhi/vulkan/graphicsapi.h>
#include <onyx/rhi/vulkan/instance.h>
#include <onyx/rhi/vulkan/memoryallocator.h>
#include <onyx/rhi/vulkan/physicaldevice.h>

#define VMA_ASSERT ONYX_ASSERT
#include <vulkanmemoryallocator.h>

namespace onyx::rhi::vulkan {
MemoryAllocator::MemoryAllocator( String tag, const VulkanGraphicsApi& graphicsApi )
    : m_tag( std::move( tag ) ) {
    /*VmaDeviceMemoryCallbacks deviceMemoryCallbacks{};
    deviceMemoryCallbacks.pfnAllocate = &MemoryAllocator::OnVmaAllocateMemory;
    deviceMemoryCallbacks.pUserData = this;*/

    // Initialize VulkanMemoryAllocator
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.vulkanApiVersion = Instance::VULKAN_API_VERSION;
    allocatorInfo.physicalDevice = graphicsApi.getPhysicalDevice().getHandle();
    allocatorInfo.device = graphicsApi.getDevice().getHandle();
    allocatorInfo.instance = graphicsApi.getInstance().getHandle();
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

    vmaCreateAllocator( &allocatorInfo, &m_allocator );
}

MemoryAllocator::~MemoryAllocator() {
    vmaDestroyAllocator( m_allocator );
    m_allocator = nullptr;
}

VmaAllocation MemoryAllocator::allocate( VkBuffer buffer,
                                         VkMemoryPropertyFlags /*requiredFlags*/,
                                         VkMemoryPropertyFlags /*preferredFlags*/ ) {
    VmaAllocationCreateInfo allocCreateInfo{};
    allocCreateInfo.pool = m_vmaPool;

    VmaAllocation allocation;
    VmaAllocationInfo allocInfo;
    if( vmaAllocateMemoryForBuffer( m_allocator, buffer, &allocCreateInfo, &allocation, &allocInfo ) != VK_SUCCESS )
        return nullptr;

    ONYX_ASSERT( allocInfo.deviceMemory == m_vmaAllocationDeviceMemory );
    return allocation;
}

VmaAllocation MemoryAllocator::allocate( VkImage image,
                                         VkMemoryPropertyFlags /*requiredFlags*/,
                                         VkMemoryPropertyFlags /*preferredFlags*/ ) {
    VmaAllocationCreateInfo allocCreateInfo{};
    allocCreateInfo.pool = m_vmaPool;

    VmaAllocation allocation;
    VmaAllocationInfo allocInfo;
    if( vmaAllocateMemoryForImage( m_allocator, image, &allocCreateInfo, &allocation, &allocInfo ) != VK_SUCCESS )
        return nullptr;

    ONYX_ASSERT( allocInfo.deviceMemory == m_vmaAllocationDeviceMemory );
    return allocation;
}

VmaAllocation MemoryAllocator::allocateDedicatedMemory( VkBuffer buffer,
                                                        uint32_t memoryTypeBits,
                                                        VkMemoryPropertyFlags requiredFlags,
                                                        VkMemoryPropertyFlags preferredFlags ) {
    VmaAllocationCreateInfo allocCreateInfo{};
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    // allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    allocCreateInfo.memoryTypeBits = memoryTypeBits;
    allocCreateInfo.requiredFlags = requiredFlags;
    allocCreateInfo.preferredFlags = preferredFlags;

    VmaAllocation allocation;
    VmaAllocationInfo allocInfo{};
    VK_CHECK_RESULT( vmaAllocateMemoryForBuffer( m_allocator, buffer, &allocCreateInfo, &allocation, &allocInfo ) )

    // TODO: Tracking
    // ONYX_LOG_DEBUG("MemoryAllocator ({}): allocating memory for buffer; size = {}", m_Tag, allocInfo.size);
    return allocation;
}

VmaAllocation MemoryAllocator::allocateDedicatedMemory( VkImage image,
                                                        uint32_t memoryTypeBits,
                                                        VkMemoryPropertyFlags requiredFlags,
                                                        VkMemoryPropertyFlags preferredFlags ) {
    VmaAllocationCreateInfo allocCreateInfo{};
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    // allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    allocCreateInfo.memoryTypeBits = memoryTypeBits;
    allocCreateInfo.requiredFlags = requiredFlags;
    allocCreateInfo.preferredFlags = preferredFlags;
    // allocCreateInfo.pUserData = nullptr;
    // allocCreateInfo.pool = nullptr;
    // allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    // allocCreateInfo.priority = 0.0f;

    VmaAllocation allocation;
    VmaAllocationInfo allocInfo{};
    VK_CHECK_RESULT( vmaAllocateMemoryForImage( m_allocator, image, &allocCreateInfo, &allocation, &allocInfo ) )

    // TODO: Tracking
    // ONYX_LOG_DEBUG("MemoryAllocator ({}): allocating image; size = {}", m_Tag, allocInfo.size);
    return allocation;
}

VmaAllocationInfo MemoryAllocator::getAllocationInfo( const VmaAllocation& allocation ) {
    VmaAllocationInfo allocInfo;
    vmaGetAllocationInfo( m_allocator, allocation, &allocInfo );
    return allocInfo;
}

VkImage MemoryAllocator::alias( VmaAllocation allocation, const VkImageCreateInfo& aliasInfo ) {
    VkImage alias = nullptr;
    VK_CHECK_RESULT( vmaCreateAliasingImage( m_allocator, allocation, &aliasInfo, &alias ) )
    return alias;
}

VkBuffer MemoryAllocator::alias( VmaAllocation allocation, const VkBufferCreateInfo& aliasInfo, uint64_t offset ) {
    VkBuffer alias = nullptr;
    VK_CHECK_RESULT( vmaCreateAliasingBuffer2( m_allocator, allocation, offset, &aliasInfo, &alias ) )
    return alias;
}

void MemoryAllocator::bind( VkBuffer buffer, VmaAllocation allocation ) {
    VK_CHECK_RESULT( vmaBindBufferMemory( m_allocator, allocation, buffer ) );
}

void MemoryAllocator::bind( VkImage image, VmaAllocation allocation ) {
    VK_CHECK_RESULT( vmaBindImageMemory( m_allocator, allocation, image ) );
}

void* MemoryAllocator::map( VmaAllocation allocation, bool shouldInvalidateMemory ) {
    void* mappedPtr = nullptr;
    if( shouldInvalidateMemory )
        VK_CHECK_RESULT( vmaInvalidateAllocation( m_allocator, allocation, 0, VK_WHOLE_SIZE ) )

    VkResult result = vmaMapMemory( m_allocator, allocation, &mappedPtr );
    if( result != VK_SUCCESS )
        shouldInvalidateMemory = true;

    VK_CHECK_RESULT( result )
    return mappedPtr;
}

void MemoryAllocator::unmap( VmaAllocation allocation ) {
    vmaUnmapMemory( m_allocator, allocation );
}

void MemoryAllocator::free( VmaAllocation allocation ) {
    vmaFreeMemory( m_allocator, allocation );
}

void MemoryAllocator::destroy( VkImage image, VmaAllocation allocation ) {
    ONYX_ASSERT( image != nullptr );
    ONYX_ASSERT( allocation != nullptr );
    vmaDestroyImage( m_allocator, image, allocation );
}

void MemoryAllocator::destroy( VkBuffer buffer, VmaAllocation allocation ) {
    ONYX_ASSERT( buffer != nullptr );
    ONYX_ASSERT( allocation != nullptr );
    vmaDestroyBuffer( m_allocator, buffer, allocation );
}

uint32_t MemoryAllocator::getAllocatedBytes() const {
    VmaStatistics stats{};
    vmaGetPoolStatistics( m_allocator, m_vmaPool, &stats );
    return static_cast< uint32_t >( stats.allocationBytes );
}

void MemoryAllocator::onVmaAllocateMemory( [[maybe_unused]] VmaAllocator allocator,
                                           [[maybe_unused]] uint32_t memoryType,
                                           VkDeviceMemory memory,
                                           [[maybe_unused]] VkDeviceSize size,
                                           void* userData ) {
    ONYX_ASSERT( userData != nullptr );
    MemoryAllocator* memoryAllocator = static_cast< MemoryAllocator* >( userData );
    ONYX_ASSERT( memoryAllocator->m_allocator == allocator );

    memoryAllocator->m_vmaAllocationDeviceMemory = memory;
}

void MemoryAllocator::initPool() {
    VmaPoolCreateInfo poolCreateInfo = {};
    poolCreateInfo.memoryTypeIndex = m_memoryType;

    poolCreateInfo.blockSize = m_poolSize;
    poolCreateInfo.minBlockCount = 1;
    poolCreateInfo.maxBlockCount = 1;

    VK_CHECK_RESULT( vmaCreatePool( m_allocator, &poolCreateInfo, &m_vmaPool ) )
}

} // namespace onyx::rhi::vulkan
