#include <onyx/inputactions/inputactionsystem.h>

#include <onyx/assets/assetsystem.h>

#include <onyx/input/inputsystem.h>
#include <onyx/inputactions/bindings/inputbinding.h>
#include <onyx/inputactions/inputaction.h>
#include <onyx/inputactions/inputactionsasset.h>
#include <onyx/inputactions/inputactionsmap.h>

#include <onyx/serialize/deserializer.h>

namespace onyx::input_actions {
InputActionSystem::InputActionSystem( const InputActionSystemSettings& settings,
                                      input::InputSystem& inputSystem,
                                      assets::AssetSystem& assetSystem )
    : m_InputSystem( &inputSystem ) {
    assets::AssetHandle< InputActionsContext > defaultInputActionsMap;
    assetSystem.getAsset( settings.InputActionId, defaultInputActionsMap );
    defaultInputActionsMap->getOnLoadedEvent().Connect< &InputActionSystem::SetActionsMapAsset >( this );
    if ( defaultInputActionsMap.isValid() && defaultInputActionsMap.isLoaded() ) {
        SetActionsMapAsset( defaultInputActionsMap );
    }
}

InputActionSystem::~InputActionSystem() = default;

void InputActionSystem::update() {
    if ( m_ContextId.isValid() == false )
        return;

    UpdateContext( m_InputActionsAsset->GetContext( m_ContextId ) );
}

void InputActionSystem::SetActionsMapAsset( assets::AssetHandle< InputActionsContext > inputAsset ) {
    inputAsset->getOnLoadedEvent().Disconnect( this );

    if ( m_InputActionsAsset != inputAsset ) {
        StringId32 newContextId = m_ContextId;
        m_InputActionsAsset = inputAsset;
        m_ContextId = 0;

        if ( m_InputActionsAsset.isValid() && ( m_InputActionsAsset->GetMaps().empty() == false ) ) {
            if ( ( newContextId.isValid() == false ) || ( m_InputActionsAsset->HasContext( newContextId ) == false ) ) {
                newContextId = m_InputActionsAsset->GetMaps().begin()->first;
            }

            SetCurrentInputActionMap( newContextId );
        } else {
            SetCurrentInputActionMap( 0 );
        }
    }
}

void InputActionSystem::SetCurrentInputActionMap( StringId32 id ) {
    if ( id != m_ContextId ) {
        m_CurrentActionStates.clear();

        // should we clear input signals not in the map anymore?

        m_ContextId = id;

        if ( id.isValid() && m_InputActionsAsset.isValid() ) {
            InitContext();
        }
    }
}

Optional< const InputActionState* > InputActionSystem::GetActionState( StringId64 actionId ) const {
    auto it = std::find_if( m_CurrentActionStates.begin(),
                            m_CurrentActionStates.end(),
                            [ & ]( const InputActionState& state ) { return state.ActionId == actionId; } );

    if ( it == m_CurrentActionStates.end() )
        return std::nullopt;

    const InputActionState& actionState = *it;
    return &actionState;
}

Optional< InputActionState* > InputActionSystem::GetActionState( StringId64 actionId ) {
    auto it = std::find_if( m_CurrentActionStates.begin(),
                            m_CurrentActionStates.end(),
                            [ & ]( const InputActionState& state ) { return state.ActionId == actionId; } );

    if ( it == m_CurrentActionStates.end() )
        return std::nullopt;

    InputActionState& actionState = *it;
    return &actionState;
}

bool InputActionSystem::IsActionTriggered( StringId64 actionId ) const {
    if ( const InputActionState* state = GetActionState( actionId ).value_or( nullptr ) ) {
        return isZero( state->Value ) == false;
    }

    return false;
}

void InputActionSystem::InitContext() {
    const InputActionsMap& context = m_InputActionsAsset->GetContext( m_ContextId );

    m_CurrentActionStates.reserve( context.GetActions().size() );
    for ( const InputAction& action : context.GetActions() ) {
        auto it = std::find_if( m_CurrentActionStates.begin(),
                                m_CurrentActionStates.end(),
                                [ & ]( const InputActionState& state ) { return state.ActionId == action.GetId(); } );

        if ( it != m_CurrentActionStates.end() )
            continue;

        m_CurrentActionStates.emplace_back( action.GetId() );
    }
}

void InputActionSystem::UpdateContext( InputActionsMap& context ) {
    DynamicArray< InputAction >& actions = context.GetActions();
    const uint32_t actionsCount = static_cast< uint32_t >( actions.size() );

    for ( uint32_t actionIndex = 0; actionIndex < actionsCount; ++actionIndex ) {
        InputAction& action = actions[ actionIndex ];
        StringId64 actionId = action.GetId();

        Optional< InputActionState* > optionalActionState = GetActionState( actionId );
        ONYX_ASSERT( optionalActionState.has_value() );

        InputActionState& actionState = *optionalActionState.value();

        DynamicArray< UniquePtr< InputBinding > >& bindings = action.GetBindings();
        const uint32_t bindingsCount = static_cast< uint32_t >( bindings.size() );

        bool hasTriggered = false;
        Vector3f32 newInputValue( std::numeric_limits< float32 >::lowest() );
        for ( uint32_t bindingIndex = 0; bindingIndex < bindingsCount; ++bindingIndex ) {
            InputBinding& binding = *bindings[ bindingIndex ];

            Vector3f32 bindingInputValue;
            bool isTriggered = binding.Update( *m_InputSystem, *this, bindingInputValue );

            if ( isTriggered ) {
                hasTriggered = true;
                newInputValue.X = std::max( newInputValue.X, bindingInputValue.X );
                newInputValue.Y = std::max( newInputValue.Y, bindingInputValue.Y );
                newInputValue.Z = std::max( newInputValue.Z, bindingInputValue.Z );
            }
        }

        if ( hasTriggered == false ) {
            newInputValue = Vector3f32::zero();
        }

        if ( actionState.Value != newInputValue ) {
            actionState.Value = newInputValue;
            InputActionEvent event{ actionId, actionState.Value };
            m_InputActionSignals[ actionId ].Dispatch( event );
        }
    }
}
} // namespace onyx::input_actions

namespace onyx {
bool Serialization< input_actions::InputActionSystemSettings >::serialize(
    Serializer& /*serializer*/,
    const input_actions::InputActionSystemSettings& /*system*/ ) {
    return true;
}

bool Serialization< input_actions::InputActionSystemSettings >::deserialize(
    const Deserializer& deserializer,
    input_actions::InputActionSystemSettings& outSettings ) {
    StringView inputMapPath;
    if ( deserializer.read< "inputmap" >( inputMapPath ) ) {
        outSettings.InputActionId = assets::AssetId( FilePath( inputMapPath ) );
    }

    return true;
}
} // namespace onyx
