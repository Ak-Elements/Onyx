#pragma once

namespace onyx::rhi
{
    enum class SamplerFilter : onyxU8
    {
        Nearest,
        Linear,
        Cubic,
        Count
    };

    enum class SamplerMipMapMode : onyxU8
    {
        Nearest,
        Linear,
        Count
    };

    enum class SamplerAddressMode : onyxU8
    {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,
        MirrorClampToEdge,
        Count
    };

    enum class SamplerReductionMode : onyxU8
    {
        WeightedAverage,
        Min,
        Max,
        WeightedAverageClamp,
        Count
    };

    struct SamplerProperties
    {
        onyxU32 Hash()
        {
            onyxU32 hash = 0;
            hash = hash::FNV1aHash(MinFilter, hash);
            hash = hash::FNV1aHash(MagFilter, hash);
            hash = hash::FNV1aHash(MipFilter, hash);
            hash = hash::FNV1aHash(AddressModeU, hash);
            hash = hash::FNV1aHash(AddressModeV, hash);
            hash = hash::FNV1aHash(AddressModeW, hash);
            hash = hash::FNV1aHash(Reduction, hash);
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

    class Sampler : public RefCounted
    {
    };
}