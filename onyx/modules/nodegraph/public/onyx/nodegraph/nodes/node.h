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
    void Prepare( PrepareContext& context ) const { OnPrepare( context ); }
    void Update( ExecutionContext& context ) const { OnUpdate( context ); }
    void Finish() { OnFinished(); }

    bool serialize( Serializer& serialize ) const;
    bool deserialize( const Deserializer& deserializer );

  private:
    virtual void OnPrepare( PrepareContext& /*context*/ ) const {}
    virtual void OnUpdate( ExecutionContext& /*context*/ ) const {}
    virtual void OnFinished() {}

    virtual bool serializePins( Serializer& serializer ) const;
    virtual bool deserializePins( const Deserializer& deserializer );

  public:
    // REMOVE
    const Guid64& GetId() const { return m_Id; }
    void SetId( Guid64 id ) { m_Id = id; }

    virtual StringId32 GetTypeId() const = 0;

    PinBase* GetPinById( Guid64 globalPinId );
    const PinBase* GetPinById( Guid64 globalPinId ) const;
    bool HasPin( Guid64 globalPinId ) const;

    // TODO: Improve this interface potentially?
    virtual uint32_t GetInputPinCount() const { return 0; }
    virtual uint32_t GetOutputPinCount() const { return 0; }

    virtual PinBase* GetInputPin( uint32_t /*index*/ ) {
        ONYX_ASSERT( false, "Not implemenented." );
        return nullptr;
    }
    virtual const PinBase* GetInputPin( uint32_t /*index*/ ) const {
        ONYX_ASSERT( false, "Not implemenented." );
        return nullptr;
    }
    virtual PinBase* GetOutputPin( uint32_t /*index*/ ) {
        ONYX_ASSERT( false, "Not implemenented." );
        return nullptr;
    }
    virtual const PinBase* GetOutputPin( uint32_t /*index*/ ) const {
        ONYX_ASSERT( false, "Not implemenented." );
        return nullptr;
    }

  protected:
    virtual bool OnSerialize( Serializer& /*serializer*/ ) const { return true; }
    virtual bool OnDeserialize( const Deserializer& /*deserializer*/ ) { return true; }

    template < typename PinT > // requires is_specialization_of_v<Pin, PinT>
    Optional< PinT* > GetInputPinByLocalId() {
        PinBase* inputPin = GetInputPinByLocalId( PinT::LocalId );
        if ( inputPin != nullptr ) {
            return static_cast< PinT* >( inputPin );
        }

        return {};
    }

    template < typename PinT > // requires is_specialization_of_v<Pin, PinT>
    Optional< const PinT* > GetInputPinByLocalId() const {
        const PinBase* inputPin = GetInputPinByLocalId( PinT::LocalId );
        if ( inputPin != nullptr ) {
            return static_cast< const PinT* >( inputPin );
        }

        return {};
    }

    template < typename PinT > // requires is_specialization_of_v<Pin, PinT>
    Optional< PinT* > GetOutputPinByLocalId() {
        PinBase* outputPin = GetOutputPinByLocalId( PinT::LocalId );
        if ( outputPin != nullptr ) {
            return static_cast< PinT& >( *outputPin );
        }

        return {};
    }

    template < typename PinT > // requires is_specialization_of_v<Pin, PinT>
    Optional< const PinT* > GetOutputPinByLocalId() const {
        const PinBase* outputPin = GetOutputPinByLocalId( PinT::LocalId );
        if ( outputPin != nullptr ) {
            return static_cast< const PinT* >( outputPin );
        }

        return {};
    }

    PinBase* GetInputPinByLocalId( StringId32 localPinId );
    const PinBase* GetInputPinByLocalId( StringId32 localPinId ) const;

    PinBase* GetOutputPinByLocalId( StringId32 localPinId );
    const PinBase* GetOutputPinByLocalId( StringId32 localPinId ) const;

  public:
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    StringView GetName() const { return m_DebugName; }
    void SetName( const String& name ) { m_DebugName = name; }
#endif

#if ONYX_IS_EDITOR
    bool DrawInPropertyGrid( HashMap< Guid64, std::any >& constantPinData ) {
        return OnDrawInPropertyGrid( constantPinData );
    }

    PinVisibility GetPinVisibility( StringId32 localPinId ) const { return DoGetPinVisibility( localPinId ); }

    void UIDrawNode() { OnUIDrawNode(); }
    void UIDrawNodeBackground() { OnUIDrawNodeBackground(); }

    virtual StringView GetPinName( StringId32 /*localPinId*/ ) const;
    virtual PinVisibility DoGetPinVisibility( StringId32 /*localPinId*/ ) const { return PinVisibility::Default; }

    virtual std::any CreateDefaultForPin( StringId32 pinId ) const = 0;

  protected:
    virtual bool OnDrawInPropertyGrid( HashMap< Guid64, std::any >& constantPinData ) {
        bool modified = false;
        const uint32_t inputPinCount = GetInputPinCount();
        for ( uint32_t i = 0; i < inputPinCount; ++i ) {
            PinBase* inputPin = GetInputPin( i );
            if ( inputPin->GetType() == PinTypeId::Execute )
                continue;

            if ( enums::none( GetPinVisibility( inputPin->GetLocalId() ), PinVisibility::InPropertyGrid ) )
                continue;

            if ( inputPin->IsConnected() )
                continue;

            const Guid64 globalId = inputPin->GetGlobalId();
            if ( constantPinData.contains( globalId ) == false )
                constantPinData[ globalId ] = CreateDefaultForPin( inputPin->GetLocalId() );

            inputPin->DrawPropertyPanel( GetPinName( inputPin->GetLocalId() ), constantPinData[ globalId ] );
        }

        return modified;
    }

    virtual void OnUIDrawNode() {}
    virtual void OnUIDrawNodeBackground() {}
#endif

  protected:
    Guid64 m_Id;

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    String m_DebugName;
#endif
};

template < typename... Policies >
class NodeWithPolicy : public Node, public Policies... {};
} // namespace onyx::node_graph
