#pragma once

namespace Onyx::Graphics
{
    struct Vertex
    {
        Vector3f32 Position;
        Vector3f32 Normal;
        Vector2f32 UV;
    };

    struct FontVertex
    {
        Vector3f32 Position;
        Vector2f32 UV;
        onyxU32 Color;
    };
}
