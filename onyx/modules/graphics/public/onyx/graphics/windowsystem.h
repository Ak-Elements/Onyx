#pragma once

#include <onyx/engine/enginesystem.h>

namespace Onyx::Graphics
{
    struct WindowSettings;
    class Window;

    class WindowSystem : public IEngineSystem
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::WindowSystem";
        StringId32 GetTypeId() const override { return TypeId; }

        WindowSystem(const WindowSettings& windowSettings);
        ~WindowSystem() override;

        Window& GetMainWindow() { return *m_MainWindow; }
        const Window& GetMainWindow() const { return *m_MainWindow; }

    private:
        UniquePtr<Window> m_MainWindow;
    };
}

template <>
struct Onyx::Serialization<Onyx::Graphics::WindowSettings>
{
    static bool Serialize(Serializer& serializer, const Graphics::WindowSettings& settings);
    static bool Deserialize(const Deserializer& deserializer, Graphics::WindowSettings& outSettings);
};
