#pragma once

namespace Onyx::Graphics
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
        Vector3f32 HalfExtents;

        onyxU32 Color;
    };
}
