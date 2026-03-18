#pragma once 

#include <onyx/engine/enginesystem.h>
#include <onyx/platform/platformcontext.h>

#include <onyx/platform/window.h>

namespace onyx::input
{
    class InputSystem;
}

namespace onyx::platform
{
    class PlatformSystem : public IEngineSystem
    {
    public:
        using WindowCreatedSignalT = Signal<void(const Window&)>;
        using WindowDestroySignalT = Signal<void(const Window&)>;

        static constexpr StringId32 TypeId = "onyx::platform::PlatformSystem";
        StringId32 GetTypeId() const override { return TypeId; }

        PlatformSystem(WindowSettings windowSettings, input::InputSystem& inputSystem);

        PlatformContext& GetContext() { return m_Context; }
        const PlatformContext& GetContext() const { return m_Context; }

        input::InputSystem& GetInputSystem() { ONYX_ASSERT(m_InputSystem != nullptr); return *m_InputSystem; }

        void CreateNewWindow(WindowSettings settings);
        Window& GetMainWindow();
        const Window& GetMainWindow() const;
    
        const DynamicArray<UniquePtr<Window>>& GetWindows() const { return m_Windows; }

        template<auto Candidate, typename Type>
        void OnWindowCreate(Type* instance)
        {
            Sink sink(m_WindowCreatedSignal);
            sink.template Connect<Candidate>(instance);
        }

        template<auto Candidate, typename Type>
        void OnWindowDestroy(Type* instance)
        {
            Sink sink(m_WindowDestroySignal);
            sink.template Connect<Candidate>(instance);
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
        input::InputSystem* m_InputSystem = nullptr; 

        PlatformContext m_Context;
        DynamicArray<UniquePtr<Window>> m_Windows;

        WindowCreatedSignalT m_WindowCreatedSignal;
        WindowDestroySignalT m_WindowDestroySignal;
    };
}