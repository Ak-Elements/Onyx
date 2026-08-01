#pragma once

#include <onyx/assets/assethandle.h>
#include <onyx/engine/enginesystem.h>

#include <onyx/inputactions/inputactionsasset.h>
#include <onyx/inputactions/inputactionsmap.h>

namespace onyx::assets {
class AssetSystem;
}

namespace onyx::input {
class InputSystem;
}

namespace onyx::input_actions {
struct InputBindingContext;
struct InputActionsMap;
struct InputAction;

struct InputActionEvent {
    InputActionEvent( StringId64 id, Vector3f32 value )
        : m_actionId( id )
        , m_value( value ) {}

    template < typename T >
    T getData() const {
        if constexpr( std::is_same_v< bool, T > ) {
            return m_value[ 0 ] != 0.0f;
        } else if constexpr( std::is_same_v< float32, T > ) {
            return m_value[ 0 ];
        } else if constexpr( std::is_same_v< Vector2f32, T > ) {
            return Vector2f32( m_value );
        } else {
            return m_value;
        }
    }

    [[nodiscard]] StringId64 getId() const { return m_actionId; }

  private:
    StringId64 m_actionId;
    Vector3f32 m_value;
};

struct InputActionState {
    InputActionState( StringId64 actionId )
        : ActionId( actionId ) {}

    Vector3f32 Value;
    StringId64 ActionId;
};

struct InputActionSystemSettings {
    assets::AssetId InputActionId{ "engine:/inputcontexts.oinput" };
};

class InputActionSystem : public IEngineSystem {
    friend struct Serialization< InputActionSystem >;

    using InputActionSignalT = Signal< void( const InputActionEvent& ) >;

  public:
    static constexpr StringId32 TypeId = "onyx::input_actions::InputActionSystem";
    StringId32 getTypeId() const override { return TypeId; }

    InputActionSystem( const InputActionSystemSettings& settings,
                       input::InputSystem& inputSystem,
                       assets::AssetSystem& assetSystem );
    ~InputActionSystem() override;

    void update();

    void setActionsMapAsset( assets::AssetHandle< InputActionsContext > inputAsset );
    void setCurrentInputActionMap( StringId32 id );

    Optional< InputActionState* > getActionState( StringId64 actionId );
    Optional< const InputActionState* > getActionState( StringId64 actionId ) const;

    template < auto Candidate, typename... Type >
    void onInput( StringId64 actionId, Type&&... valueOrInstance ) {
        Sink sink( m_inputActionSignals[ actionId ] );
        sink.template connect< Candidate >( std::forward< Type... >( valueOrInstance )... );
    }

    template < typename... Type >
    void disconnect( Type&&... valueOrInstance ) {
        for( InputActionSignalT& inputSignal : m_inputActionSignals | std::views::values ) {
            Sink sink( inputSignal );
            sink.disconnect( std::forward< Type... >( valueOrInstance )... );
        }
    }

    bool isActionTriggered( StringId64 actionId ) const;

  private:
    void initContext();
    void updateContext( InputActionsMap& context );

  private:
    input::InputSystem* m_inputSystem = nullptr;

    assets::AssetHandle< InputActionsContext > m_inputActionsAsset;
    DynamicArray< InputActionState > m_currentActionStates;
    HashMap< StringId64, InputActionSignalT > m_inputActionSignals;

    StringId32 m_contextId = 0;
};
} // namespace onyx::input_actions

namespace onyx {
template <>
struct Serialization< input_actions::InputActionSystemSettings > {
    static bool serialize( Serializer& serializer, const input_actions::InputActionSystemSettings& settings );
    static bool deserialize( const Deserializer& deserializer, input_actions::InputActionSystemSettings& outSettings );
};
} // namespace onyx
