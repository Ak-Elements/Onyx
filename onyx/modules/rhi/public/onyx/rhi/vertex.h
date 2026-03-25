#pragma once

namespace onyx::rhi {
struct Vertex {
    float32 PositionX;
    float32 PositionY;
    float32 PositionZ;
    float32 U;
    float32 NormalX;
    float32 NormalY;
    float32 NormalZ;
    float32 V;
};

struct FontVertex {
    Vector3f32 Position;
    Vector2f32 UV;
    uint32_t Color;
};
} // namespace onyx::rhi
