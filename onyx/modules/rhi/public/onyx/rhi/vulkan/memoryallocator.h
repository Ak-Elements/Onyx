#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/vulkan/buffer.h>
#include <onyx/rhi/vulkan/vulkan.h>

struct VmaAllocationInfo;
struct VmaAllocator_T;
struct VmaPool_T;

namespace onyx::rhi::vulkan {
class Device;
class Instance;
class PhysicalDevice;
struct GpuMemoryStats {
    uint64_t Used = 0;
    uint64_t Free = 0;
};

class MemoryAllocator : public NonCopyable {
  public:
    MemoryAllocator( String tag, const VulkanGraphicsApi& graphicsApi );
    ~MemoryAllocator();

    VmaAllocation allocate( VkBuffer buffer,
                            VkMemoryPropertyFlags requiredFlags,
                            VkMemoryPropertyFlags preferredFlags );
    VmaAllocation allocate( VkImage buffer, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags );
    VmaAllocation allocateDedicatedMemory( VkBuffer buffer,
                                           uint32_t memoryTypeBits,
                                           VkMemoryPropertyFlags requiredFlags,
                                           VkMemoryPropertyFlags preferredFlags );
    VmaAllocation allocateDedicatedMemory( VkImage image,
                                           uint32_t memoryTypeBits,
                                           VkMemoryPropertyFlags requiredFlags,
                                           VkMemoryPropertyFlags preferredFlags );
    VmaAllocationInfo getAllocationInfo( const VmaAllocation& allocation );

    VkImage alias( VmaAllocation allocation, const VkImageCreateInfo& aliasInfo );
    VkBuffer alias( VmaAllocation allocation, const VkBufferCreateInfo& aliasInfo, uint64_t offset );

    void bind( VkBuffer buffer, VmaAllocation allocation );
    void bind( VkImage image, VmaAllocation allocation );

    void* map( VmaAllocation allocation, bool shouldInvalidateMemory );
    void unmap( VmaAllocation allocation );

    void free( VmaAllocation allocation );

    void destroy( VkImage image, VmaAllocation allocation );
    void destroy( VkBuffer buffer, VmaAllocation allocation );

    [[nodiscard]] uint32_t getAllocatedBytes() const;

  private:
    static void onVmaAllocateMemory( VmaAllocator_T* allocator,
                                     uint32_t memoryType,
                                     VkDeviceMemory memory,
                                     VkDeviceSize size,
                                     void* userData );

    void initPool();

  private:
    uint8_t m_memoryType = 0; // default

    uint32_t m_poolSize = 0;

    VmaAllocator_T* m_allocator = nullptr;
    VmaPool_T* m_vmaPool = nullptr;
    VkDeviceMemory m_vmaAllocationDeviceMemory = nullptr;

    String m_tag;
};
} // namespace onyx::rhi::vulkan
