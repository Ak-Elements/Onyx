#include <onyx/inputactions/inputaction.h>

#include <onyx/inputactions/bindings/inputbindingsfactory.h>

namespace onyx::input_actions {
InputAction::InputAction() = default;

InputAction::InputAction( StringId64 actionId )
    : m_Id( actionId ) {}

InputAction::InputAction( const InputAction& other )
    : m_Id( other.m_Id )
    , m_Type( other.m_Type ) {
    for ( const UniquePtr< InputBinding >& binding : other.m_Bindings ) {
        m_Bindings.emplace_back( InputBindingsFactory::Copy( *binding ) );
    }
}

InputAction& InputAction::operator=( const InputAction& other ) {
    if ( this == &other ) {
        return *this;
    }

    m_Id = other.m_Id;
    m_Type = other.m_Type;

    m_Bindings.clear();
    m_Bindings.reserve( other.m_Bindings.size() );
    for ( const UniquePtr< InputBinding >& binding : other.m_Bindings ) {
        m_Bindings.emplace_back( InputBindingsFactory::Copy( *binding ) );
    }

    return *this;
}

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
void InputAction::RemoveBinding( uint32_t index ) {
    ONYX_ASSERT( index < m_Bindings.size(), "Binding index invalid." );
    std::erase( m_Bindings, m_Bindings[ index ] );
}

void InputAction::SetType( ActionType type ) {
    m_Bindings.clear();
    m_Type = type;
}
#endif
} // namespace onyx::input_actions
