#pragma once

#include <onyx/mortoncode2d.h>
#include <onyx/mortoncode3d.h>

namespace onyx {
using MortonCode2dU32 = MortonCode2D< uint32_t, uint16_t >;
using MortonCode2dU64 = MortonCode2D< uint64_t, uint32_t >;
using MortonCode3dU32 = MortonCode3D< uint32_t, uint16_t >;
using MortonCode3dU64 = MortonCode3D< uint64_t, uint32_t >;
} // namespace onyx
