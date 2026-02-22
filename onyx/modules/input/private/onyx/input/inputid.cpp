#include <onyx/input/inputid.h>

#include <onyx/input/gamecontroller.h>
#include <onyx/input/mouse.h>
#include <onyx/input/keycodes.h>
#include <onyx/input/inputtypes.h>
#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx::Input
{
    bool IsAxis1D(InputID id)
    {
        return IsMouseAxis1D(id) || IsGameControllerAxis1D(id);
    }

    bool IsAxis2D(InputID id)
    {
        return IsMouseAxis2D(id) || IsGameControllerAxis2D(id);

    }

    bool IsMouseButton(InputID id)
    {
        onyxU16 first = Enums::ToIntegral(MouseButton::First);
        onyxU16 last = Enums::ToIntegral(MouseButton::Last);
        return (id.ID > first) && (id.ID < last);
    }

    bool IsMouseAxis1D(InputID id)
    {
        onyxU16 first = Enums::ToIntegral(MouseAxis::First);
        onyxU16 last = Enums::ToIntegral(MouseAxis::Last);
        return (id.ID > first) && (id.ID < last) && (IsMouseAxis2D(id) == false);
    }

    bool IsMouseAxis2D(InputID id)
    {
        onyxU16 xyAxis = Enums::ToIntegral(MouseAxis::XY);
        onyxU16 deltaXYAxis = Enums::ToIntegral(MouseAxis::DeltaXY);
        return (id.ID == xyAxis) || (id.ID == deltaXYAxis);
    }

    bool IsKeyboardKey(InputID id)
    {
        onyxU16 first = Enums::ToIntegral(Key::First);
        onyxU16 last = Enums::ToIntegral(Key::Last);
        return (id.ID > first) && (id.ID < last);
    }

    bool IsGameControllerButton(InputID id)
    {
        onyxU16 first = Enums::ToIntegral(GameControllerButton::First);
        onyxU16 last = Enums::ToIntegral(GameControllerButton::Last);
        return (id.ID > first) && (id.ID < last);
    }

    bool IsGameControllerAxis1D(InputID id)
    {
        onyxU16 first = Enums::ToIntegral(GameControllerAxis::First);
        onyxU16 last = Enums::ToIntegral(GameControllerAxis::Last);
        return (id.ID > first) && (id.ID < last) && (IsGameControllerAxis2D(id) == false);
    }

    bool IsGameControllerAxis2D(InputID id)
    {
        onyxU16 leftStickAxis = Enums::ToIntegral(GameControllerAxis::LeftStick_XY);
        onyxU16 rightStickAxis = Enums::ToIntegral(GameControllerAxis::RightStick_XY);
        return (id.ID == leftStickAxis) || (id.ID == rightStickAxis);
    }

    StringView ToString(InputID id)
    {
        if (IsMouseButton(id))
        {
            return Enums::ToString<MouseButton>(id.ID);
        }
        if (IsMouseAxis1D(id) || IsMouseAxis1D(id))
        {
            return Enums::ToString<MouseAxis>(id.ID);
        }
        if (IsKeyboardKey(id))
        {
            return Enums::ToString<Key>(id.ID);
        }
        if (IsGameControllerButton(id))
        {
            return Enums::ToString<GameControllerButton>(id.ID);
        }
        if (IsGameControllerAxis2D(id) || IsGameControllerAxis1D(id))
        {
            return Enums::ToString<GameControllerAxis>(id.ID);
        }

        return "Unknown";
    }

    StringView GetInputTypeString(InputID id)
    {      
        return Enums::ToString(GetInputType(id));
    }

    InputType GetInputType(InputID id)
    {
        if (IsMouseButton(id) || IsMouseAxis2D(id) || IsMouseAxis1D(id))
            return InputType::Mouse;
        if (IsKeyboardKey(id))
            return InputType::Keyboard;
        if (IsGameControllerButton(id) || IsGameControllerAxis2D(id) || IsGameControllerAxis1D(id))
            return InputType::Gamepad;

        ONYX_ASSERT("Unknown input");
        return InputType::Invalid;
    }
}

namespace Onyx
{
    bool Serialization<Input::InputID>::Serialize(Serializer& serializer, const Input::InputID& id)
    {
        return serializer.Write(id.ID);
    }

    bool Serialization<Input::InputID>::Deserialize(const Deserializer& deserializer, Input::InputID& outId)
    {
        return deserializer.Read(outId.ID);
    }
}