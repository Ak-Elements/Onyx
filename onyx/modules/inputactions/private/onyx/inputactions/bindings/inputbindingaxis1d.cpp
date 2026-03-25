#include <onyx/input/inputsystem.h>
#include <onyx/inputactions/bindings/inputbindingaxis1d.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::input_actions {
bool InputBindingAxis1D::DoUpdate( const input::InputSystem& inputSystem, Vector3f32& outInputValue ) {
    outInputValue.X = numericCast< float32 >( inputSystem.GetAxisValue1D( 0, m_Axis ) );
    return isZero( outInputValue.X ) == false;
}

void InputBindingAxis1D::SetInputBindingSlot( uint32_t, input::InputID inputID ) {
    m_Axis = inputID;
}
} // namespace onyx::input_actions

namespace onyx {
bool Serialization< input_actions::InputBindingAxis1D >::serialize( Serializer& serializer,
                                                                    const input_actions::InputBindingAxis1D& binding ) {
    return serializer.write< "axis" >( binding.m_Axis );
}

bool Serialization< input_actions::InputBindingAxis1D >::deserialize( const Deserializer& deserializer,
                                                                      input_actions::InputBindingAxis1D& outBinding ) {
    return deserializer.read< "axis" >( outBinding.m_Axis );
}
} // namespace onyx
