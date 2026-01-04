#pragma once

#include <onyx/input/gamecontroller.h>
#include <onyx/input/keycodes.h>
#include <onyx/input/mouse.h>

namespace Onyx::Input
{
    enum class ButtonState : onyxU8
    {
        Down,
        Up,
        Repeat
    };

    struct KeyboardEvent
    {
        //Key
        onyxU16 Char;
        Key Key;
        ButtonState State;
    };

    struct MouseButtonEvent
    {
        MouseButton Button;
        ButtonState State;
    };

    struct MousePositionEvent
    {
        Vector2s32 Position;
    };

    struct MouseAxisEvent
    {
        onyxS16 Value;
    };

    struct GameControllerButtonEvent
    {
        onyxU32 ControllerIndex;
        GameControllerButton Button;
        ButtonState State;
    };

    struct GameControllerAxisEvent
    {
        onyxU32 ControllerIndex;
        GameControllerAxis Axis;
        onyxS16 Value;
    };
}