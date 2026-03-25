#pragma once

#include <onyx/onyx.h>
#include <onyx/rhi/graphicstypes.h>
#include <onyx/rhi/memoryaccess.h>

namespace onyx::rhi {
struct TextureStorageProperties {
    TextureStorageProperties() = default;

    uint32_t GetLocatorCount() const;

    bool operator==( const TextureStorageProperties& anOther ) const {
        return std::memcmp( this, &anOther, sizeof anOther ) == 0;
    }
    bool operator!=( const TextureStorageProperties& anOther ) const {
        return std::memcmp( this, &anOther, sizeof anOther ) != 0;
    }

    Vector3s32 m_Size = { 1, 1, 1 };
    uint16_t m_ArraySize = 0;
    uint8_t m_MaxMipLevel = 1;

    MSAAProperties m_MSAAProperties;

    CPUAccess m_CpuAccess = CPUAccess::None;
    GPUAccess m_GpuAccess = GPUAccess::Read;

    TextureType m_Type = TextureType::Texture2D;
    TextureFormat m_Format = TextureFormat::Invalid;

    uint8_t m_HintFlags = 0;

    // Add tilemode?
    bool m_IsTexture : 1 = false;
    bool m_IsFrameBuffer : 1 = false;
    bool m_IsWritable : 1 = false;
    bool m_IsPartiallyResident : 1 = false;
    bool m_Padding1 : 3 = false;

    String m_DebugName;
};
} // namespace onyx::rhi
