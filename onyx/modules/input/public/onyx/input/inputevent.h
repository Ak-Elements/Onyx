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

        bool IsMouseEvent() const { return IsMouseAxisEvent() || IsMouseButtonEvent() || IsMousePositionEvent(); }
        bool IsMouseAxisEvent() const { return (Id == InputEventType::MouseWheel); }
        bool IsMouseButtonEvent() const { return (Id == InputEventType::MouseButtonUp) || (Id == InputEventType::MouseButtonDown); }
        bool IsMousePositionEvent() const { return (Id == InputEventType::MousePositionChanged); }
        bool IsKeyboardEvent() const { return (Id == InputEventType::KeyUp) || (Id == InputEventType::KeyDown) || (Id == InputEventType::KeyRepeat) || (Id == InputEventType::KeyCharacter); }

        bool IsGamepadButtonEvent() const { return (Id == InputEventType::GameControllerButtonDown) || (Id == InputEventType::GameControllerButtonUp) || (Id == InputEventType::GameControllerButtonRepeat); }
        bool IsGamepadAxisEvent() const { return (Id == InputEventType::GameControllerAxis); }

        InputEventType Id = InputEventType::Invalid;
    };

    struct KeyboardEvent : public InputEvent
    {
        //Key
        onyxU16 Char;
        Key Key;
        bool State;
    };

    struct MouseButtonEvent : public InputEvent
    {
        // TODO split into seperate events?
        MouseButton Button;
        bool IsPressed;
    };

    struct MousePositionEvent : public InputEvent
    {
        Vector2s32 Position;
    };

    struct MouseAxisEvent : public InputEvent
    {
        onyxS16 Value;
    };

    struct GameControllerButtonEvent : public InputEvent
    {
        onyxU32 ControllerIndex;
        GameControllerButton Button;
        bool IsPressed;
    };

    struct GameControllerAxisEvent : public InputEvent
    {
        onyxU32 ControllerIndex;
        GameControllerAxis Axis;
        onyxS16 Value;
    };
}