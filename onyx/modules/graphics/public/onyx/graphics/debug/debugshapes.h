#pragma once

namespace onyx::graphics {
struct DebugSphere {
    Vector3f32 Position;
    float32 Radius = 1.0f;

    Color Color;
};

struct DebugBox {
    Vector3f32 Position;
    Color Color;

    Matrix3x3f32 Rotation;
    Vector3f32 HalfExtents;
};
} // namespace onyx::graphics
