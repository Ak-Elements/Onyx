#pragma once

namespace onyx::rhi {
enum class SamplerFilter : uint8_t { Nearest, Linear, Cubic, Count };

enum class SamplerMipMapMode : uint8_t { Nearest, Linear, Count };

enum class SamplerAddressMode : uint8_t {
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
    MirrorClampToEdge,
    Count
};

enum class SamplerReductionMode : uint8_t { WeightedAverage, Min, Max, WeightedAverageClamp, Count };

struct SamplerProperties {
    uint32_t Hash() {
        uint32_t hash = 0;
        hash = hash::fnV1aHash( MinFilter, hash );
        hash = hash::fnV1aHash( MagFilter, hash );
        hash = hash::fnV1aHash( MipFilter, hash );
        hash = hash::fnV1aHash( AddressModeU, hash );
        hash = hash::fnV1aHash( AddressModeV, hash );
        hash = hash::fnV1aHash( AddressModeW, hash );
        hash = hash::fnV1aHash( Reduction, hash );
        return hash;
    }
    SamplerFilter MinFilter = SamplerFilter::Linear;
    SamplerFilter MagFilter = SamplerFilter::Linear;
    SamplerMipMapMode MipFilter = SamplerMipMapMode::Linear;

    SamplerAddressMode AddressModeU = SamplerAddressMode::Repeat;
    SamplerAddressMode AddressModeV = SamplerAddressMode::Repeat;
    SamplerAddressMode AddressModeW = SamplerAddressMode::Repeat;

    SamplerReductionMode Reduction = SamplerReductionMode::WeightedAverage;

    String Name = "";

}; // struct SamplerCreation

class Sampler : public RefCounted {};
} // namespace onyx::rhi