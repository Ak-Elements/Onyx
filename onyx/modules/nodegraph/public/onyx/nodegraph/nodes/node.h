#pragma once

#include <onyx/nodegraph/pins/pinbase.h>

namespace onyx::node_graph {
struct PrepareContext;
struct ExecutionContext;
class PinBase;

struct ExecutePolicy {
  public:
    void Prepare() { OnPrepare(); }
    void Update( ExecutionContext& context ) { OnUpdate( context ); }
    void Finish() { OnFinished(); }

  private:
    virtual void OnPrepare() {}
    virtual void OnUpdate( ExecutionContext& /*context*/ ) {}
    virtual void OnFinished() {}
};

#if ONYX_IS_EDITOR
enum class PinVisibility : uint8_t { None = 0, InNode = 1, InPropertyGrid = 2, Default = InNode | InPropertyGrid };
#endif

class Node {
  public:
    virtual ~Node() = default;

    // REMOVE TODO: Node should be data / pins only and the logic should be inferred by CRTP?
  public:
    void prepare( PrepareContext& context ) const { onPrepare( context ); }
    void update( ExecutionContext& context ) const { onUpdate( context ); }
    void finish() { onFinished(); }

    bool serialize( Serializer& serialize ) const;
    bool deserialize( const Deserializer& deserializer );

  private:
    virtual void onPrepare( PrepareContext& /*context*/ ) const {}
    virtual void onUpdate( ExecutionContext& /*context*/ ) const {}
    virtual void onFinished() {}

    virtual bool serializePins( Serializer& serializer ) const;
    virtual bool deserializePins( const Deserializer& deserializer );

  public:
    // REMOVE
    [[nodiscard]] const Guid64& getId() const { return m_id; }
    void setId( Guid64 id ) { m_id = id; }

    [[nodiscard]] virtual StringId32 getTypeId() const = 0;

    PinBase* getPinById( Guid64 globalPinId );
    [[nodiscard]] const PinBase* getPinById( Guid64 globalPinId ) const;
    [[nodiscard]] bool hasPin( Guid64 globalPinId ) const;

    // TODO: Improve this interface potentially?
    [[nodiscard]] virtual uint32_t getInputPinCount() const { return 0; }
    [[nodiscard]] virtual uint32_t getOutputPinCount() const { return 0; }

    virtual PinBase* getInputPin( uint32_t /*index*/ ) {
        ONYX_ASSERT( false, "Not implemenented." );
        return nullptr;
    }
    [[nodiscard]] virtual const PinBase* getInputPin( uint32_t /*index*/ ) const {
        ONYX_ASSERT( false, "Not implemenented." );
        return nullptr;
    }
    virtual PinBase* getOutputPin( uint32_t /*index*/ ) {
        ONYX_ASSERT( false, "Not implemenented." );
        return nullptr;
    }
    [[nodiscard]] virtual const PinBase* getOutputPin( uint32_t /*index*/ ) const {
        ONYX_ASSERT( false, "Not implemenented." );
        return nullptr;
    }

  protected:
    virtual bool onSerialize( Serializer& /*serializer*/ ) const { return true; }
    virtual bool onDeserialize( const Deserializer& /*deserializer*/ ) { return true; }

    template < typename PinT > // requires is_specialization_of_v<Pin, PinT>
    Optional< PinT* > getInputPinByLocalId() {
        PinBase* inputPin = getInputPinByLocalId( PinT::LocalId );
        if( inputPin != nullptr ) {
            return static_cast< PinT* >( inputPin );
        }

        return {};
    }

    template < typename PinT > // requires is_specialization_of_v<Pin, PinT>
    Optional< const PinT* > getInputPinByLocalId() const {
        const PinBase* inputPin = getInputPinByLocalId( PinT::LocalId );
        if( inputPin != nullptr ) {
            return static_cast< const PinT* >( inputPin );
        }

        return {};
    }

    template < typename PinT > // requires is_specialization_of_v<Pin, PinT>
    Optional< PinT* > getOutputPinByLocalId() {
        PinBase* outputPin = getOutputPinByLocalId( PinT::LocalId );
        if( outputPin != nullptr ) {
            return static_cast< PinT& >( *outputPin );
        }

        return {};
    }

    template < typename PinT > // requires is_specialization_of_v<Pin, PinT>
    Optional< const PinT* > getOutputPinByLocalId() const {
        const PinBase* outputPin = getOutputPinByLocalId( PinT::LocalId );
        if( outputPin != nullptr ) {
            return static_cast< const PinT* >( outputPin );
        }

        return {};
    }

    PinBase* getInputPinByLocalId( StringId32 localPinId );
    [[nodiscard]] const PinBase* getInputPinByLocalId( StringId32 localPinId ) const;

    PinBase* getOutputPinByLocalId( StringId32 localPinId );
    [[nodiscard]] const PinBase* getOutputPinByLocalId( StringId32 localPinId ) const;

  public:
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    [[nodiscard]] StringView getName() const { return m_debugName; }
    void setName( const String& name ) { m_debugName = name; }
#endif

#if ONYX_IS_EDITOR
    bool drawInPropertyGrid( HashMap< Guid64, std::any >& constantPinData ) {
        return onDrawInPropertyGrid( constantPinData );
    }

    [[nodiscard]] PinVisibility getPinVisibility( StringId32 localPinId ) const {
        return doGetPinVisibility( localPinId );
    }

    void uiDrawNode() { onUiDrawNode(); }
    void uiDrawNodeBackground() { onUiDrawNodeBackground(); }

    [[nodiscard]] virtual StringView getPinName( StringId32 /*localPinId*/ ) const;
    [[nodiscard]] virtual PinVisibility doGetPinVisibility( StringId32 /*localPinId*/ ) const {
        return PinVisibility::Default;
    }

    [[nodiscard]] virtual std::any createDefaultForPin( StringId32 pinId ) const = 0;

  protected:
    virtual bool onDrawInPropertyGrid( HashMap< Guid64, std::any >& constantPinData ) {
        bool modified = false;
        const uint32_t inputPinCount = getInputPinCount();
        for( uint32_t i = 0; i < inputPinCount; ++i ) {
            PinBase* inputPin = getInputPin( i );
            if( inputPin->getType() == PinTypeId::Execute )
                continue;

            if( enums::none( getPinVisibility( inputPin->getLocalId() ), PinVisibility::InPropertyGrid ) )
                continue;

            if( inputPin->isConnected() )
                continue;

            const Guid64 globalId = inputPin->getGlobalId();
            if( constantPinData.contains( globalId ) == false )
                constantPinData[ globalId ] = createDefaultForPin( inputPin->getLocalId() );

            inputPin->drawPropertyPanel( getPinName( inputPin->getLocalId() ), constantPinData[ globalId ] );
        }

        return modified;
    }

    virtual void onUiDrawNode() {}
    virtual void onUiDrawNodeBackground() {}
#endif

  protected:
    Guid64 m_id;

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    String m_debugName;
#endif
};

template < typename... Policies >
class NodeWithPolicy : public Node, public Policies... {};
} // namespace onyx::node_graph
