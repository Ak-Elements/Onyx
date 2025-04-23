#include <onyx/graphics/vulkan/memoryallocator.h>
#include <onyx/graphics/vulkan/device.h>
#include <onyx/graphics/vulkan/instance.h>
#include <onyx/graphics/vulkan/physicaldevice.h>
#include <onyx/graphics/vulkan/graphicsapi.h>

#define VMA_ASSERT ONYX_ASSERT
#include <vulkanmemoryallocator.h>

namespace Onyx::Graphics::Vulkan
{
    MemoryAllocator::MemoryAllocator(const String& tag, const VulkanGraphicsApi& graphicsApi)
        : m_Tag(tag)
    {
        /*VmaDeviceMemoryCallbacks deviceMemoryCallbacks{};
        deviceMemoryCallbacks.pfnAllocate = &MemoryAllocator::OnVmaAllocateMemory;
        deviceMemoryCallbacks.pUserData = this;*/

        // Initialize VulkanMemoryAllocator
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.vulkanApiVersion = Instance::VULKAN_API_VERSION;
        allocatorInfo.physicalDevice = graphicsApi.GetPhysicalDevice().GetHandle();
        allocatorInfo.device = graphicsApi.GetDevice().GetHandle();
        allocatorInfo.instance = graphicsApi.GetInstance().GetHandle();

        vmaCreateAllocator(&allocatorInfo, &m_Allocator);
    }

    MemoryAllocator::~MemoryAllocator()
    {
        vmaDestroyAllocator(m_Allocator);
        m_Allocator = nullptr;
    }

    VmaAllocation MemoryAllocator::Allocate(VkBuffer buffer, VkMemoryPropertyFlags /*requiredFlags*/, VkMemoryPropertyFlags /*preferredFlags*/)
    {
        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.pool = m_VmaPool;

        VmaAllocation allocation;
        VmaAllocationInfo allocInfo;
        if (vmaAllocateMemoryForBuffer(m_Allocator, buffer, &allocCreateInfo, &allocation, &allocInfo) != VK_SUCCESS)
            return nullptr;

        ONYX_ASSERT(allocInfo.deviceMemory == m_VmaAllocationDeviceMemory);
        return allocation;
    }

    VmaAllocation MemoryAllocator::Allocate(VkImage image, VkMemoryPropertyFlags /*requiredFlags*/, VkMemoryPropertyFlags /*preferredFlags*/)
    {
        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.pool = m_VmaPool;

        VmaAllocation allocation;
        VmaAllocationInfo allocInfo;
        if (vmaAllocateMemoryForImage(m_Allocator, image, &allocCreateInfo, &allocation, &allocInfo) != VK_SUCCESS)
            return nullptr;

        ONYX_ASSERT(allocInfo.deviceMemory == m_VmaAllocationDeviceMemory);
        return allocation;
    }

    VmaAllocation MemoryAllocator::AllocateDedicatedMemory(VkBuffer buffer, onyxU32 memoryTypeBits, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags)
    {
        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
        allocCreateInfo.memoryTypeBits = memoryTypeBits;
        allocCreateInfo.requiredFlags = requiredFlags;
        allocCreateInfo.preferredFlags = preferredFlags;
        
        VmaAllocation allocation;
        VmaAllocationInfo allocInfo{};
        VK_CHECK_RESULT(vmaAllocateMemoryForBuffer(m_Allocator, buffer, &allocCreateInfo, &allocation, &allocInfo))

        // TODO: Tracking
        ONYX_LOG_DEBUG("MemoryAllocator ({}): allocating memory for buffer; size = {}", m_Tag, allocInfo.size);
        
        return allocation;
    }

    VmaAllocation MemoryAllocator::AllocateDedicatedMemory(VkImage image, onyxU32 memoryTypeBits, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags)
    {
        VmaAllocationCreateInfo allocCreateInfo {};
        allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
        allocCreateInfo.memoryTypeBits = memoryTypeBits;
        allocCreateInfo.requiredFlags = requiredFlags;
        allocCreateInfo.preferredFlags = preferredFlags;
        //allocCreateInfo.pUserData = nullptr;
        //allocCreateInfo.pool = nullptr;
        //allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        //allocCreateInfo.priority = 0.0f;

        VmaAllocation allocation;
        VmaAllocationInfo allocInfo{};
        VK_CHECK_RESULT(vmaAllocateMemoryForImage(m_Allocator, image, &allocCreateInfo, &allocation, &allocInfo))

        // TODO: Tracking
        ONYX_LOG_DEBUG("MemoryAllocator ({}): allocating image; size = {}", m_Tag, allocInfo.size);

        return allocation;
    }

