#pragma once

namespace Onyx::Graphics
{
	enum class GPUAccess : onyxU8
	{
		Read,
		Write,
		Staging,			// copying in/out of GPU only
	};

	enum class CPUAccess : onyxU8
	{
		None,
		UpdateUnsynchronized,
		UpdateKeep,
		Read, // mostly for staging
		Write,
		ReadWrite
	};

	enum class MapMode : onyxU8
	{
		Write = 1 << 0,
		Read = 1 << 1,
		ReadWrite = Write | Read,

		// unsynchronized requires external synchronization
		Unsynchronized = 1 << 2,
		WriteUnsynchronized = Write | Unsynchronized,
		ReadUnsynchronized = Read | Unsynchronized,
		ReadWriteUnsynchronized = ReadWrite | Unsynchronized,
	};

	ONYX_ENABLE_BITMASK_OPERATORS(MapMode);
}