#pragma once

#include <onyx/noncopyable.h>
#include <onyx/graphics/vulkan/vulkan.h>
#include <onyx/graphics/vulkan/buffer.h>
#include <onyx/graphics/vulkan/graphicsapi.h>

#define VMA_ASSERT ONYX_ASSERT
#include <vk_mem_alloc.h>

namespace Onyx::Graphics::Vulkan
{
	class Device;
	class Instance;
	class PhysicalDevice;
    struct GpuMemoryStats
    {
        onyxU64 Used = 0;
        onyxU64 Free = 0;
    };

    class MemoryAllocator : public NonCopyable
    {
    public:
		MemoryAllocator(const std::string& tag, const VulkanGraphicsApi& graphicsApi);
		~MemoryAllocator();

		VmaAllocation Allocate(VkBuffer buffer, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags);
		VmaAllocation Allocate(VkImage buffer, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags);
        VmaAllocation AllocateDedicatedMemory(VkBuffer buffer, onyxU32 memoryTypeBits, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags);
		VmaAllocation AllocateDedicatedMemory(VkImage image, onyxU32 memoryTypeBits, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags);
		VmaAllocationInfo GetAllocationInfo(const VmaAllocation& allocation);

		VkImage Alias(VmaAllocation allocation, const VkImageCreateInfo& aliasInfo);
		VkBuffer Alias(VmaAllocation allocation, const VkBufferCreateInfo& aliasInfo);

		void Bind(VkBuffer buffer, VmaAllocation allocation);
		void Bind(VkImage image, VmaAllocation allocation);

		void* Map(VmaAllocation allocation, bool shouldInvalidateMemory);
		void Unmap(VmaAllocation allocation);

		void Free(VmaAllocation allocation);

		void Destroy(VkImage image, VmaAllocation allocation);
		void Destroy(VkBuffer buffer, VmaAllocation allocation);

		onyxU32 GetAllocatedBytes() const;

    private:
		static void OnVmaAllocateMemory(VmaAllocator allocator, onyxU32 memoryType, VkDeviceMemory memory, VkDeviceSize size, void* userData);

		void InitPool();
	private:
		onyxU8 m_Alignment = 0;
		onyxU8 m_MemoryType = 0; // default

		onyxU32 m_PoolSize = 0;

        VmaAllocator m_Allocator = nullptr;
		VmaPool m_VmaPool = nullptr;
		VkDeviceMemory m_VmaAllocationDeviceMemory = nullptr;

		std::string m_Tag;
    };
}
