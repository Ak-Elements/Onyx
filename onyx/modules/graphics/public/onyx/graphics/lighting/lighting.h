#pragma once

namespace Onyx::Graphics
{
    struct DirectionalLight
    {
        Vector3f Direction;
        onyxF32 Intensity = 1.0f;

        Vector3f Color { 1.0f };
        onyxF32 ShadowAmount = 1.0f;

        bool IsShadowCasting = true;
        onyxF32 Padding[7];
    };

    struct PointLight
    {
        Vector3f Position;
        onyxF32 Intensity = 1.0f;

        Vector3f Color{ 1.0f };
        onyxF32 Radius;

        bool IsShadowCasting = true;
        onyxF32 Padding[7];
    };

    struct SpotLight
    {
        Vector3f Position;
        onyxF32 Intensity = 1.0f;

        Vector3f Direction;
        onyxF32 AngleAttenuation = 1.0f;

        Vector3f Color{ 1.0f };
        onyxF32 Range;

        onyxF32 Angle;
        onyxF32 Falloff;
        bool IsShadowCasting = true;
        onyxF32 Padding;
    };
}