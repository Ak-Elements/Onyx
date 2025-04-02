#pragma once

namespace Onyx
{
    class Stream;

    namespace FileSystem
    {
        struct JsonValue;
    }
}

namespace Onyx::GameCore
{
    struct IdComponent
    {
        static constexpr bool HideInEditor = true;

        //TODO: change to UUID
        onyxU64 Id;

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
