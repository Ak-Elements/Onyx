#include <onyx/platform/windowsettings.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx
{
    bool Serialization<platform::WindowSettings>::serialize(Serializer& serializer, const platform::WindowSettings& settings)
    {
        serializer.write<"size">(settings.Size);
        serializer.write<"mode">(settings.Mode);
        return true;
    }

    bool Serialization<platform::WindowSettings>::deserialize(const Deserializer& deserializer, platform::WindowSettings& outSettings)
    {
        deserializer.read<"size">(outSettings.Size);
        deserializer.read<"mode">(outSettings.Mode);
        return true;
    }
}
