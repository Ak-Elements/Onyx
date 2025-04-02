#include <onyx/gamecore/components/graphics/lightcomponents.h>
#include <onyx/filesystem/onyxfile.h>

namespace Onyx::GameCore
{
    /**
     * Directional light
     */
    void DirectionalLightComponent::Serialize(Stream& outStream) const
    {
        ONYX_UNUSED(outStream);
    }

    void DirectionalLightComponent::Deserialize(const Stream& inStream)
    {
        ONYX_UNUSED(inStream);
    }

    void DirectionalLightComponent::DeserializeJson(const FileSystem::JsonValue& inStream)
    {
        std::array<onyxF32, 3> radiance{ 0.0f };
        if (inStream.Get("color", radiance))
        {
            Light.Color[0] = radiance[0];
            Light.Color[1] = radiance[1];
            Light.Color[2] = radiance[2];
        }

        inStream.Get("Intensity", Light.Intensity);
        inStream.Get("IsShadowCasting", Light.IsShadowCasting);
    }

    /**
     * Point light
     */
    void PointLightComponent::Serialize(Stream& outStream) const
    {
        ONYX_UNUSED(outStream);
    }

    void DirectionalLightComponent::SerializeJson(FileSystem::JsonValue& outStream) const
    {
        std::array<onyxF32, 3> color{ Light.Color[0], Light.Color[1], Light.Color[2] };
        outStream.Set("color", color);

        outStream.Set("Intensity", Light.Intensity);
        outStream.Set("ShadowAmount", Light.ShadowAmount);
        outStream.Set("IsShadowCasting", Light.IsShadowCasting);
    }

    void PointLightComponent::Deserialize(const Stream& inStream)
    {
        ONYX_UNUSED(inStream);
    }

    void PointLightComponent::SerializeJson(FileSystem::JsonValue& outStream) const
    {


        std::array<onyxF32, 3> color{ Light.Color[0], Light.Color[1], Light.Color[2] };
        outStream.Set("color", color);

        outStream.Set("Intensity", Light.Intensity);
        outStream.Set("Radius", Light.Radius);
        outStream.Set("IsShadowCasting", Light.IsShadowCasting);
    }

    void PointLightComponent::DeserializeJson(const FileSystem::JsonValue& inStream)
    {
        std::array<onyxF32, 3> color{ 0.0f };
        if (inStream.Get("color", color))
        {
            Light.Color[0] = color[0];
            Light.Color[1] = color[1];
            Light.Color[2] = color[2];
        }

        inStream.Get("Intensity", Light.Intensity);
        inStream.Get("Radius", Light.Radius);
        inStream.Get("IsShadowCasting", Light.IsShadowCasting);
    }

    /**
     * Spot Light
     */
    void SpotLightComponent::Serialize(Stream& outStream) const
    {
        ONYX_UNUSED(outStream);
    }

    void SpotLightComponent::Deserialize(const Stream& inStream)
    {
        ONYX_UNUSED(inStream);
    }

    void SpotLightComponent::SerializeJson(FileSystem::JsonValue& outStream) const
    {

        std::array<onyxF32, 3> color{ Light.Color[0], Light.Color[1], Light.Color[2] };
        outStream.Set("color", color);

        outStream.Set("Intensity", Light.Intensity);
        outStream.Set("Falloff", Light.Falloff);
        outStream.Set("Range", Light.Range);
        outStream.Set("Angle", Light.Angle);
        outStream.Set("AngleAttenuation", Light.AngleAttenuation);
        outStream.Set("IsShadowCasting", Light.IsShadowCasting);
    }

    void SpotLightComponent::DeserializeJson(const FileSystem::JsonValue& inStream)
    {
        std::array<onyxF32, 3> color{ 0.0f };
        if (inStream.Get("color", color))
        {
            Light.Color[0] = color[0];
            Light.Color[1] = color[1];
            Light.Color[2] = color[2];
        }

        inStream.Get("Intensity", Light.Intensity);
        inStream.Get("Falloff", Light.Falloff);
        inStream.Get("Range", Light.Range);
        inStream.Get("Angle", Light.Angle);
        inStream.Get("AngleAttenuation", Light.AngleAttenuation);
        inStream.Get("IsShadowCasting", Light.IsShadowCasting);
    }
}
