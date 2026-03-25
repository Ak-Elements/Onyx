#pragma once

#include <onyx/input/gamecontroller.h>
#include <onyx/input/keycodes.h>
#include <onyx/input/mouse.h>

namespace onyx::input {
enum class ButtonState : uint8_t { Down, Up, Repeat };

struct KeyboardEvent {
    // Key
    uint16_t Char;
    Key Key;
    ButtonState State;
};

struct MouseButtonEvent {
    MouseButton Button;
    ButtonState State;
};

struct MousePositionEvent {
    Vector2s32 Position;
};

struct MouseAxisEvent {
    int16_t Value;
};

struct GameControllerButtonEvent {
    uint32_t ControllerIndex;
    GameControllerButton Button;
    ButtonState State;
};

struct GameControllerAxisEvent {
    uint32_t ControllerIndex;
    GameControllerAxis Axis;
    int16_t Value;
};
} // namespace onyx::input