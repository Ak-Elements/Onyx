#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/memoryaccess.h>
#include <onyx/rhi/vulkan/vulkan.h>

struct VmaAllocation_T;
using VmaAllocation = VmaAllocation_T*;

namespace onyx::rhi {
enum class CPUAccess : uint8_t;
enum class GPUAccess : uint8_t;

namespace vulkan {
class MemoryAllocator;
class Device;
class VulkanGraphicsApi;

class DeviceMemory : NonCopyable {
  public:
    DeviceMemory() = default;
    DeviceMemory( MemoryAllocator& allocator );
    virtual ~DeviceMemory();

    void* Map( MapMode mode );
    void Unmap();

  protected:
    void GetMemoryPropertyFlags( const CPUAccess& cpuAccess,
                                 const GPUAccess& gpuAccess,
                                 VkMemoryPropertyFlags& outRequired,
                                 VkMemoryPropertyFlags& outPreferred );
    void GetMemoryRange( VkMappedMemoryRange& aRange ) const;

  protected:
    MemoryAllocator* m_Allocator = nullptr;
    VmaAllocation m_Memory = nullptr;

  protected:
    void* m_MappedPtr = nullptr;

    bool m_IsWrite = false;
    bool m_IsNonCoherent = false;
};
} // namespace vulkan
} // namespace onyx::rhi