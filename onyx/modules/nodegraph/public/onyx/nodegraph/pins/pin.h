#pragma once

#include <onyx/nodegraph/nodegraphtyperegistry.h>
#include <onyx/nodegraph/pins/pinbase.h>
#include <onyx/nodegraph/pins/pinmeta.hpp>

namespace onyx::node_graph {
template < typename DataT, CompileTimeString PinId >
class Pin : public PinBase {
  public:
    using DataType = DataT;
    static constexpr StringId32 LocalId{ PinId };
    static constexpr PinTypeId DataTypeId = static_cast< PinTypeId >( typeHash< DataT >() );

    Pin()
        : PinBase( Guid64Generator::getGuid() ) {
        if constexpr( HasTypeId< DataT > ) {
            NodeGraphTypeRegistry::registerType< DataT >();
        } else {
            PinMetaObject< DataT >::registerType();
        }
    }

    Pin( Guid64 globalPinId )
        : PinBase( globalPinId ) {
        if constexpr( HasTypeId< DataT > ) {
            NodeGraphTypeRegistry::registerType< DataT >();
        } else {
            PinMetaObject< DataT >::registerType();
        }
    }

    [[nodiscard]] std::any createDefault() const override { return DataT(); }

#if ONYX_IS_EDITOR
    void drawPropertyPanel( StringView name, std::any& anyValue ) const override;
    [[nodiscard]] constexpr uint32_t getTypeColor() const override { return PinMetaObject< DataT >::getPinTypeColor(); }
#endif

    [[nodiscard]] StringId32 getLocalId() const override { return LocalId; }
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    [[nodiscard]] StringView getLocalIdString() const override { return LocalId.getString(); }
#endif

    PinTypeId getType() override { return DataTypeId; }
    [[nodiscard]] PinTypeId getType() const override { return DataTypeId; }
};

#if ONYX_IS_EDITOR
template < typename DataT, CompileTimeString PinId >
void Pin< DataT, PinId >::drawPropertyPanel( StringView name, std::any& anyValue ) const {
    if constexpr( std::is_same_v< DataT, ExecutePin > == false ) {
        DataT& value = std::any_cast< DataT& >( anyValue );
        PinMetaObject< DataT >::drawPinInPropertyGrid( name, value );
    }
}
#endif
} // namespace onyx::node_graph
