#include <onyx/input/inputsystem.h>

#if ONYX_USE_SDL2
#include <imgui_impl_sdl.h>
#include <SDL_events.h>
#elif ONYX_IS_WINDOWS
#include <onyx/input/platform/windows/windows_keycodes.h>
#else
//static_assert(false, "Unsupported window library.");
#endif

#include <onyx/input/inputevent.h>
#include <onyx/graphics/window.h>
#include <onyx/log/logger.h>

namespace Onyx::Input
{
    void InputSystem::Init(Graphics::Window& window)
    {
#if ONYX_IS_WINDOWS && !ONYX_USE_SDL2
        window.SetWindowMessageHandler([this](onyxU32 messageType, onyxU64 wParam, onyxU64 lParam) { return HandleNativeInput(messageType, wParam, lParam); });
#endif

#if ONYX_USE_GAMEINPUT
        m_Input.SetControllerConnectedHandler({ this, &InputSystem::HandleGamepadConnected });
        m_Input.SetControllerDisconnectedHandler({ this, &InputSystem::HandleGamepadDisconnected });

        m_Input.SetControllerButtonHandler({ this, &InputSystem::HandleGameControllerButtonMessage });
        m_Input.SetControllerStickHandler({ this, &InputSystem::HandleGameControllerAxisMessage });
#endif

#if ONYX_USE_SDL2
        SDL_AddEventWatch([](void* userData, SDL_Event* event)
        {
            InputSystem* inputSys = static_cast<InputSystem*>(userData);

            //TODO: move this to a sdl class
            bool isQuit = event->type == SDL_QUIT;

            //TODO: Maybe return int?
            return inputSys->HandleInput(isQuit ? InputAction::Quit : InputAction::None) ? 0 : 1;
        }, this);
#endif
        
        //close the window when user clicks the X button or alt-f4s
    }

    void InputSystem::Shutdown(Graphics::Window& window)
    {
        window.ClearWindowMessageHandler();
    }

    void InputSystem::Update()
    {
        m_MouseDelta = m_MousePosition - m_LastMousePosition;
        m_LastMousePosition = m_MousePosition;

        m_MouseWheelDelta = m_MouseScroll - m_LastMouseScroll;
        m_LastMouseScroll = m_MouseScroll;

#if ONYX_USE_GAMEINPUT
        m_Input.Update();
#endif

#if ONYX_USE_SDL2
        SDL_Event e;
        //Handle events on queue
        while (SDL_PollEvent(&e))
        {
            //ONYX_LOG_DEBUG("event received");
            // move this away
            //ImGui_ImplSDL2_ProcessEvent(&e);

            //SDL_WaitEventTimeout(&e, 10);
        }
#endif
    }

    void InputSystem::SetMousePosition(const Vector2s16& mousePos)
    {
        if (m_MousePosition != mousePos)
        {
            m_MousePosition = mousePos;

            MouseEvent event;
            event.m_Id = InputEventId::MousePositionChanged;
            event.m_Position = mousePos;
            m_OnInput(&event);
        }
    }

