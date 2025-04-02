#pragma once

#include <onyx/onyx.h>
#include <onyx/graphics/memoryaccess.h>
#include <onyx/graphics/graphicstypes.h>

namespace Onyx::Graphics
{
	struct TextureStorageProperties
	{
		TextureStorageProperties() = default;

		onyxU32 GetLocatorCount() const;

		bool operator==(const TextureStorageProperties& anOther) const { return memcmp(this, &anOther, sizeof anOther) == 0; }
		bool operator!=(const TextureStorageProperties& anOther) const { return memcmp(this, &anOther, sizeof anOther) != 0; }

		Vector3s32 m_Size = {1, 1, 1};
		onyxU16 m_ArraySize = 0;
		onyxU8 m_MaxMipLevel = 1;

		MSAAProperties m_MSAAProperties;

        CPUAccess m_CpuAccess = CPUAccess::None;
        GPUAccess m_GpuAccess = GPUAccess::Read;

        TextureType m_Type = TextureType::Texture2D;
		TextureFormat m_Format = TextureFormat::Invalid;

		onyxU8 m_HintFlags = 0;

		// Add tilemode?
		bool m_IsTexture : 1 = false;
		bool m_IsFrameBuffer : 1 = false;
		bool m_IsWritable : 1 = false;
		bool m_IsPartiallyResident : 1 = false;
		bool m_Padding1 : 3 = false;

		String m_DebugName;
	};
}
