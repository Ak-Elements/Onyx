#include <onyx/inputactions/bindings/inputbindingaxis2dcomposite.h>

#include <onyx/input/inputsystem.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::input_actions {
void InputBindingAxis2DComposite::Reset() {
    m_InputUp = 0;
    m_InputDown = 0;
    m_InputLeft = 0;
    m_InputRight = 0;
}

bool InputBindingAxis2DComposite::DoUpdate( const input::InputSystem& inputSystem, Vector3f32& outInputValue ) {
    bool isUpDown = inputSystem.IsButtonDown( m_InputUp );
    bool isDownDown = inputSystem.IsButtonDown( m_InputDown );
    bool isLeftDown = inputSystem.IsButtonDown( m_InputLeft );
    bool isRightDown = inputSystem.IsButtonDown( m_InputRight );

    if ( ( isUpDown == false ) && ( isDownDown == false ) && ( isLeftDown == false ) && ( isRightDown == false ) )
        return false;

    outInputValue.X = ( isRightDown ? 1.0f : 0.0f ) - ( isLeftDown ? 1.0f : 0.0f );
    outInputValue.Y = ( isUpDown ? 1.0f : 0.0f ) - ( isDownDown ? 1.0f : 0.0f );
    return true;
}

input::InputID InputBindingAxis2DComposite::GetBoundInputForSlot( uint32_t index ) const {
    switch ( index ) {
    case 0:
        return m_InputUp;
    case 1:
        return m_InputDown;
    case 2:
        return m_InputLeft;
    case 3:
        return m_InputRight;
    default:
        ONYX_ASSERT( false );
        return input::InputID{};
    }
}

void InputBindingAxis2DComposite::SetInputBindingSlot( uint32_t index, input::InputID inputID ) {
    switch ( index ) {
    case 0:
        m_InputUp = inputID;
        break;
    case 1:
        m_InputDown = inputID;
        break;
    case 2:
        m_InputLeft = inputID;
        break;
    case 3:
        m_InputRight = inputID;
        break;
    default:
        ONYX_ASSERT( false );
        break;
    }
}

StringView InputBindingAxis2DComposite::GetInputBindingSlotName( uint32_t index ) const {
    switch ( index ) {
    case 0:
        return "Up";
    case 1:
        return "Down";
    case 2:
        return "Left";
    case 3:
        return "Right";
    default:
        return "";
    }
}
} // namespace onyx::input_actions

namespace onyx {
bool Serialization< input_actions::InputBindingAxis2DComposite >::serialize(
    Serializer& serializer,
    const input_actions::InputBindingAxis2DComposite& binding ) {
    return serializer.write< "up" >( binding.m_InputUp ) && serializer.write< "down" >( binding.m_InputDown ) &&
           serializer.write< "left" >( binding.m_InputLeft ) && serializer.write< "right" >( binding.m_InputRight );
}

bool Serialization< input_actions::InputBindingAxis2DComposite >::deserialize(
    const Deserializer& deserializer,
    input_actions::InputBindingAxis2DComposite& outBinding ) {
    return deserializer.read< "up" >( outBinding.m_InputUp ) && deserializer.read< "down" >( outBinding.m_InputDown ) &&
           deserializer.read< "left" >( outBinding.m_InputLeft ) &&
           deserializer.read< "right" >( outBinding.m_InputRight );
}
} // namespace onyx
