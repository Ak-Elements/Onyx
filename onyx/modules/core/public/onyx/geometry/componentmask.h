#pragma once

namespace onyx {
enum class VectorComponentMask : uint8_t {
    None = 0,
    X = 1 << 0,
    Y = 1 << 2,
    Z = 1 << 3,
    W = 1 << 4,
    XY = X | Y,
    XZ = X | Z,
    YZ = Y | Z,
    XYZ = X | Y | Z,
    All = X | Y | Z | W
};

} // namespace onyx