    VmaAllocationInfo MemoryAllocator::GetAllocationInfo(const VmaAllocation& allocation)
    {
        VmaAllocationInfo allocInfo;
        vmaGetAllocationInfo(m_Allocator, allocation, &allocInfo);
        return allocInfo;
    }

    VkImage MemoryAllocator::Alias(VmaAllocation allocation, const VkImageCreateInfo& aliasInfo)
    {
        VkImage alias = nullptr;
        VK_CHECK_RESULT(vmaCreateAliasingImage(m_Allocator, allocation, &aliasInfo, &alias))
        return alias;
    }

    VkBuffer MemoryAllocator::Alias(VmaAllocation allocation, const VkBufferCreateInfo& aliasInfo)
    {
        VkBuffer alias = nullptr;
        VK_CHECK_RESULT(vmaCreateAliasingBuffer(m_Allocator, allocation, &aliasInfo, &alias))
        return alias;
    }

    void MemoryAllocator::Bind(VkBuffer buffer, VmaAllocation allocation)
    {
        VK_CHECK_RESULT(vmaBindBufferMemory(m_Allocator, allocation, buffer));
    }

    void MemoryAllocator::Bind(VkImage image, VmaAllocation allocation)
    {
        VK_CHECK_RESULT(vmaBindImageMemory(m_Allocator, allocation, image));
    }

    void* MemoryAllocator::Map(VmaAllocation allocation, bool shouldInvalidateMemory)
    {
        void* mappedPtr = nullptr;
        if (shouldInvalidateMemory)
            VK_CHECK_RESULT(vmaInvalidateAllocation(m_Allocator, allocation, 0, VK_WHOLE_SIZE))

        VkResult result = vmaMapMemory(m_Allocator, allocation, &mappedPtr);
        if (result != VK_SUCCESS)
            shouldInvalidateMemory = true;

        VK_CHECK_RESULT(result)
        return mappedPtr;
    }

    void MemoryAllocator::Unmap(VmaAllocation allocation)
    {
        vmaUnmapMemory(m_Allocator, allocation);
    }

    void MemoryAllocator::Free(VmaAllocation allocation)
    {
        vmaFreeMemory(m_Allocator, allocation);
    }

    void MemoryAllocator::Destroy(VkImage image, VmaAllocation allocation)
    {
        ONYX_ASSERT(image != nullptr);
        ONYX_ASSERT(allocation != nullptr);
        vmaDestroyImage(m_Allocator, image, allocation);
    }

    void MemoryAllocator::Destroy(VkBuffer buffer, VmaAllocation allocation)
    {
        ONYX_ASSERT(buffer != nullptr);
        ONYX_ASSERT(allocation != nullptr);
        vmaDestroyBuffer(m_Allocator, buffer, allocation);
    }

    onyxU32 MemoryAllocator::GetAllocatedBytes() const
    {
        VmaStatistics stats{};
        vmaGetPoolStatistics(m_Allocator, m_VmaPool, &stats);
        return static_cast<onyxU32>(stats.allocationBytes);
    }

    void MemoryAllocator::OnVmaAllocateMemory(VmaAllocator allocator, onyxU32 memoryType, VkDeviceMemory memory, VkDeviceSize size, void* userData)
    {
        ONYX_ASSERT(userData != nullptr);
        MemoryAllocator* memoryAllocator = static_cast<MemoryAllocator*>(userData);
        ONYX_ASSERT(memoryAllocator->m_Allocator == allocator);

        memoryAllocator->m_VmaAllocationDeviceMemory = memory;

        ONYX_UNUSED(allocator);
        ONYX_UNUSED(memoryType);
        ONYX_UNUSED(size);
    }

    void MemoryAllocator::InitPool()
    {
        VmaPoolCreateInfo poolCreateInfo = {};
        poolCreateInfo.memoryTypeIndex = m_MemoryType;
        
        poolCreateInfo.blockSize = m_PoolSize;
        poolCreateInfo.minBlockCount = 1;
        poolCreateInfo.maxBlockCount = 1;

        VK_CHECK_RESULT(vmaCreatePool(m_Allocator, &poolCreateInfo, &m_VmaPool))
    }

}
