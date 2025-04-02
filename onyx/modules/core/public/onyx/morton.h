#pragma once

#include <onyx/mortoncode2d.h>
#include <onyx/mortoncode3d.h>

namespace Onyx
{
    using MortonCode2D_U32 = MortonCode2D<onyxU32, onyxU16>;
    using MortonCode2D_U64 = MortonCode2D<onyxU64, onyxU32>;
    using MortonCode3D_U32 = MortonCode3D<onyxU32, onyxU16>;
    using MortonCode3D_U64 = MortonCode3D<onyxU64, onyxU32>;
}
