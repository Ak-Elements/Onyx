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
    MemoryAllocator( const std::string& tag, const VulkanGraphicsApi& graphicsApi );
    ~MemoryAllocator();

    VmaAllocation Allocate( VkBuffer buffer,
                            VkMemoryPropertyFlags requiredFlags,
                            VkMemoryPropertyFlags preferredFlags );
    VmaAllocation Allocate( VkImage buffer, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags );
    VmaAllocation AllocateDedicatedMemory( VkBuffer buffer,
                                           uint32_t memoryTypeBits,
                                           VkMemoryPropertyFlags requiredFlags,
                                           VkMemoryPropertyFlags preferredFlags );
    VmaAllocation AllocateDedicatedMemory( VkImage image,
                                           uint32_t memoryTypeBits,
                                           VkMemoryPropertyFlags requiredFlags,
                                           VkMemoryPropertyFlags preferredFlags );
    VmaAllocationInfo GetAllocationInfo( const VmaAllocation& allocation );

    VkImage Alias( VmaAllocation allocation, const VkImageCreateInfo& aliasInfo );
    VkBuffer Alias( VmaAllocation allocation, const VkBufferCreateInfo& aliasInfo, uint64_t offset );

    void Bind( VkBuffer buffer, VmaAllocation allocation );
    void Bind( VkImage image, VmaAllocation allocation );

    void* Map( VmaAllocation allocation, bool shouldInvalidateMemory );
    void Unmap( VmaAllocation allocation );

    void Free( VmaAllocation allocation );

    void Destroy( VkImage image, VmaAllocation allocation );
    void Destroy( VkBuffer buffer, VmaAllocation allocation );

    uint32_t GetAllocatedBytes() const;

  private:
    static void OnVmaAllocateMemory( VmaAllocator_T* allocator,
                                     uint32_t memoryType,
                                     VkDeviceMemory memory,
                                     VkDeviceSize size,
                                     void* userData );

    void InitPool();

  private:
    uint8_t m_MemoryType = 0; // default

    uint32_t m_PoolSize = 0;

    VmaAllocator_T* m_Allocator = nullptr;
    VmaPool_T* m_VmaPool = nullptr;
    VkDeviceMemory m_VmaAllocationDeviceMemory = nullptr;

    std::string m_Tag;
};
} // namespace onyx::rhi::vulkan
