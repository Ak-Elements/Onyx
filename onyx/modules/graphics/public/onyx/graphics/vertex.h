#pragma once

namespace Onyx::Graphics
{
    struct Vertex
    {
        Vector3f Position;
        Vector3f Normal;
        Vector2f UV;
    };

    struct FontVertex
    {
        Vector3f Position;
        Vector2f UV;
        onyxU32 Color;
    };
}
