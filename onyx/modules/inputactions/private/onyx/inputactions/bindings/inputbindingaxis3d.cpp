#include <onyx/input/inputsystem.h>
#include <onyx/inputactions/bindings/inputbindingaxis3d.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::input_actions {
bool InputBindingAxis3D::DoUpdate( const input::InputSystem& inputSystem, Vector3f32& outInputValue ) {
    outInputValue.X = numericCast< float32 >( inputSystem.GetAxisValue1D( 0, m_AxisX ) );
    outInputValue.X = numericCast< float32 >( inputSystem.GetAxisValue1D( 0, m_AxisY ) );
    outInputValue.Z = numericCast< float32 >( inputSystem.GetAxisValue1D( 0, m_AxisZ ) );
    return isZero( outInputValue.X ) == false || isZero( outInputValue.Y ) || isZero( outInputValue.Z );
}

void InputBindingAxis3D::Reset() {
    m_AxisX = 0;
    m_AxisY = 0;
    m_AxisZ = 0;
}

input::InputID InputBindingAxis3D::GetBoundInputForSlot( uint32_t index ) const {
    switch ( index ) {
    case 0:
        return m_AxisX;
    case 1:
        return m_AxisY;
    case 2:
        return m_AxisZ;
    default:
        ONYX_ASSERT( false );
        return input::InputID{};
    }
}

void InputBindingAxis3D::SetInputBindingSlot( uint32_t index, input::InputID inputID ) {
    switch ( index ) {
    case 0:
        m_AxisX = inputID;
        break;
    case 1:
        m_AxisY = inputID;
        break;
    case 2:
        m_AxisZ = inputID;
        break;
    default:
        ONYX_ASSERT( false );
        return;
    }
}

StringView InputBindingAxis3D::GetInputBindingSlotName( uint32_t index ) const {
    switch ( index ) {
    case 0:
        return "X";
    case 1:
        return "Y";
    case 2:
        return "Z";
    default:
        return "";
    }
}
} // namespace onyx::input_actions

namespace onyx {
bool Serialization< input_actions::InputBindingAxis3D >::serialize( Serializer& serializer,
                                                                    const input_actions::InputBindingAxis3D& binding ) {
    return serializer.write< "axisX" >( binding.m_AxisX ) && serializer.write< "axisY" >( binding.m_AxisY ) &&
           serializer.write< "axisZ" >( binding.m_AxisZ );
}

bool Serialization< input_actions::InputBindingAxis3D >::deserialize( const Deserializer& deserializer,
                                                                      input_actions::InputBindingAxis3D& outBinding ) {
    return deserializer.read< "axisX" >( outBinding.m_AxisX ) && deserializer.read< "axisY" >( outBinding.m_AxisY ) &&
           deserializer.read< "axisZ" >( outBinding.m_AxisZ );
}
} // namespace onyx
