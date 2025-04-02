#pragma once

#include <onyx/input/gamepad.h>
#include <onyx/input/keycodes.h>
#include <onyx/input/mouse.h>

namespace Onyx::Input
{
    enum class InputEventId : onyxU8
    {
        Invalid,
        MouseButtonUp,
        MouseButtonDown,
        MousePositionChanged,
        MouseWheel,
        KeyUp,
        KeyDown,
        KeyRepeat,
        KeyCharacter,
        GameControllerButtonDown,
        GameControllerButtonUp,
        GameControllerButtonRepeat,
        GameControllerAxis
    };

    struct InputEvent
    {
        virtual ~InputEvent() = default;

        bool IsMouseEvent() const { return (m_Id == InputEventId::MouseButtonUp) || (m_Id == InputEventId::MouseButtonDown) || (m_Id == InputEventId::MousePositionChanged) || (m_Id == InputEventId::MouseWheel); }
        bool IsKeyboardEvent() const { return (m_Id == InputEventId::KeyUp) || (m_Id == InputEventId::KeyDown) || (m_Id == InputEventId::KeyRepeat) || (m_Id == InputEventId::KeyCharacter); }

        bool IsGamepadButtonEvent() const { return (m_Id == InputEventId::GameControllerButtonDown) || (m_Id == InputEventId::GameControllerButtonUp) || (m_Id == InputEventId::GameControllerButtonRepeat); }
        bool IsGamepadAxisEvent() const { return (m_Id == InputEventId::GameControllerAxis); }

        InputEventId m_Id = InputEventId::Invalid;
        Modifier m_TriggerModifiers = Modifier::None; // ctrl, alt, shift - is this needed on consoles?
    };

    struct KeyboardEvent : public InputEvent
    {
        Key m_Key = Key::Invalid;
        onyxU16 m_Char = 0;
    };

    struct MouseEvent : public InputEvent
    {
        // TODO split into seperate events?
        MouseButton m_Button;
        Vector2s16 m_Position;
        onyxS16 m_Scroll;
    };

    struct GameControllerButtonEvent : public InputEvent
    {
        onyxU32 m_ControllerIndex;
        GameControllerButton m_Button;
    };

    struct GameControllerAxisEvent : public InputEvent
    {
        onyxU32 m_ControllerIndex;
        GameControllerAxis m_Axis;
        onyxS16 m_Value;
    };
}