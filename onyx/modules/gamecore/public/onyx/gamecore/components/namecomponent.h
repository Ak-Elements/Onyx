#pragma once
#include <onyx/filesystem/onyxfile.h>

#if !ONYX_IS_RETAIL || ONYX_IS_EDITOR

namespace Onyx::GameCore
{
    struct NameComponent
    {
        static constexpr bool HideInEditor = true;

        String Name;

        void Serialize(Stream& outStream) const;
        void Deserialize(const Stream& inStream);

        void SerializeJson(FileSystem::JsonValue& outStream) const;
        void DeserializeJson(const FileSystem::JsonValue& inStream);
    };
}
#endif