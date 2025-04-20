#pragma once

#include <onyx/engine/enginesystem.h>

#if USE_SDL2
#include <SDL_events.h>
#endif

#include <onyx/input/gamepad.h>
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
    class Window;
}

namespace Onyx::Input
{
    struct InputEvent;

    class InputSystem : public IEngineSystem
    {
        friend class Graphics::Window;
    public:
        InputSystem() = default;
        ~InputSystem() override = default;

        void Init(Graphics::Window& window);
        void Shutdown(Graphics::Window& window);
        void Update(onyxU64 deltaTime);

        bool IsButtonDown(MouseButton button) const { return m_MouseButtonStates[Enums::ToIntegral(button)]; }
        bool IsButtonDown(Key button) const { return m_KeyState[Enums::ToIntegral(button)]; }
        bool IsButtonDown(GameControllerButton /*button*/) const { return false; }

        const Vector2s16& GetMousePosition() const { return m_MousePosition; }
        void SetMousePosition(const Vector2s16& mousePos);

        Vector2s16 GetMouseDelta() const { return m_MouseDelta; }
        onyxS16 GetMouseWheelDelta() const { return m_MouseWheelDelta; }

        onyxS16 GetControllerAxisValue(onyxU32 controllerIndex, GameControllerAxis axis) const;

        ONYX_EVENT(OnInput, const InputEvent*)

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
        // Move mouse and keyboard states to a pc specific impl and gameInput to a pointer instead of a value object to decrease InputSystem size
        struct Gamepad
        {
            bool m_IsConnected = false;

            onyxU32 m_ButtonStates = 0;
            InplaceArray<onyxS16, Enums::ToIntegral(GameControllerAxis::Count)> m_AxisValues;
        };

        bool m_MouseButtonStates[Enums::ToIntegral(MouseButton::Count)] = { false };
        bool m_KeyState[static_cast<onyxS16>(Key::Count)] = { false };

        Vector2s16 m_MousePosition = { 0, 0 };
        Vector2s16 m_MouseDelta = { 0, 0 };
        Vector2s16 m_LastMousePosition = { 0, 0 };

        onyxS16 m_MouseScroll;
        onyxS16 m_LastMouseScroll;
        onyxS16 m_MouseWheelDelta = 0;

        DynamicArray<Gamepad> m_Gamepads;
    };
}
