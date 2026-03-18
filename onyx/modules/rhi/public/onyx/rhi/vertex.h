#pragma once

namespace onyx::rhi
{
    struct Vertex
    {
        onyxF32 PositionX;
        onyxF32 PositionY;
        onyxF32 PositionZ;
        onyxF32 U;
        onyxF32 NormalX;
        onyxF32 NormalY;
        onyxF32 NormalZ;
        onyxF32 V;
    };

    struct FontVertex
    {
        Vector3f32 Position;
        Vector2f32 UV;
        onyxU32 Color;
    };
}
