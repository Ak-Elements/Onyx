#pragma once

namespace onyx::rhi
{
    struct DirectionalLight
    {
        Vector3f32 Direction;
        onyxF32 Intensity = 1.0f;

        Vector3f32 Color { 1.0f };
        onyxF32 ShadowAmount = 1.0f;

        bool IsShadowCasting = true;
        onyxF32 Padding[7];
    };

    struct PointLight
    {
        Vector3f32 Position;
        onyxF32 Intensity = 1.0f;

        Vector3f32 Color{ 1.0f };
        onyxF32 Radius;

        onyxU32 IsEnabled = 0;
        onyxU32 IsShadowCasting = 1;
        onyxF32 Padding[6];
    };

    struct SpotLight
    {
        Vector3f32 Position;
        onyxF32 Intensity = 1.0f;

        Vector3f32 Direction;
        onyxF32 AngleAttenuation = 1.0f;

        Vector3f32 Color{ 1.0f };
        onyxF32 Range;

        onyxF32 Angle;
        onyxF32 Falloff;
        bool IsShadowCasting = true;
        onyxF32 Padding;
    };


    struct LightBlock
    {
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

    struct LightClusterAABB
    {
        Vector4f32 Min;
        Vector4f32 Max;
    };

    struct ONYX_ALIGN(16) Lighting
    {
        Array<DirectionalLight, 64> DirectionalLights;
        Array<PointLight, 64> PointLights;
        Array<SpotLight, 64> SpotLights;

        onyxU32 DirectionalLightsCount = 0;
        onyxU32 PointLightsCount;
        onyxU32 SpotLightsCount;
        onyxU32 Padding;
    };
}