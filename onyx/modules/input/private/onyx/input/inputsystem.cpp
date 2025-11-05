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
#include <onyx/graphics/windowsystem.h>
#include <onyx/input/inputid.h>
#include <onyx/log/logger.h>

namespace Onyx::Input
{
    void InputSystem::Init(Graphics::WindowSystem& windowSystem)
    {
        m_MainWindow = &windowSystem.GetMainWindow();

#if ONYX_IS_WINDOWS && !ONYX_USE_SDL2
        m_MainWindow->SetWindowMessageHandler([this](onyxU32 messageType, onyxU64 wParam, onyxU64 lParam) { return HandleNativeInput(messageType, wParam, lParam); });
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
        
    }

    InputSystem::~InputSystem()
    {
        m_MainWindow->ClearWindowMessageHandler();
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

    onyxS16 InputSystem::GetAxisValue1D(onyxU32 deviceIndex, InputID id) const
    {
        switch (id.ID)
        {
            case Enums::ToIntegral(MouseAxis::X):
                return m_MousePosition.X;
            case Enums::ToIntegral(MouseAxis::Y):
                return m_MousePosition.Y;
            case Enums::ToIntegral(MouseAxis::DeltaX):
                return m_MouseDelta.X;
            case Enums::ToIntegral(MouseAxis::DeltaY):
                return m_MouseDelta.Y;
            case Enums::ToIntegral(MouseAxis::Wheel):
                return m_MouseWheelDelta;
            case Enums::ToIntegral(GameControllerAxis::LeftStick_X):
            case Enums::ToIntegral(GameControllerAxis::LeftStick_Y):
            case Enums::ToIntegral(GameControllerAxis::RightStick_X):
            case Enums::ToIntegral(GameControllerAxis::RightStick_Y):
            case Enums::ToIntegral(GameControllerAxis::LeftTrigger):
            case Enums::ToIntegral(GameControllerAxis::RightTrigger):
                return GetControllerAxisValue(deviceIndex, static_cast<GameControllerAxis>(id.ID));
            default:
                ONYX_ASSERT(false, "Invalid axis input id");
                return 0;
        }
    }

    Vector2s16 InputSystem::GetAxisValue2D(onyxU32 deviceIndex, InputID id) const
    {
        switch (id.ID)
        {
            case Enums::ToIntegral(MouseAxis::XY):
                return m_MousePosition;
            case Enums::ToIntegral(MouseAxis::DeltaXY):
                return m_MouseDelta;
            case Enums::ToIntegral(GameControllerAxis::LeftStick_XY):
                return { GetControllerAxisValue(deviceIndex, GameControllerAxis::LeftStick_X), GetControllerAxisValue(deviceIndex, GameControllerAxis::LeftStick_Y) };
            case Enums::ToIntegral(GameControllerAxis::RightStick_XY):
                return { GetControllerAxisValue(deviceIndex, GameControllerAxis::RightStick_X), GetControllerAxisValue(deviceIndex, GameControllerAxis::RightStick_Y) };
            default:
                ONYX_ASSERT(false, "Invalid axis 2D input id");
                return Vector2s16::Zero();
        }
    }

    bool InputSystem::IsButtonDown(InputID id) const
    {
        if (IsMouseButton(id))
        {
            return IsButtonDown(static_cast<MouseButton>(id.ID));
        }
        if (IsKeyboardKey(id))
        {
            return IsButtonDown(static_cast<Key>(id.ID));
        }
        if (IsGameControllerButton(id))
        {
            return IsButtonDown(static_cast<GameControllerButton>(id.ID), 0);
        }

        return false;
    }

    bool InputSystem::IsButtonDown(MouseButton button) const
    {
        constexpr onyxU16 first = Enums::ToIntegral(MouseAxis::First);
        constexpr onyxU16 last = Enums::ToIntegral(MouseAxis::Last);
        ONYX_ASSERT(Enums::ToIntegral(button) > first);
        ONYX_ASSERT(Enums::ToIntegral(button) < last);

        onyxU16 index = Enums::ToIntegral(button) - first;
        return m_MouseButtonStates[index];
    }

    bool InputSystem::IsButtonDown(Key key) const
    {
        constexpr onyxU16 first = Enums::ToIntegral(Key::First);
        constexpr onyxU16 last = Enums::ToIntegral(Key::Last);
        ONYX_ASSERT(Enums::ToIntegral(key) > first);
        ONYX_ASSERT(Enums::ToIntegral(key) < last);

        onyxU16 index = Enums::ToIntegral(key) - first;
        return m_KeyState[index];
    }

    bool InputSystem::IsButtonDown(GameControllerButton button, onyxU8 deviceIndex) const
    {
        if (deviceIndex >= m_Gamepads.size())
            return false;

        constexpr onyxU16 first = Enums::ToIntegral(GameControllerButton::First);
        constexpr onyxU16 last = Enums::ToIntegral(GameControllerButton::Last);
        ONYX_ASSERT(Enums::ToIntegral(button) > first);
        ONYX_ASSERT(Enums::ToIntegral(button) < last);

        onyxU16 bitmaskIndex = Enums::ToIntegral(button) - first;
        return m_Gamepads[deviceIndex].m_ButtonStates & (1 << bitmaskIndex);
    }

    void InputSystem::SetMousePosition(const Vector2s16& mousePos)
    {
        if (m_MousePosition != mousePos)
        {
            m_MousePosition = mousePos;

            MouseEvent event;
            event.m_Id = InputEventType::MousePositionChanged;
            event.m_Position = mousePos;
            m_OnInput(&event);
        }
    }

    onyxS16 InputSystem::GetControllerAxisValue(onyxU32 controllerIndex, GameControllerAxis axis) const
    {
        //TODO: 0.5.0 fix this for controllers
        if (controllerIndex >= m_Gamepads.size())
            return 0;

        ONYX_ASSERT(controllerIndex < m_Gamepads.size());
        const Gamepad& gamepad = m_Gamepads[controllerIndex];
        ONYX_ASSERT(gamepad.m_IsConnected);

        constexpr onyxU16 offset = Enums::ToIntegral(GameControllerButton::First);
        onyxU8 index = static_cast<onyxU8>(Enums::ToIntegral(axis) - offset);
        return gamepad.m_AxisValues[index];
    }

    void InputSystem::EnableSystemMouseCapture(bool enable)
    {
        m_MainWindow->EnableSystemMouseCapture(enable);
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
                event.m_Id = (hasReleased ? InputEventType::KeyUp : (IsKeyRepeated(lParam) ? InputEventType::KeyRepeat : InputEventType::KeyDown));
                event.m_Key = ConvertWindowsKey(wParam, lParam);

                constexpr onyxU32 offset = Enums::ToIntegral(Key::First);
                onyxU16 index = Enums::ToIntegral(event.m_Key) - offset;
                m_KeyState[index] = hasReleased == false;

                /*Modifier modifiers = Modifier::None;
                if (IsModifierKey(event.m_Key))
                {
                    if (m_KeyState[Enums::ToIntegral(Key::Left_Ctrl)] || m_KeyState[Enums::ToIntegral(Key::Right_Ctrl)])
                        modifiers |= Modifier::Ctrl;

                    if (m_KeyState[Enums::ToIntegral(Key::Left_Shift)] || m_KeyState[Enums::ToIntegral(Key::Right_Shift)])
                        modifiers |= Modifier::Shift;

                    if (m_KeyState[Enums::ToIntegral(Key::Left_Alt)] || m_KeyState[Enums::ToIntegral(Key::Right_Alt)])
                        modifiers |= Modifier::Alt;
                }

                event.m_TriggerModifiers = modifiers;*/

                m_OnInput(&event);
                return true;
            }
            case WM_CHAR:
            case WM_SYSCHAR:
            {
                KeyboardEvent event;
                event.m_Id = InputEventType::KeyCharacter;
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
            case Graphics::Window::ONYX_WM_SYSTEM_PRIMARY_MOUSEDOWN:
            case Graphics::Window::ONYX_WM_SYSTEM_PRIMARY_MOUSEUP:
            {
                MouseEvent event;
                if (messageType == WM_LBUTTONDOWN || messageType == WM_LBUTTONUP || messageType == Graphics::Window::ONYX_WM_SYSTEM_PRIMARY_MOUSEDOWN || messageType == Graphics::Window::ONYX_WM_SYSTEM_PRIMARY_MOUSEUP)
                    event.m_Button = MouseButton::Button_1;
                else if (messageType == WM_RBUTTONDOWN || messageType == WM_RBUTTONUP)
                    event.m_Button = MouseButton::Button_2;
                else if (messageType == WM_MBUTTONDOWN || messageType == WM_MBUTTONUP)
                    event.m_Button = MouseButton::Button_3;
                else
                {
                    const onyxS32 extraMouseButton = (wParam >> 16) & 0xFFFF;
                    event.m_Button = static_cast<MouseButton>(static_cast<onyxS32>(MouseButton::Button_3) + extraMouseButton);
                }


                const bool hasReleased = ((messageType == WM_LBUTTONUP) ||
                    (messageType == WM_RBUTTONUP) ||
                    (messageType == WM_MBUTTONUP) ||
                    (messageType == WM_XBUTTONUP) ||
                    (messageType == Graphics::Window::ONYX_WM_SYSTEM_PRIMARY_MOUSEUP));

                event.m_Id = hasReleased ? InputEventType::MouseButtonUp : InputEventType::MouseButtonDown;
                constexpr onyxU32 offset = Enums::ToIntegral(MouseButton::First);
                m_MouseButtonStates[Enums::ToIntegral(event.m_Button) - offset] = hasReleased == false;
                m_OnInput(&event);

                return true;
            }
            
            case WM_MOUSEWHEEL:
            {
                MouseEvent event;
                event.m_Id = InputEventType::MouseWheel;
                
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
#else
                break;
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

        constexpr onyxU16 offset = Enums::ToIntegral(Key::Last) + 1;
        onyxU8 index = static_cast<onyxU8>(Enums::ToIntegral(button) - offset);
        onyxU32 buttonMask = 1 << index;
        bool wasPressed = (gamepad.m_ButtonStates & buttonMask) != 0;

        if ((isPressed == false) && (wasPressed == false))
            return;

        if (isPressed)
            gamepad.m_ButtonStates |= buttonMask;
        else
            gamepad.m_ButtonStates &= ~buttonMask;

        InputEventType eventId = isPressed ? (wasPressed ? InputEventType::GameControllerButtonRepeat : InputEventType::GameControllerButtonDown) : InputEventType::GameControllerButtonUp;

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

        constexpr onyxU16 offset = Enums::ToIntegral(GameControllerButton::Last) + 1;
        onyxU8 index = static_cast<onyxU8>(Enums::ToIntegral(axis) - offset);
        if (axisValue == gamepad.m_AxisValues[index])
            return;

        gamepad.m_AxisValues[index] = axisValue;

        GameControllerAxisEvent event;
        event.m_Id = InputEventType::GameControllerAxis;
        event.m_ControllerIndex = controllerIndex;
        event.m_Axis = axis;
        event.m_Value = axisValue;
        m_OnInput(&event);
    }

#endif
}
