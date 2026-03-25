#pragma once

namespace onyx::rhi {
struct DirectionalLight {
    Vector3f32 Direction;
    float32 Intensity = 1.0f;

    Vector3f32 Color{ 1.0f };
    float32 ShadowAmount = 1.0f;

    bool IsShadowCasting = true;
    float32 Padding[ 7 ];
};

struct PointLight {
    Vector3f32 Position;
    float32 Intensity = 1.0f;

    Vector3f32 Color{ 1.0f };
    float32 Radius;

    uint32_t IsEnabled = 0;
    uint32_t IsShadowCasting = 1;
    float32 Padding[ 6 ];
};

struct SpotLight {
    Vector3f32 Position;
    float32 Intensity = 1.0f;

    Vector3f32 Direction;
    float32 AngleAttenuation = 1.0f;

    Vector3f32 Color{ 1.0f };
    float32 Range;

    float32 Angle;
    float32 Falloff;
    bool IsShadowCasting = true;
    float32 Padding;
};

struct LightBlock {
    Vector3f32 color;
    float intensity;

    Vector3f32 position;
    float innerAngle;

    Vector3f32 direction;
    float outerAngle;

    int type;
    int numShadowSamples;
    float radius;
    float pad;
};

struct LightClusterAABB {
    Vector4f32 Min;
    Vector4f32 Max;
};

struct ONYX_ALIGN( 16 ) Lighting {
    Array< DirectionalLight, 64 > DirectionalLights;
    Array< PointLight, 64 > PointLights;
    Array< SpotLight, 64 > SpotLights;

    uint32_t DirectionalLightsCount = 0;
    uint32_t PointLightsCount;
    uint32_t SpotLightsCount;
    uint32_t Padding;
};
} // namespace onyx::rhi