#pragma once

namespace onyx::graphics
{
    struct DebugSphere
    {
        Vector3f32 Position;
        onyxF32 Radius;

        onyxU32 Color;
    };

    struct DebugBox
    {
        Vector3f32 Position;
        onyxU32 Color;

        Matrix3x3f32 Rotation;
        Vector3f32 HalfExtents;
    };
}
