#pragma once
#include <onyx/input/keycodes.h>

namespace onyx::input {
enum class ConnectionType { USB, Wireless };

enum class GameControllerButton : uint16_t {
    First = enums::toIntegral( Key::Last ),
    South,
    East,
    West,
    North,
    Back,
    Guide,
    Start,
    LeftStick,
    RightStick,
    LeftShoulder,
    RightShoulder,
    DPad_Up,
    DPad_Down,
    DPad_Left,
    DPad_Right,

    // Other potential buttons inspired by SDL
    Misc1, /* Additional button (e.g. Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture
              button, Amazon Luna microphone button, Google Stadia capture button) */
    Right_Paddle_1, /* Upper or primary paddle, under your right hand (e.g. Xbox Elite paddle P1) */
    Left_Paddle_1,  /* Upper or primary paddle, under your left hand (e.g. Xbox Elite paddle P3) */
    Right_Paddle_2, /* Lower or secondary paddle, under your right hand (e.g. Xbox Elite paddle P2) */
    Left_Paddle_2,  /* Lower or secondary paddle, under your left hand (e.g. Xbox Elite paddle P4) */
    Touchpad,       /* PS4/PS5 touchpad button */
    Misc2,          /* Additional button */
    Misc3,          /* Additional button */
    Misc4,          /* Additional button */
    Misc5,          /* Additional button */
    Misc6,          /* Additional button */

    Last,
    Invalid = 0
};

static constexpr uint16_t GameControllerButton_Count = enums::toIntegral( GameControllerButton::Last ) -
                                                       enums::toIntegral( GameControllerButton::First );

enum class GameControllerAxis : uint16_t {
    First = enums::toIntegral( GameControllerButton::Last ),

    LeftStick_X,
    LeftStick_Y,
    RightStick_X,
    RightStick_Y,
    LeftTrigger,
    RightTrigger,

    LeftStick_XY,
    RightStick_XY,

    Last,
    Invalid = 0
};

static constexpr uint16_t GameControllerAxis_Count = enums::toIntegral( GameControllerAxis::Last ) -
                                                     enums::toIntegral( GameControllerAxis::First );

struct GameController {
    int32_t m_AxisValues[ GameControllerAxis_Count ];
    uint32_t Index = 0;
    uint32_t ButtonStates = 0;
    bool IsConnected = false;
};

constexpr uint16_t ToIndex( GameControllerButton button ) {
    constexpr uint16_t first = enums::toIntegral( GameControllerButton::First );
#if ONYX_ASSERT_ENABLED
    constexpr uint16_t last = enums::ToIntegral( GameControllerButton::Last );
    ONYX_ASSERT( enums::ToIntegral( button ) > first );
    ONYX_ASSERT( enums::ToIntegral( button ) < last );
#endif
    return enums::toIntegral( button ) - first;
}

constexpr uint16_t ToIndex( GameControllerAxis axis ) {
    constexpr uint16_t first = enums::toIntegral( GameControllerButton::First );
#if ONYX_ASSERT_ENABLED
    constexpr uint16_t last = enums::ToIntegral( GameControllerButton::Last );
    ONYX_ASSERT( enums::ToIntegral( axis ) > first );
    ONYX_ASSERT( enums::ToIntegral( axis ) < last );
#endif
    return enums::toIntegral( axis ) - first;
}
} // namespace onyx::input