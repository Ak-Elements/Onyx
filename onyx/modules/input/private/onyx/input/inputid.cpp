#include <onyx/input/inputid.h>

#include <onyx/input/gamecontroller.h>
#include <onyx/input/inputtypes.h>
#include <onyx/input/keycodes.h>
#include <onyx/input/mouse.h>
#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::input {
bool IsAxis1D( InputID id ) {
    return IsMouseAxis1D( id ) || IsGameControllerAxis1D( id );
}

bool IsAxis2D( InputID id ) {
    return IsMouseAxis2D( id ) || IsGameControllerAxis2D( id );
}

bool IsMouseButton( InputID id ) {
    uint16_t first = enums::toIntegral( MouseButton::First );
    uint16_t last = enums::toIntegral( MouseButton::Last );
    return ( id.ID > first ) && ( id.ID < last );
}

bool IsMouseAxis1D( InputID id ) {
    uint16_t first = enums::toIntegral( MouseAxis::First );
    uint16_t last = enums::toIntegral( MouseAxis::Last );
    return ( id.ID > first ) && ( id.ID < last ) && ( IsMouseAxis2D( id ) == false );
}

bool IsMouseAxis2D( InputID id ) {
    uint16_t xyAxis = enums::toIntegral( MouseAxis::XY );
    uint16_t deltaXYAxis = enums::toIntegral( MouseAxis::DeltaXY );
    return ( id.ID == xyAxis ) || ( id.ID == deltaXYAxis );
}

bool IsKeyboardKey( InputID id ) {
    uint16_t first = enums::toIntegral( Key::First );
    uint16_t last = enums::toIntegral( Key::Last );
    return ( id.ID > first ) && ( id.ID < last );
}

bool IsGameControllerButton( InputID id ) {
    uint16_t first = enums::toIntegral( GameControllerButton::First );
    uint16_t last = enums::toIntegral( GameControllerButton::Last );
    return ( id.ID > first ) && ( id.ID < last );
}

bool IsGameControllerAxis1D( InputID id ) {
    uint16_t first = enums::toIntegral( GameControllerAxis::First );
    uint16_t last = enums::toIntegral( GameControllerAxis::Last );
    return ( id.ID > first ) && ( id.ID < last ) && ( IsGameControllerAxis2D( id ) == false );
}

bool IsGameControllerAxis2D( InputID id ) {
    uint16_t leftStickAxis = enums::toIntegral( GameControllerAxis::LeftStick_XY );
    uint16_t rightStickAxis = enums::toIntegral( GameControllerAxis::RightStick_XY );
    return ( id.ID == leftStickAxis ) || ( id.ID == rightStickAxis );
}

StringView ToString( InputID id ) {
    if ( IsMouseButton( id ) ) {
        return enums::toString< MouseButton >( id.ID );
    }
    if ( IsMouseAxis1D( id ) || IsMouseAxis1D( id ) ) {
        return enums::toString< MouseAxis >( id.ID );
    }
    if ( IsKeyboardKey( id ) ) {
        return enums::toString< Key >( id.ID );
    }
    if ( IsGameControllerButton( id ) ) {
        return enums::toString< GameControllerButton >( id.ID );
    }
    if ( IsGameControllerAxis2D( id ) || IsGameControllerAxis1D( id ) ) {
        return enums::toString< GameControllerAxis >( id.ID );
    }

    return "Unknown";
}

StringView GetInputTypeString( InputID id ) {
    return enums::toString( GetInputType( id ) );
}

InputType GetInputType( InputID id ) {
    if ( IsMouseButton( id ) || IsMouseAxis2D( id ) || IsMouseAxis1D( id ) )
        return InputType::Mouse;
    if ( IsKeyboardKey( id ) )
        return InputType::Keyboard;
    if ( IsGameControllerButton( id ) || IsGameControllerAxis2D( id ) || IsGameControllerAxis1D( id ) )
        return InputType::Gamepad;

    ONYX_ASSERT( "Unknown input" );
    return InputType::Invalid;
}
} // namespace onyx::input

namespace onyx {
bool Serialization< input::InputID >::serialize( Serializer& serializer, const input::InputID& id ) {
    return serializer.write( id.ID );
}

bool Serialization< input::InputID >::deserialize( const Deserializer& deserializer, input::InputID& outId ) {
    return deserializer.read( outId.ID );
}
} // namespace onyx