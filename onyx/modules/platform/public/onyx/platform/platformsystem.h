#pragma once 

#include <onyx/engine/enginesystem.h>
#include <onyx/platform/platformcontext.h>

#include <onyx/platform/window.h>

namespace Onyx::Input
{
    class InputSystem;
}

namespace Onyx::Platform
{
    class PlatformSystem : public IEngineSystem
    {
    public:
        using WindowCreatedSignalT = Signal<void(const Window&)>;
        using WindowDestroySignalT = Signal<void(const Window&)>;

        static constexpr StringId32 TypeId = "Onyx::Platform::PlatformSystem";
        StringId32 GetTypeId() const override { return TypeId; }

        PlatformSystem(WindowSettings windowSettings, Input::InputSystem& inputSystem);

        PlatformContext& GetContext() { return m_Context; }
        const PlatformContext& GetContext() const { return m_Context; }

        Input::InputSystem& GetInputSystem() { ONYX_ASSERT(m_InputSystem != nullptr); return *m_InputSystem; }

        void CreateNewWindow(WindowSettings settings);
        Window& GetMainWindow();
        const Window& GetMainWindow() const;
    
        const DynamicArray<UniquePtr<Window>>& GetWindows() const { return m_Windows; }

        template<auto Candidate, typename Type>
        void OnWindowCreate(Type* instance)
        {
            Sink sink(m_WindowCreatedSignal);
            sink.Connect<Candidate>(instance);
        }

        template<auto Candidate, typename Type>
        void OnWindowDestroy(Type* instance)
        {
            Sink sink(m_WindowDestroySignal);
            sink.Connect<Candidate>(instance);
        }

        template <typename Type>
        void DisconnectSignals(Type* instance)
        {
            Sink createdSignalSink(m_WindowCreatedSignal);
            Sink destroySignalSink(m_WindowDestroySignal);
            createdSignalSink.Disconnect(instance);
            destroySignalSink.Disconnect(instance);
        }

    private:
        Input::InputSystem* m_InputSystem = nullptr; 

        PlatformContext m_Context;
        DynamicArray<UniquePtr<Window>> m_Windows;

        WindowCreatedSignalT m_WindowCreatedSignal;
        WindowDestroySignalT m_WindowDestroySignal;
    };
}