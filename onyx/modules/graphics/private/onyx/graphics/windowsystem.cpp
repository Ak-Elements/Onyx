#include <onyx/graphics/windowsystem.h>

#include <onyx/graphics/window.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx::Graphics
{
   
    WindowSystem::WindowSystem()
        : m_MainWindow(MakeUnique<Window>())
    {
    }

    WindowSystem::~WindowSystem() = default;

    void WindowSystem::Init()
    {
        m_MainWindow->Create();
        m_MainWindow->SetIcon("engine:/onyx128x128.png");
        //m_MainWindow->Show();
    }
}

namespace Onyx
{
    template <>
    struct Serialization<Graphics::WindowSettings>
    {
        static bool Serialize(Serializer& serializer, const Graphics::WindowSettings& settings)
        {
            serializer.Write<"size">(settings.Size);
            serializer.Write<"mode">(settings.Mode);
            return true;

        }
        static bool Deserialize(const Deserializer& deserializer, Graphics::WindowSettings& outSettings)
        {
            deserializer.Read<"size">(outSettings.Size);
            deserializer.Read<"mode">(outSettings.Mode);
            return true;
        }
    };

    bool Serialization<Graphics::WindowSystem>::Serialize(Serializer& serializer, const Graphics::WindowSystem& system)
    {
        return serializer.Write<"window">(system.GetMainWindow().GetSettings());
    }

    bool Serialization<Graphics::WindowSystem>::Deserialize(const Deserializer& deserializer, Graphics::WindowSystem& outSystem)
    {
        Graphics::WindowSettings& windowSettings = outSystem.GetMainWindow().GetSettings();
        return deserializer.Read<"window">(windowSettings) &&
            deserializer.Read<"name">(windowSettings.Title);
    }
}