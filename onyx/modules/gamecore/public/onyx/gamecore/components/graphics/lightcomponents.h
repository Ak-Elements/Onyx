#pragma once
#include <onyx/rhi/lighting/lighting.h>

namespace Onyx
{
    namespace FileSystem
    {
        struct JsonValue;
    }

    class Stream;
}

namespace Onyx
{

namespace GameCore
{
    struct DirectionalLightComponent
    {
        static constexpr StringId32 TypeId = "Onyx::GameCore::Components::DirectionalLightComponent";
        StringId32 GetTypeId() const { return TypeId; }

        Graphics::DirectionalLight Light;

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        // this is implemented in the editor module as we do not have ImGui linked in onyx_entity 
        bool DrawProperties(bool showHidden);
#endif
    };

    struct PointLightComponent
    {
        static constexpr StringId32 TypeId = "Onyx::GameCore::Components::PointLightComponent";
        StringId32 GetTypeId() const { return TypeId; }

        Graphics::PointLight Light { .IsEnabled =  true };

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        // this is implemented in the editor module as we do not have ImGui linked in onyx_entity 
        bool DrawProperties(bool showHidden);
#endif
    };

    struct SpotLightComponent
    {
        static constexpr StringId32 TypeId = "Onyx::GameCore::Components::SpotLightComponent";
        StringId32 GetTypeId() const { return TypeId; }

        Graphics::SpotLight Light;

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        // this is implemented in the editor module as we do not have ImGui linked in onyx_entity 
        bool DrawProperties(bool showHidden);
#endif
    };
}

template <>
struct Serialization<GameCore::DirectionalLightComponent>
{
    static bool Serialize(Serializer& serializer, const GameCore::DirectionalLightComponent& directionalLight);
    static bool Deserialize(const Deserializer& deserializer, GameCore::DirectionalLightComponent& outDirectionalLight);
};

template <>
struct Serialization<GameCore::PointLightComponent>
{
    static bool Serialize(Serializer& serializer, const GameCore::PointLightComponent& pointLight);
    static bool Deserialize(const Deserializer& deserializer, GameCore::PointLightComponent& outPointLight);
};

template <>
struct Serialization<GameCore::SpotLightComponent>
{
    static bool Serialize(Serializer& serializer, const GameCore::SpotLightComponent& spotLight);
    static bool Deserialize(const Deserializer& deserializer, GameCore::SpotLightComponent& outSpotLight);
};

}
