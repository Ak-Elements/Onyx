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

struct DebugCapsule {
    Vector3f32 Position;
    Color Color;

    Matrix3x3f32 Rotation;
    float32 Radius = 1.0f;
    float32 HalfHeight = 1.0f;
};
} // namespace onyx::graphics
