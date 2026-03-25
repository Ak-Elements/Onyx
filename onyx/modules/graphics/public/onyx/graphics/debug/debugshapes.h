#pragma once

namespace onyx::graphics {
struct DebugSphere {
    Vector3f32 Position;
    float32 Radius;

    uint32_t Color;
};

struct DebugBox {
    Vector3f32 Position;
    uint32_t Color;

    Matrix3x3f32 Rotation;
    Vector3f32 HalfExtents;
};
} // namespace onyx::graphics
