#include <onyx/platform/windowsettings.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx
{
    bool Serialization<platform::WindowSettings>::Serialize(Serializer& serializer, const platform::WindowSettings& settings)
    {
        serializer.Write<"size">(settings.Size);
        serializer.Write<"mode">(settings.Mode);
        return true;
    }

    bool Serialization<platform::WindowSettings>::Deserialize(const Deserializer& deserializer, platform::WindowSettings& outSettings)
    {
        deserializer.Read<"size">(outSettings.Size);
        deserializer.Read<"mode">(outSettings.Mode);
        return true;
    }
}
