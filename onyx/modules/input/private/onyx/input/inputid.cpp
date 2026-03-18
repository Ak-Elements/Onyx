#include <onyx/input/inputid.h>

#include <onyx/input/gamecontroller.h>
#include <onyx/input/mouse.h>
#include <onyx/input/keycodes.h>
#include <onyx/input/inputtypes.h>
#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::input
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
        onyxU16 first = enums::ToIntegral(MouseButton::First);
        onyxU16 last = enums::ToIntegral(MouseButton::Last);
        return (id.ID > first) && (id.ID < last);
    }

    bool IsMouseAxis1D(InputID id)
    {
        onyxU16 first = enums::ToIntegral(MouseAxis::First);
        onyxU16 last = enums::ToIntegral(MouseAxis::Last);
        return (id.ID > first) && (id.ID < last) && (IsMouseAxis2D(id) == false);
    }

    bool IsMouseAxis2D(InputID id)
    {
        onyxU16 xyAxis = enums::ToIntegral(MouseAxis::XY);
        onyxU16 deltaXYAxis = enums::ToIntegral(MouseAxis::DeltaXY);
        return (id.ID == xyAxis) || (id.ID == deltaXYAxis);
    }

    bool IsKeyboardKey(InputID id)
    {
        onyxU16 first = enums::ToIntegral(Key::First);
        onyxU16 last = enums::ToIntegral(Key::Last);
        return (id.ID > first) && (id.ID < last);
    }

    bool IsGameControllerButton(InputID id)
    {
        onyxU16 first = enums::ToIntegral(GameControllerButton::First);
        onyxU16 last = enums::ToIntegral(GameControllerButton::Last);
        return (id.ID > first) && (id.ID < last);
    }

    bool IsGameControllerAxis1D(InputID id)
    {
        onyxU16 first = enums::ToIntegral(GameControllerAxis::First);
        onyxU16 last = enums::ToIntegral(GameControllerAxis::Last);
        return (id.ID > first) && (id.ID < last) && (IsGameControllerAxis2D(id) == false);
    }

    bool IsGameControllerAxis2D(InputID id)
    {
        onyxU16 leftStickAxis = enums::ToIntegral(GameControllerAxis::LeftStick_XY);
        onyxU16 rightStickAxis = enums::ToIntegral(GameControllerAxis::RightStick_XY);
        return (id.ID == leftStickAxis) || (id.ID == rightStickAxis);
    }

    StringView ToString(InputID id)
    {
        if (IsMouseButton(id))
        {
            return enums::ToString<MouseButton>(id.ID);
        }
        if (IsMouseAxis1D(id) || IsMouseAxis1D(id))
        {
            return enums::ToString<MouseAxis>(id.ID);
        }
        if (IsKeyboardKey(id))
        {
            return enums::ToString<Key>(id.ID);
        }
        if (IsGameControllerButton(id))
        {
            return enums::ToString<GameControllerButton>(id.ID);
        }
        if (IsGameControllerAxis2D(id) || IsGameControllerAxis1D(id))
        {
            return enums::ToString<GameControllerAxis>(id.ID);
        }

        return "Unknown";
    }

    StringView GetInputTypeString(InputID id)
    {      
        return enums::ToString(GetInputType(id));
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

namespace onyx
{
    bool Serialization<input::InputID>::Serialize(Serializer& serializer, const input::InputID& id)
    {
        return serializer.Write(id.ID);
    }

    bool Serialization<input::InputID>::Deserialize(const Deserializer& deserializer, input::InputID& outId)
    {
        return deserializer.Read(outId.ID);
    }
}