#pragma once

#include <onyx/rhi/graphicstypes.h>
#include <onyx/rhi/sampler.h>

namespace onyx::rhi {
class Sampler;

struct TextureMipLocator {
    uint8_t m_MipLevel = 0;
    uint8_t m_MaxMipLevel = 0;
    uint16_t m_ArrayIndex = 0;
    uint16_t m_ArraySize = 0;

    bool operator==( const TextureMipLocator& anOther ) const {
        return m_MipLevel == anOther.m_MipLevel && m_ArrayIndex == anOther.m_ArrayIndex;
    }
    bool operator!=( const TextureMipLocator& anOther ) const { return !( *this == anOther ); }
};

struct TextureProperties : public TextureMipLocator {
    TextureProperties() = default;
    TextureProperties( TextureFormat format )
        : m_Format( format ) {}

    TextureUsage m_Usage = TextureUsage::Texture;
    TextureFormat m_Format = TextureFormat::Invalid;

    SamplerProperties m_Sampler;

    bool m_IsWriteable : 1 = false;
    bool m_AllowCubeMapLoads : 1 = false;
    bool m_Padding : 6 = false;

    String m_DebugName;
};
} // namespace onyx::rhi
