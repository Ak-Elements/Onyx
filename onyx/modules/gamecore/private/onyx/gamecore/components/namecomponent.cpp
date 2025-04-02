#include <onyx/gamecore/components/namecomponent.h>

#if !ONYX_IS_RETAIL || ONYX_IS_EDITOR

namespace Onyx::GameCore
{
    void NameComponent::Serialize(Stream& outStream) const
    {
        outStream.Write(Name);
    }

    void NameComponent::Deserialize(const Stream& inStream)
    {
        inStream.Read(Name);
    }

    void NameComponent::SerializeJson(FileSystem::JsonValue& outStream) const
    {
        outStream.Set("name", Name);
    }

    void NameComponent::DeserializeJson(const FileSystem::JsonValue& inStream)
    {
        inStream.Get("name", Name);
    }
}

#endif