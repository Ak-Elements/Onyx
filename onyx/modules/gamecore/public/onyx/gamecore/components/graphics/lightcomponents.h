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
