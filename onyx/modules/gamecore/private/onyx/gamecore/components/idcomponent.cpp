#include <onyx/gamecore/components/idcomponent.h>
#include <onyx/filesystem/onyxfile.h>

namespace Onyx::GameCore
{
    void IdComponent::Serialize(Stream& outStream) const
    {
        ONYX_UNUSED(outStream);
    }

    void IdComponent::Deserialize(const Stream& inStream)
    {
        ONYX_UNUSED(inStream);
    }

    void IdComponent::SerializeJson(FileSystem::JsonValue& outStream) const
    {
        outStream.Set("id", Id);
    }
    
    void IdComponent::DeserializeJson(const FileSystem::JsonValue& inStream)
    {
        inStream.Get("id", Id);
    }
}
