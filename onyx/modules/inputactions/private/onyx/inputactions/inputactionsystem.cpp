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
    : m_inputSystem( &inputSystem ) {
    assets::AssetId defaultInputMap = settings.InputActionId;
    if( defaultInputMap.isValid() == false ) {
        defaultInputMap = assetSystem.resolveAssetId( "engine:/inputcontexts.oinput" );
    }

    assets::AssetHandle< InputActionsContext > defaultInputActionsMap;
    assetSystem.getAsset( defaultInputMap, defaultInputActionsMap );
    defaultInputActionsMap->getOnLoadedEvent().connect< &InputActionSystem::setActionsMapAsset >( this );
    if( defaultInputActionsMap.isValid() && defaultInputActionsMap.isLoaded() ) {
        setActionsMapAsset( defaultInputActionsMap );
    }
}

InputActionSystem::~InputActionSystem() = default;

void InputActionSystem::update() {
    if( m_contextId.isValid() == false )
        return;

    updateContext( m_inputActionsAsset->getContext( m_contextId ) );
}

void InputActionSystem::setActionsMapAsset( assets::AssetHandle< InputActionsContext > inputAsset ) {
    inputAsset->getOnLoadedEvent().disconnect( this );

    if( m_inputActionsAsset != inputAsset ) {
        StringId32 newContextId = m_contextId;
        m_inputActionsAsset = inputAsset;
        m_contextId = 0;

        if( m_inputActionsAsset.isValid() && ( m_inputActionsAsset->getMaps().empty() == false ) ) {
            if( ( newContextId.isValid() == false ) || ( m_inputActionsAsset->hasContext( newContextId ) == false ) ) {
                newContextId = m_inputActionsAsset->getMaps().begin()->first;
            }

            setCurrentInputActionMap( newContextId );
        } else {
            setCurrentInputActionMap( 0 );
        }
    }
}

void InputActionSystem::setCurrentInputActionMap( StringId32 id ) {
    if( id != m_contextId ) {
        m_currentActionStates.clear();

        // should we clear input signals not in the map anymore?

        m_contextId = id;

        if( id.isValid() && m_inputActionsAsset.isValid() ) {
            initContext();
        }
    }
}

Optional< const InputActionState* > InputActionSystem::getActionState( StringId64 actionId ) const {
    auto it = std::find_if( m_currentActionStates.begin(),
                            m_currentActionStates.end(),
                            [ & ]( const InputActionState& state ) { return state.ActionId == actionId; } );

    if( it == m_currentActionStates.end() )
        return std::nullopt;

    const InputActionState& actionState = *it;
    return &actionState;
}

Optional< InputActionState* > InputActionSystem::getActionState( StringId64 actionId ) {
    auto it = std::find_if( m_currentActionStates.begin(),
                            m_currentActionStates.end(),
                            [ & ]( const InputActionState& state ) { return state.ActionId == actionId; } );

    if( it == m_currentActionStates.end() )
        return std::nullopt;

    InputActionState& actionState = *it;
    return &actionState;
}

bool InputActionSystem::isActionTriggered( StringId64 actionId ) const {
    if( const InputActionState* state = getActionState( actionId ).value_or( nullptr ) ) {
        return isZero( state->Value ) == false;
    }

    return false;
}

void InputActionSystem::initContext() {
    const InputActionsMap& context = m_inputActionsAsset->getContext( m_contextId );

    m_currentActionStates.reserve( context.GetActions().size() );
    for( const InputAction& action : context.GetActions() ) {
        auto it = std::find_if( m_currentActionStates.begin(),
                                m_currentActionStates.end(),
                                [ & ]( const InputActionState& state ) { return state.ActionId == action.GetId(); } );

        if( it != m_currentActionStates.end() )
            continue;

        m_currentActionStates.emplace_back( action.GetId() );
    }
}

void InputActionSystem::updateContext( InputActionsMap& context ) {
    DynamicArray< InputAction >& actions = context.GetActions();
    const uint32_t actionsCount = static_cast< uint32_t >( actions.size() );

    for( uint32_t actionIndex = 0; actionIndex < actionsCount; ++actionIndex ) {
        InputAction& action = actions[ actionIndex ];
        StringId64 actionId = action.GetId();

        Optional< InputActionState* > optionalActionState = getActionState( actionId );
        ONYX_ASSERT( optionalActionState.has_value() );

        InputActionState& actionState = *optionalActionState.value();

        DynamicArray< UniquePtr< InputBinding > >& bindings = action.GetBindings();
        const uint32_t bindingsCount = static_cast< uint32_t >( bindings.size() );

        bool hasTriggered = false;
        Vector3f32 newInputValue( std::numeric_limits< float32 >::lowest() );
        for( uint32_t bindingIndex = 0; bindingIndex < bindingsCount; ++bindingIndex ) {
            InputBinding& binding = *bindings[ bindingIndex ];

            Vector3f32 bindingInputValue;
            bool isTriggered = binding.Update( *m_inputSystem, *this, bindingInputValue );

            if( isTriggered ) {
                hasTriggered = true;
                newInputValue.X = std::max( newInputValue.X, bindingInputValue.X );
                newInputValue.Y = std::max( newInputValue.Y, bindingInputValue.Y );
                newInputValue.Z = std::max( newInputValue.Z, bindingInputValue.Z );
            }
        }

        if( hasTriggered == false ) {
            newInputValue = Vector3f32::zero();
        }

        if( actionState.Value != newInputValue ) {
            actionState.Value = newInputValue;
            InputActionEvent event{ actionId, actionState.Value };
            m_inputActionSignals[ actionId ].dispatch( event );
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
    assets::AssetId inputMap;
    if( deserializer.read< "inputmap" >( inputMap ) ) {
        outSettings.InputActionId = inputMap;
    }

    return true;
}
} // namespace onyx
