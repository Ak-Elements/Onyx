#pragma once
#include <onyx/graphics/lighting/lighting.h>

namespace Onyx
{
    namespace FileSystem
    {
        struct JsonValue;
    }

    class Stream;
}

namespace Onyx::GameCore
{
    struct DirectionalLightComponent
    {
        static constexpr StringId32 TypeId = "Onyx::GameCore::Components::DirectionalLightComponent";
        StringId32 GetTypeId() const { return TypeId; }

        Graphics::DirectionalLight Light;

        void Serialize(Stream& outStream) const;
        void Deserialize(const Stream& inStream);

        void SerializeJson(FileSystem::JsonValue& outStream) const;
        void DeserializeJson(const FileSystem::JsonValue& inStream);

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        // this is implemented in the editor module as we do not have ImGui linked in onyx_entity 
        void DrawImGuiEditor();
#endif
    };

    struct PointLightComponent
    {
        static constexpr StringId32 TypeId = "Onyx::GameCore::Components::PointLightComponent";
        StringId32 GetTypeId() const { return TypeId; }

        Graphics::PointLight Light;

        void Serialize(Stream& outStream) const;
        void Deserialize(const Stream& inStream);

        void SerializeJson(FileSystem::JsonValue& outStream) const;
        void DeserializeJson(const FileSystem::JsonValue& inStream);

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        // this is implemented in the editor module as we do not have ImGui linked in onyx_entity 
        void DrawImGuiEditor();
#endif
    };

    struct SpotLightComponent
    {
        static constexpr StringId32 TypeId = "Onyx::GameCore::Components::SpotLightComponent";
        StringId32 GetTypeId() const { return TypeId; }

        Graphics::SpotLight Light;

        void Serialize(Stream& outStream) const;
        void Deserialize(const Stream& inStream);

        void SerializeJson(FileSystem::JsonValue& outStream) const;
        void DeserializeJson(const FileSystem::JsonValue& inStream);

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        // this is implemented in the editor module as we do not have ImGui linked in onyx_entity 
        void DrawImGuiEditor();
#endif
    };
}
