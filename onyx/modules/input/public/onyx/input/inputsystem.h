#pragma once

#include <onyx/engine/enginesystem.h>

#if USE_SDL2
#include <SDL_events.h>
#endif

#include <onyx/input/gamecontroller.h>
#include <onyx/input/keycodes.h>
#include <onyx/input/mouse.h>

#include <onyx/eventhandler.h>

#if ONYX_IS_WINDOWS && !ONYX_USE_SDL2
#include <onyx/input/platform/windows/windows_gamecontroller.h>
#endif

namespace Onyx
{
    class Engine;
}

namespace Onyx::Graphics
{
    class WindowSystem;
    class Window;
}

namespace Onyx::Input
{
    struct InputEvent;

    class InputSystem : public IEngineSystem
    {
        friend class Graphics::Window;
    public:
        static constexpr StringId32 TypeId = "Onyx::Input::InputModule";
        StringId32 GetTypeId() const override { return TypeId; }

        InputSystem(Graphics::WindowSystem& windowSystem);
        ~InputSystem() override;

        void Update();

        onyxS16 GetAxisValue1D(onyxU32 deviceIndex, InputID id) const;
        Vector2s16 GetAxisValue2D(onyxU32 deviceIndex, InputID id) const;
        bool IsButtonDown(InputID id) const;
        bool IsButtonDown(MouseButton button) const;
        bool IsButtonDown(Key key) const;
        bool IsButtonDown(GameControllerButton button, onyxU8 deviceIndex) const;

        const Vector2s16& GetMousePosition() const { return m_MousePosition; }
        void SetMousePosition(const Vector2s16& mousePos);

        Vector2s16 GetMouseDelta() const { return m_MouseDelta; }
        onyxS16 GetMouseWheelDelta() const { return m_MouseWheelDelta; }

        onyxS16 GetControllerAxisValue(onyxU32 controllerIndex, GameControllerAxis axis) const;

        void EnableSystemMouseCapture(bool enable);

        ONYX_EVENT(OnInput, const InputEvent*);

#if ONYX_IS_WINDOWS && !ONYX_USE_SDL2
    private:
        bool HandleNativeInput(onyxU32 messageType, onyxU64 wParam, onyxU64 lParam);

        void HandleGamepadConnected(onyxU32 deviceIndex);
        void HandleGamepadDisconnected(onyxU32 deviceIndex);

        void HandleGameControllerButtonMessage(onyxU32 controllerIndex, GameControllerButton button, bool isPressed);
        void HandleGameControllerAxisMessage(onyxU32 controllerIndex, GameControllerAxis axis, onyxS16 axisValue);

#if ONYX_USE_GAMEINPUT
    private:
        //GameInput m_Input;
#endif
#endif

    private:
        Graphics::Window* m_MainWindow = nullptr;
        // Move mouse and keyboard states to a pc specific impl and gameInput to a pointer instead of a value object to decrease InputSystem size
        struct Gamepad
        {
            bool m_IsConnected = false;

            onyxU32 m_ButtonStates = 0;
            InplaceArray<onyxS16, GameControllerAxis_Count> m_AxisValues;
        };

        bool m_MouseButtonStates[MouseButton_Count] = { false };
        bool m_KeyState[Key_Count] = { false };

        Vector2s16 m_MousePosition = { 0, 0 };
        Vector2s16 m_MouseDelta = { 0, 0 };
        Vector2s16 m_LastMousePosition = { 0, 0 };

        onyxS16 m_MouseScroll;
        onyxS16 m_LastMouseScroll;
        onyxS16 m_MouseWheelDelta = 0;

        DynamicArray<Gamepad> m_Gamepads;
    };
}
