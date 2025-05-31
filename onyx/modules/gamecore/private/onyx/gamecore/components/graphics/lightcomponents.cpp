#include <onyx/gamecore/components/graphics/lightcomponents.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
    /**
     * Directional light
     */
    bool Serialization<GameCore::DirectionalLightComponent>::Serialize(Serializer& serializer, const GameCore::DirectionalLightComponent& directionalLight)
    {
        return serializer.Write<"color">(directionalLight.Light.Color) &&
            serializer.Write<"Intensity">(directionalLight.Light.Intensity) &&
            serializer.Write<"ShadowAmount">(directionalLight.Light.ShadowAmount) &&
            serializer.Write<"IsShadowCasting">(directionalLight.Light.IsShadowCasting);
    }

    bool Serialization<GameCore::DirectionalLightComponent>::Deserialize(const Deserializer& deserializer, GameCore::DirectionalLightComponent& outDirectionalLight)
    {
        return deserializer.Read<"color">(outDirectionalLight.Light.Color) &&
            deserializer.Read<"Intensity">(outDirectionalLight.Light.Intensity) &&
            deserializer.Read<"ShadowAmount">(outDirectionalLight.Light.ShadowAmount) &&
            deserializer.Read<"IsShadowCasting">(outDirectionalLight.Light.IsShadowCasting);
    }

    /**
     * Point light
     */
    bool Serialization<GameCore::PointLightComponent>::Serialize(Serializer& serializer, const GameCore::PointLightComponent& pointLight)
    {
        return serializer.Write<"color">(pointLight.Light.Color) &&
            serializer.Write<"Intensity">(pointLight.Light.Intensity) &&
            serializer.Write<"Radius">(pointLight.Light.Radius) &&
            serializer.Write<"IsShadowCasting">(pointLight.Light.IsShadowCasting);
    }

    bool Serialization<GameCore::PointLightComponent>::Deserialize(const Deserializer& deserializer, GameCore::PointLightComponent& outPointLight)
    {
        return deserializer.Read<"color">(outPointLight.Light.Color) &&
            deserializer.Read<"Intensity">(outPointLight.Light.Intensity) &&
            deserializer.Read<"Radius">(outPointLight.Light.Radius) &&
            deserializer.Read<"IsShadowCasting">(outPointLight.Light.IsShadowCasting);
    }

    /**
     * Spot Light
     */
    bool Serialization<GameCore::SpotLightComponent>::Serialize(Serializer& serializer, const GameCore::SpotLightComponent& spotLight)
    {
        return serializer.Write<"color">(spotLight.Light.Color) &&
            serializer.Write<"Intensity">(spotLight.Light.Intensity) &&
            serializer.Write<"Falloff">(spotLight.Light.Falloff) &&
            serializer.Write<"Range">(spotLight.Light.Range) &&
            serializer.Write<"Angle">(spotLight.Light.Angle) &&
            serializer.Write<"AngleAttenuation">(spotLight.Light.AngleAttenuation) &&
            serializer.Write<"IsShadowCasting">(spotLight.Light.IsShadowCasting);
    }

    bool Serialization<GameCore::SpotLightComponent>::Deserialize(const Deserializer& deserializer, GameCore::SpotLightComponent& outSpotLight)
    {
        return deserializer.Read<"color">(outSpotLight.Light.Color) &&
            deserializer.Read<"Intensity">(outSpotLight.Light.Intensity) &&
            deserializer.Read<"Falloff">(outSpotLight.Light.Falloff) &&
            deserializer.Read<"Range">(outSpotLight.Light.Range) &&
            deserializer.Read<"Angle">(outSpotLight.Light.Angle) &&
            deserializer.Read<"AngleAttenuation">(outSpotLight.Light.AngleAttenuation) &&
            deserializer.Read<"IsShadowCasting">(outSpotLight.Light.IsShadowCasting);
    }
}
