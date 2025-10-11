#pragma once

#include <onyx/input/gamecontroller.h>
#include <onyx/input/keycodes.h>
#include <onyx/input/mouse.h>

namespace Onyx::Input
{
    enum class InputEventType : onyxU8
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

        bool IsMouseEvent() const { return (m_Id == InputEventType::MouseButtonUp) || (m_Id == InputEventType::MouseButtonDown) || (m_Id == InputEventType::MousePositionChanged) || (m_Id == InputEventType::MouseWheel); }
        bool IsKeyboardEvent() const { return (m_Id == InputEventType::KeyUp) || (m_Id == InputEventType::KeyDown) || (m_Id == InputEventType::KeyRepeat) || (m_Id == InputEventType::KeyCharacter); }

        bool IsGamepadButtonEvent() const { return (m_Id == InputEventType::GameControllerButtonDown) || (m_Id == InputEventType::GameControllerButtonUp) || (m_Id == InputEventType::GameControllerButtonRepeat); }
        bool IsGamepadAxisEvent() const { return (m_Id == InputEventType::GameControllerAxis); }

        InputEventType m_Id = InputEventType::Invalid;
    };

    struct KeyboardEvent : public InputEvent
    {
        //Key
        onyxU16 m_Char = 0;
        Key m_Key;
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