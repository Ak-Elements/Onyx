#pragma once

#include <onyx/onyx.h>
#include <onyx/rhi/graphicstypes.h>
#include <onyx/rhi/memoryaccess.h>

namespace onyx::rhi {
struct TextureStorageProperties {
    TextureStorageProperties() = default;

    [[nodiscard]] uint32_t getLocatorCount() const;

    bool operator==( const TextureStorageProperties& anOther ) const {
        return std::memcmp( this, &anOther, sizeof anOther ) == 0;
    }
    bool operator!=( const TextureStorageProperties& anOther ) const {
        return std::memcmp( this, &anOther, sizeof anOther ) != 0;
    }

    Vector3s32 Size = { 1, 1, 1 };
    uint16_t ArraySize = 0;
    uint8_t MaxMipLevel = 1;

    MSAAProperties MsaaProperties;

    CPUAccess CpuAccess = CPUAccess::None;
    GPUAccess GpuAccess = GPUAccess::Read;

    TextureType Type = TextureType::Texture2D;
    TextureFormat Format = TextureFormat::Invalid;

    // Add tilemode?
    bool IsTexture : 1 = false;
    bool IsFrameBuffer : 1 = false;
    bool IsWritable : 1 = false;
    bool IsPartiallyResident : 1 = false;
    bool IsNormalTexture : 1 = false;
    bool Padding : 2 = false;

    String DebugName;
};
} // namespace onyx::rhi
