#pragma once

#include <onyx/nodegraph/pins/pinbase.h>
#include <onyx/nodegraph/pins/pinmeta.hpp>

namespace onyx::node_graph {
template < typename DataT >
class DynamicPin : public PinBase {
  public:
    using DataType = DataT;

    static UniquePtr< DynamicPin > Create( Guid64 globalId, StringId32 localId ) {
        return makeUnique< DynamicPin >( globalId, localId );
    }

    DynamicPin( StringId32 localId )
        : PinBase( Guid64Generator::getGuid() )
        , LocalId( localId ) {
        // TODO Pin type should be registered here
    }

    DynamicPin( Guid64 globalPinId, const StringId32 localId )
        : PinBase( globalPinId )
        , LocalId( localId ) {
        // TODO Pin type should be registered here
    }

    StringId32 LocalId = 0;
    static constexpr PinTypeId DataTypeId = static_cast< PinTypeId >( TypeHash< DataT >() );

    std::any CreateDefault() const override { return DataT(); }

#if ONYX_IS_EDITOR

    void DrawPropertyPanel( StringView name, std::any& anyValue ) const override;
    constexpr uint32_t GetTypeColor() const override { return PinMetaObject< DataT >::GetPinTypeColor(); }
#endif

    StringId32 GetLocalId() const override { return LocalId; }
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    StringView GetLocalIdString() const override { return LocalId.getString(); }
#endif

    PinTypeId GetType() override { return DataTypeId; }
    PinTypeId GetType() const override { return DataTypeId; }
};

#if ONYX_IS_EDITOR
template < typename DataT >
void DynamicPin< DataT >::DrawPropertyPanel( StringView name, std::any& anyValue ) const {
    if constexpr ( std::is_same_v< DataT, ExecutePin > == false ) {
        DataT& value = std::any_cast< DataT& >( anyValue );
        PinMetaObject< DataT >::DrawPinInPropertyGrid( name, value );
    }
}

#endif

UniquePtr< PinBase > CreatePin( StringId32 typeId, Guid64 globalId, StringId32 localId );
} // namespace onyx::node_graph