    onyxS16 InputSystem::GetControllerAxisValue(onyxU32 controllerIndex, GameControllerAxis axis) const
    {
        ONYX_ASSERT(controllerIndex < m_Gamepads.size());
        const Gamepad& gamepad = m_Gamepads[controllerIndex];
        ONYX_ASSERT(gamepad.m_IsConnected);

        return gamepad.m_AxisValues[Enums::ToIntegral(axis)];
    }

#if ONYX_IS_WINDOWS && !ONYX_USE_SDL2
    bool InputSystem::HandleNativeInput(onyxU32 messageType, onyxU64 wParam, onyxU64 lParam)
    {
        // TODO: Move to a windows specific implementation for the handler to not pollute this class
        switch (messageType)
        {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            const bool hasReleased = (((lParam >> 16) & 0xFFFF) & KF_UP) == KF_UP;

            KeyboardEvent event;
            event.m_Id = (hasReleased ? InputEventId::KeyUp : (IsKeyRepeated(lParam) ? InputEventId::KeyRepeat : InputEventId::KeyDown));
            event.m_Key = ConvertWindowsKey(wParam, lParam);
            m_KeyState[static_cast<onyxS16>(event.m_Key)] = hasReleased == false;

            Modifier modifiers = Modifier::None;
            if (IsModifierKey(event.m_Key))
            {
                if (m_KeyState[Enums::ToIntegral(Key::Left_Ctrl)] || m_KeyState[Enums::ToIntegral(Key::Right_Ctrl)])
                    modifiers |= Modifier::Ctrl;

                if (m_KeyState[Enums::ToIntegral(Key::Left_Shift)] || m_KeyState[Enums::ToIntegral(Key::Right_Shift)])
                    modifiers |= Modifier::Shift;

                if (m_KeyState[Enums::ToIntegral(Key::Left_Alt)] || m_KeyState[Enums::ToIntegral(Key::Right_Alt)])
                    modifiers |= Modifier::Alt;
            }

            event.m_TriggerModifiers = modifiers;

            m_OnInput(&event);
            return true;
        }
        case WM_CHAR:
        case WM_SYSCHAR:
        {
            KeyboardEvent event;
            event.m_Id = InputEventId::KeyCharacter;
            event.m_Key = ConvertWindowsKey(wParam, lParam);
            event.m_Char = static_cast<onyxU16>(wParam);
            m_OnInput(&event);
            return true;
        }
        case WM_MOUSEMOVE:
        {
            const Vector2s16 mousePosition(static_cast<onyxS16>(lParam & 0xFFFF), static_cast<onyxS16>((lParam >> 16) & 0xFFFF));
            SetMousePosition(mousePosition); // event is sent in set mouse position
            return true;
        }
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        {
            MouseEvent event;
            if (messageType == WM_LBUTTONDOWN || messageType == WM_LBUTTONUP)
                event.m_Button = MouseButton::Left;
            else if (messageType == WM_RBUTTONDOWN || messageType == WM_RBUTTONUP)
                event.m_Button = MouseButton::Right;
            else if (messageType == WM_MBUTTONDOWN || messageType == WM_MBUTTONUP)
                event.m_Button = MouseButton::Middle;
            else
            {
                const onyxS32 extraMouseButton = (wParam >> 16) & 0xFFFF;
                event.m_Button = static_cast<MouseButton>(static_cast<onyxS32>(MouseButton::Button_3) + extraMouseButton);
            }


            const bool hasReleased = ((messageType == WM_LBUTTONUP) || (messageType == WM_RBUTTONUP) || (messageType == WM_MBUTTONUP) || (messageType == WM_XBUTTONUP));
            event.m_Id = hasReleased ? InputEventId::MouseButtonUp : InputEventId::MouseButtonDown;
            m_MouseButtonStates[Enums::ToIntegral(event.m_Button)] = hasReleased == false;
            m_OnInput(&event);

            return true;
        }
        
        case WM_MOUSEWHEEL:
        {
            MouseEvent event;
            event.m_Id = InputEventId::MouseWheel;
            
            event.m_Scroll = (onyxS16)HIWORD(wParam) / (onyxS16)WHEEL_DELTA;
            m_MouseScroll += event.m_Scroll;
            m_OnInput(&event);
            return true;
        }

        case WM_DEVICECHANGE:
        {
#if ONYX_USE_GAMEINPUT
            //if (wParam == DBT_DEVICEARRIVAL)
            {
                bool hasHandled = m_Input.OnDeviceChange();
                if (hasHandled)
                    return true;
            }
#endif
        }
        }

        return false;
    }

    void InputSystem::HandleGamepadConnected(onyxU32 deviceIndex)
    {
        ONYX_ASSERT(deviceIndex <= m_Gamepads.size());

        if (deviceIndex == m_Gamepads.size())
            m_Gamepads.emplace_back();

        m_Gamepads[deviceIndex].m_IsConnected = true;
    }

    void InputSystem::HandleGamepadDisconnected(onyxU32 deviceIndex)
    {
        ONYX_ASSERT(deviceIndex < m_Gamepads.size());
        m_Gamepads[deviceIndex].m_IsConnected = false;
    }

    void InputSystem::HandleGameControllerButtonMessage(onyxU32 controllerIndex, GameControllerButton button, bool isPressed)
    {
        ONYX_ASSERT(controllerIndex < m_Gamepads.size());
        Gamepad& gamepad = m_Gamepads[controllerIndex];
        ONYX_ASSERT(gamepad.m_IsConnected);

        onyxU32 buttonMask = 1 << Enums::ToIntegral(button);
        bool wasPressed = (gamepad.m_ButtonStates & buttonMask) != 0;

        if ((isPressed == false) && (wasPressed == false))
            return;

        if (isPressed)
            gamepad.m_ButtonStates |= buttonMask;
        else
            gamepad.m_ButtonStates &= ~buttonMask;

        InputEventId eventId = isPressed ? (wasPressed ? InputEventId::GameControllerButtonRepeat : InputEventId::GameControllerButtonDown) : InputEventId::GameControllerButtonUp;

        GameControllerButtonEvent event;
        event.m_Id = eventId;
        event.m_ControllerIndex = controllerIndex;
        event.m_Button = button;
        m_OnInput(&event);
    }

    void InputSystem::HandleGameControllerAxisMessage(onyxU32 controllerIndex, GameControllerAxis axis, onyxS16 axisValue)
    {
        ONYX_ASSERT(controllerIndex < m_Gamepads.size());
        Gamepad& gamepad = m_Gamepads[controllerIndex];
        ONYX_ASSERT(gamepad.m_IsConnected);

        if (axisValue == gamepad.m_AxisValues[Enums::ToIntegral(axis)])
            return;

        gamepad.m_AxisValues[Enums::ToIntegral(axis)] = axisValue;

        GameControllerAxisEvent event;
        event.m_Id = InputEventId::GameControllerAxis;
        event.m_ControllerIndex = controllerIndex;
        event.m_Axis = axis;
        event.m_Value = axisValue;
        m_OnInput(&event);
    }

#endif
}
