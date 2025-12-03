#include <onyx/graphics/windowsystem.h>

#include <onyx/graphics/window.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx::Graphics
{
   
    WindowSystem::WindowSystem(const WindowSettings& settings)
        : m_MainWindow(MakeUnique<Window>(settings))
    {
        m_MainWindow->Create();
        m_MainWindow->SetIcon("engine:/onyx128x128.png");
    }

    WindowSystem::~WindowSystem() = default;
}

namespace Onyx
{
    bool Serialization<Graphics::WindowSettings>::Serialize(Serializer& serializer, const Graphics::WindowSettings& settings)
    {
        serializer.Write<"size">(settings.Size);
        serializer.Write<"mode">(settings.Mode);
        return true;
    }

    bool Serialization<Graphics::WindowSettings>::Deserialize(const Deserializer& deserializer, Graphics::WindowSettings& outSettings)
    {
        deserializer.Read<"size">(outSettings.Size);
        deserializer.Read<"mode">(outSettings.Mode);
        return true;
    }
}