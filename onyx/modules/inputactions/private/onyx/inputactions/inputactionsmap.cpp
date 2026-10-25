#include <onyx/inputactions/inputactionsmap.h>

#include <onyx/filesystem/file.h>
#include <onyx/inputactions/inputaction.h>

namespace onyx::input_actions {
Optional< InputAction* > InputActionsMap::getAction( StringId64 actionId ) {
    for( InputAction& action : m_actions ) {
        if( action.GetId() == actionId )
            return &action;
    }

    return std::nullopt;
}

void InputActionsMap::removeAction( StringId64 actionId ) {
    std::erase_if( m_actions, [ & ]( const InputAction& action ) { return action.GetId() == actionId; } );
}

bool InputActionsMap::hasInputAction( StringId64 actionId ) const {
    for( const InputAction& action : m_actions ) {
        if( action.GetId() == actionId )
            return true;
    }

    return false;
}
} // namespace onyx::input_actions
