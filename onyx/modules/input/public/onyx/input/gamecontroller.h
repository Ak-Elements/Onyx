#pragma once
#include <onyx/input/keycodes.h>
#include <onyx/input/inputid.h>

namespace Onyx::Input
{
    enum class ConnectionType
    {
        USB,
        Wireless
    };


    enum class GameControllerButton : onyxU16
    {
        First = Enums::ToIntegral(Key::Last),
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
        Misc1,           /* Additional button (e.g. Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button, Google Stadia capture button) */
        Right_Paddle_1,   /* Upper or primary paddle, under your right hand (e.g. Xbox Elite paddle P1) */
        Left_Paddle_1,    /* Upper or primary paddle, under your left hand (e.g. Xbox Elite paddle P3) */
        Right_Paddle_2,   /* Lower or secondary paddle, under your right hand (e.g. Xbox Elite paddle P2) */
        Left_Paddle_2,    /* Lower or secondary paddle, under your left hand (e.g. Xbox Elite paddle P4) */
        Touchpad,        /* PS4/PS5 touchpad button */
        Misc2,           /* Additional button */
        Misc3,           /* Additional button */
        Misc4,           /* Additional button */
        Misc5,           /* Additional button */
        Misc6,           /* Additional button */

        Last,
        Invalid = InputID::Invalid
    };

    static constexpr onyxU16 GameControllerButton_Count = Enums::ToIntegral(GameControllerButton::Last) - Enums::ToIntegral(GameControllerButton::First);

    enum class GameControllerAxis : onyxU16
    {
        First = Enums::ToIntegral(GameControllerButton::Last),

        LeftStick_X,
        LeftStick_Y,
        RightStick_X,
        RightStick_Y,
        LeftTrigger,
        RightTrigger,

        LeftStick_XY,
        RightStick_XY,

        Last,
        Invalid = InputID::Invalid
    };

    static constexpr onyxU16 GameControllerAxis_Count = Enums::ToIntegral(GameControllerAxis::Last) - Enums::ToIntegral(GameControllerAxis::First);

    struct GameController
    {
        onyxU32 Index = 0;
        InplaceArray<bool, static_cast<onyxU8>(GameControllerButton_Count)> ButtonStates { false };
    };
}