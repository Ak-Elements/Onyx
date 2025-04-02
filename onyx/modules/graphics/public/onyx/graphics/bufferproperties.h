#pragma once

#include <onyx/graphics/memoryaccess.h>

namespace Onyx::Graphics
{
	enum class BufferType : onyxU8
	{
	    Vertex = 1,
		Index = 2,
		Uniform = 4,
		Buffer = 8,
		Indirect = 16,
	};

	ONYX_ENABLE_BITMASK_OPERATORS(BufferType);

	struct BufferProperties
	{
		onyxU32 m_Size = 0;
		onyxU32 m_BindFlags : 6 = 0;
		onyxU32 m_StructSize : 23 = 0;
		bool m_IsWritable : 1 = false;
		bool m_AllowSuballocated : 1 = true;

        CPUAccess m_CpuAccess = CPUAccess::None;
        GPUAccess m_GpuAccess = GPUAccess::Read;

		String m_DebugName;
    };
}