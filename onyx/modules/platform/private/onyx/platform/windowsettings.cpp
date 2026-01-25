#include <onyx/platform/windowsettings.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx
{
    bool Serialization<Platform::WindowSettings>::Serialize(Serializer& serializer, const Platform::WindowSettings& settings)
    {
        serializer.Write<"size">(settings.Size);
        serializer.Write<"mode">(settings.Mode);
        return true;
    }

    bool Serialization<Platform::WindowSettings>::Deserialize(const Deserializer& deserializer, Platform::WindowSettings& outSettings)
    {
        deserializer.Read<"size">(outSettings.Size);
        deserializer.Read<"mode">(outSettings.Mode);
        return true;
    }
}
