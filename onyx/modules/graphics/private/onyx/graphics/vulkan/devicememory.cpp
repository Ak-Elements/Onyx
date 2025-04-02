#include <onyx/graphics/vulkan/devicememory.h>

#include <onyx/graphics/vulkan/device.h>
#include <onyx/graphics/vulkan/graphicsapi.h>
#include <onyx/graphics/vulkan/memoryallocator.h>

namespace Onyx::Graphics::Vulkan
{
    DeviceMemory::DeviceMemory(MemoryAllocator& allocator)
        : m_Allocator(&allocator)
    {
    }

    DeviceMemory::~DeviceMemory()
    {
	    if (m_Memory != nullptr)
		    m_Allocator->Free(m_Memory);
    }

    void* DeviceMemory::Map(MapMode mapMode)
    {
	    ONYX_ASSERT(m_MappedPtr == nullptr, "DeviceMemory::Map: Already mapped.");
	    m_MappedPtr = m_Allocator->Map(m_Memory, (mapMode == MapMode::Read) && m_IsNonCoherent);
	    m_IsWrite = Enums::HasAllFlags(mapMode, MapMode::Write);
	    return m_MappedPtr;
    }

    void DeviceMemory::Unmap()
    {
	    ONYX_ASSERT(m_MappedPtr != nullptr, "DeviceMemory::Unmap: Not mapped.");

	    m_Allocator->Unmap(m_Memory);
	    m_MappedPtr = nullptr;
	    /*VkMappedMemoryRange range = {};
	    GetMemoryRange(range);

	    if (m_IsWrite && m_IsNonCoherent)
		    VK_CHECK_RESULT(vkFlushMappedMemoryRanges(device.GetHandle(), 1, &range));

	    m_IsWrite = false;
	    m_MappedPtr = nullptr;

	    vkUnmapMemory(device.GetHandle(), range.memory);*/
    }

	void DeviceMemory::GetMemoryRange(VkMappedMemoryRange& range) const
    {
		ONYX_ASSERT(m_Memory != nullptr);
		range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;

		if (m_Memory)
		{
			const VmaAllocationInfo& allocInfo = m_Allocator->GetAllocationInfo(m_Memory);
			range.memory = allocInfo.deviceMemory;
			range.offset = allocInfo.offset;
			range.size = allocInfo.size;
		}
    }

	void DeviceMemory::GetMemoryPropertyFlags(const CPUAccess& cpuAccess, const GPUAccess& gpuAccess, VkMemoryPropertyFlags& outRequired, VkMemoryPropertyFlags& outPreferred)
	{
		ONYX_UNUSED(gpuAccess);
		switch (cpuAccess)
		{
		case CPUAccess::None:
		case CPUAccess::UpdateKeep:
			outRequired = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			return;
		case CPUAccess::Write:
			outRequired = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			return;
		case CPUAccess::Read:
			outRequired = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			return;
		case CPUAccess::UpdateUnsynchronized:
			outRequired = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			outPreferred = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			return;
		default:
			outRequired = 0;
			outPreferred = 0;
			ONYX_ASSERT(false, "Unhandled access type {}", static_cast<onyxU8>(cpuAccess));
			return;
		}
	}
}
