#pragma once

#include <onyx/noncopyable.h>
#include <onyx/graphics/memoryaccess.h>
#include <onyx/graphics/vulkan/vulkan.h>

struct VmaAllocation_T;
using VmaAllocation = VmaAllocation_T*;

namespace Onyx::Graphics
{
    enum class CPUAccess : onyxU8;
    enum class GPUAccess : onyxU8;

    namespace Vulkan
    {
        class MemoryAllocator;
        class Device;
        class VulkanGraphicsApi;

        class DeviceMemory : NonCopyable
        {
        public:
            DeviceMemory() = default;
            DeviceMemory(MemoryAllocator& allocator);
            virtual ~DeviceMemory();

            void* Map(MapMode mode);
            void Unmap();

        protected:
            void GetMemoryPropertyFlags(const CPUAccess& cpuAccess, const GPUAccess& gpuAccess, VkMemoryPropertyFlags& outRequired, VkMemoryPropertyFlags& outPreferred);
            void GetMemoryRange(VkMappedMemoryRange& aRange) const;

        protected:
            MemoryAllocator* m_Allocator = nullptr;
            VmaAllocation m_Memory = nullptr;
        protected:
            void* m_MappedPtr = nullptr;

            bool m_IsWrite = false;
            bool m_IsNonCoherent = false;
        };
    }
}