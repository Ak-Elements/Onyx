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
        static constexpr StringId32 TypeId = "Onyx::Platform::PlatformSystem";
        StringId32 GetTypeId() const override { return TypeId; }

        PlatformSystem(Input::InputSystem& inputSystem);

        PlatformContext& GetContext() { return m_Context; }
        const PlatformContext& GetContext() const { return m_Context; }

        Input::InputSystem& GetInputSystem() { ONYX_ASSERT(m_InputSystem != nullptr); return *m_InputSystem; }

        void CreateNewWindow(WindowSettings settings);
        Window& GetMainWindow();
        const Window& GetMainWindow() const;
        
    private:
        Input::InputSystem* m_InputSystem = nullptr; 

        PlatformContext m_Context;
        DynamicArray<UniquePtr<Window>> m_Windows;
    };
}