#pragma once

#include <onyx/nodegraph/nodegraphtypemeta.h>

namespace onyx::node_graph {
class NodeGraphTypeRegistry {
  public:
    template < typename T > requires HasTypeId< T >
    static void registerType() {
        constexpr StringId32 SerializedTypeId = T::TypeId;
        constexpr PinTypeId RuntimePinTypeId = static_cast< PinTypeId >( typeHash< T >() );

        bool hasRegisteredType = s_typeMeta.contains( SerializedTypeId );
#if ONYX_ASSERT_ENABLED
        auto it = s_runtimeToStaticTypeId.find( RuntimePinTypeId );

        bool hasFoundTypeId = it != s_runtimeToStaticTypeId.end();
        bool isMatchingTypeId = ( hasFoundTypeId == false ) || ( it->second == SerializedTypeId );
        ONYX_ASSERT( ( hasRegisteredType == hasFoundTypeId ),
                     "Type({}) was not registered with the same PinTypeId({})",
                     SerializedTypeId,
                     enums::toIntegral( RuntimePinTypeId ) );
        ONYX_ASSERT( ( hasRegisteredType == hasFoundTypeId ) && isMatchingTypeId,
                     "Type({}) was registered but a similar type overlaps the PinTypeId({}, typeId:{})",
                     SerializedTypeId,
                     enums::toIntegral( RuntimePinTypeId ),
                     it->second );
#endif

        if( hasRegisteredType ) {
            return;
        }

        s_typeMeta[ SerializedTypeId ] = makeUnique< NodeGraphTypeMeta< T > >( SerializedTypeId );
        s_runtimeToStaticTypeId[ RuntimePinTypeId ] = SerializedTypeId;
    }

    template < typename T, CompileTimeString TypeId >
    static void registerType() {
        constexpr StringId32 SerializedTypeId = TypeId;
        constexpr PinTypeId RuntimePinTypeId = static_cast< PinTypeId >( typeHash< T >() );

        bool hasRegisteredType = s_typeMeta.contains( SerializedTypeId );
#if ONYX_ASSERT_ENABLED
        auto it = s_runtimeToStaticTypeId.find( RuntimePinTypeId );

        bool hasFoundTypeId = it != s_runtimeToStaticTypeId.end();
        bool isMatchingTypeId = ( hasFoundTypeId == false ) || ( it->second == SerializedTypeId );
        ONYX_ASSERT( ( hasRegisteredType == hasFoundTypeId ),
                     "Type({}) was not registered with the same PinTypeId({})",
                     SerializedTypeId,
                     enums::toIntegral( RuntimePinTypeId ) );
        ONYX_ASSERT( ( hasRegisteredType == hasFoundTypeId ) && isMatchingTypeId,
                     "Type({}) was registered but a similar type overlaps the PinTypeId({}, typeId:{})",
                     SerializedTypeId,
                     enums::toIntegral( RuntimePinTypeId ),
                     it->second );
#endif

        if( hasRegisteredType ) {
            return;
        }

        s_typeMeta[ SerializedTypeId ] = makeUnique< NodeGraphTypeMeta< T > >( SerializedTypeId );
        s_runtimeToStaticTypeId[ RuntimePinTypeId ] = SerializedTypeId;
    }

    static StringId32 getSerializedTypeId( PinTypeId pinTypeId ) { return s_runtimeToStaticTypeId.at( pinTypeId ); }

    static const INodeGraphTypeMeta& getTypeMeta( StringId32 typeId ) {
        UniquePtr< INodeGraphTypeMeta >& typeMeta = s_typeMeta.at( typeId );
        return *typeMeta;
    }

    static const INodeGraphTypeMeta& getTypeMeta( PinTypeId pinTypeId ) {
        StringId32 typeId = s_runtimeToStaticTypeId.at( pinTypeId );
        UniquePtr< INodeGraphTypeMeta >& typeMeta = s_typeMeta.at( typeId );
        return *typeMeta;
    }

  private:
    static HashMap< StringId32, UniquePtr< INodeGraphTypeMeta > > s_typeMeta;
    static HashMap< PinTypeId, StringId32 > s_runtimeToStaticTypeId;
};
} // namespace onyx::node_graph
