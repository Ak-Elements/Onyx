#pragma once

#include <onyx/graphics/memoryaccess.h>

namespace Onyx::Graphics
{
	enum class BufferUsage : onyxU8
	{
	    Vertex = 1,
		Index = 2,
		Uniform = 4,
		Storage = 8,
		Indirect = 16,
		DeviceAddress = 32,
	};

	ONYX_ENABLE_BITMASK_OPERATORS(BufferUsage);

	struct BufferProperties
	{
		onyxU32 m_Size = 0;
		onyxU32 m_UsageFlags : 6 = 0;
		onyxU32 m_StructSize : 23 = 0;
		bool m_IsWritable : 1 = false;
		bool m_AllowSuballocated : 1 = true;

        CPUAccess m_CpuAccess = CPUAccess::None;
        GPUAccess m_GpuAccess = GPUAccess::Read;

		String m_DebugName;
    };
}