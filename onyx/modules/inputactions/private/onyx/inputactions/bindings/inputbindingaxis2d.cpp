#include <onyx/input/inputsystem.h>
#include <onyx/inputactions/bindings/inputbindingaxis2d.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::input_actions {
bool InputBindingAxis2D::DoUpdate( const input::InputSystem& inputSystem, Vector3f32& outInputValue ) {
    outInputValue.X = numericCast< float32 >( inputSystem.GetAxisValue1D( 0, m_AxisX ) );
    outInputValue.Y = numericCast< float32 >( inputSystem.GetAxisValue1D( 0, m_AxisY ) );
    return isZero( outInputValue.X ) == false || isZero( outInputValue.Y ) == false;
}

void InputBindingAxis2D::Reset() {
    m_AxisX = 0;
    m_AxisY = 0;
}

input::InputID InputBindingAxis2D::GetBoundInputForSlot( uint32_t index ) const {
    return index == 0 ? m_AxisX : m_AxisY;
}

void InputBindingAxis2D::SetInputBindingSlot( uint32_t index, input::InputID inputID ) {
    if ( index == 0 )
        m_AxisX = inputID;
    else
        m_AxisY = inputID;
}

StringView InputBindingAxis2D::GetInputBindingSlotName( uint32_t index ) const {
    return index == 0 ? "X" : "Y";
}
} // namespace onyx::input_actions

namespace onyx {
bool Serialization< input_actions::InputBindingAxis2D >::serialize( Serializer& serializer,
                                                                    const input_actions::InputBindingAxis2D& binding ) {
    return serializer.write< "axisX" >( binding.m_AxisX ) && serializer.write< "axisY" >( binding.m_AxisY );
}

bool Serialization< input_actions::InputBindingAxis2D >::deserialize( const Deserializer& deserializer,
                                                                      input_actions::InputBindingAxis2D& outBinding ) {
    return deserializer.read< "axisX" >( outBinding.m_AxisX ) && deserializer.read< "axisY" >( outBinding.m_AxisY );
}
} // namespace onyx
