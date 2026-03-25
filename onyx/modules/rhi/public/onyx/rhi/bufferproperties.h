#pragma once

#include <onyx/rhi/memoryaccess.h>

namespace onyx::rhi {
enum class BufferUsage : uint8_t {
    Vertex = 1,
    Index = 2,
    Uniform = 4,
    Storage = 8,
    Indirect = 16,
    DeviceAddress = 32,
    Conditional = 64,
};

struct BufferProperties {
    String m_DebugName;

    uint64_t m_Size = 0;
    uint8_t m_UsageFlags = 0;
    CPUAccess m_CpuAccess = CPUAccess::None;
    GPUAccess m_GpuAccess = GPUAccess::Read;

    bool m_IsWritable = false;
    bool m_AllowSuballocated = true;
};
} // namespace onyx::rhi