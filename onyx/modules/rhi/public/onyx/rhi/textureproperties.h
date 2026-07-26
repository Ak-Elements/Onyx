#pragma once

#include <onyx/rhi/graphicstypes.h>
#include <onyx/rhi/sampler.h>

namespace onyx::rhi {
class Sampler;

struct TextureMipLocator {
    uint8_t MipLevel = 0;
    uint8_t MaxMipLevel = 0;
    uint16_t ArrayIndex = 0;
    uint16_t ArraySize = 0;

    bool operator==( const TextureMipLocator& anOther ) const {
        return MipLevel == anOther.MipLevel && ArrayIndex == anOther.ArrayIndex;
    }
    bool operator!=( const TextureMipLocator& anOther ) const { return !( *this == anOther ); }
};

struct TextureProperties : public TextureMipLocator {
    TextureProperties() = default;
    TextureProperties( TextureFormat format )
        : Format( format ) {}

    TextureUsage Usage = TextureUsage::Texture;
    TextureFormat Format = TextureFormat::Invalid;

    Optional< SamplerProperties > Sampler;

    bool IsWriteable : 1 = false;
    bool AllowCubeMapLoads : 1 = false;
    bool Padding : 6 = false;

    String DebugName;
};
} // namespace onyx::rhi
