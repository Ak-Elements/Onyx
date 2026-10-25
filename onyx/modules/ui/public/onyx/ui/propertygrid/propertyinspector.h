#pragma once

#if !ONYX_IS_RETAIL
namespace onyx::ui {
template < typename T >
struct PropertyInspector;

template < typename T >
concept HasPropertyInspector = requires( T& instance, bool forceShow ) {
    { PropertyInspector< T >::draw( instance, forceShow ) } -> std::same_as< bool >;
};

template < typename T >
struct PropertyInspector {
    static bool draw( T& /*instance*/, bool /*forceShow*/ ) { return false; }
};

struct PropertyInspectors {
  public:
    using InspectFunctionT = bool ( * )( void*, bool );

    template < typename T > requires HasPropertyInspector< T >
    static void registerInspector() {
        constexpr uint32_t RuntimeTypeId = typeHash< T >();

        if constexpr( HasTypeId< T > ) {
            s_staticIdToRuntimeId[ T::TypeId ] = RuntimeTypeId;
        }
        s_propertyInspectors[ RuntimeTypeId ] = []( void* instance, bool forceShow ) {
            return PropertyInspector< T >::draw( *static_cast< T* >( instance ), forceShow );
        };
    }

    static bool draw( uint32_t runtimeTypeId, void* instance, bool forceShow ) {
        auto it = s_propertyInspectors.find( runtimeTypeId );
        if( it == s_propertyInspectors.end() ) {
            ONYX_LOG_WARNING( "Type {} is missing an inspector. Should it be marked as runtime only?", runtimeTypeId );
            return false;
        }

        return it->second( instance, forceShow );
    }

    template < typename T >
    static Optional< InspectFunctionT > getInspector() {
        constexpr uint32_t TypeId = typeHash< T >();
        auto it = s_propertyInspectors.find( TypeId );
        if( it == s_propertyInspectors.end() ) {
            return std::nullopt;
        }

        return it->second;
    }

    static Optional< InspectFunctionT > getInspector( StringId32 typeId ) {
        auto idIt = s_staticIdToRuntimeId.find( typeId );
        if( idIt == s_staticIdToRuntimeId.end() )
            return std::nullopt;

        auto it = s_propertyInspectors.find( idIt->second );
        if( it == s_propertyInspectors.end() ) {
            return std::nullopt;
        }

        return it->second;
    }

    template < typename T >
    static bool isTypeRegistered() {
        constexpr uint32_t TypeId = typeHash< T >();
        return isTypeRegistered( TypeId );
    }

    static bool isTypeRegistered( uint32_t runtimeTypeId ) { return s_propertyInspectors.contains( runtimeTypeId ); }

  private:
    static inline HashMap< StringId32, uint32_t > s_staticIdToRuntimeId;
    static inline HashMap< uint32_t, InspectFunctionT > s_propertyInspectors;
};
} // namespace onyx::ui
#endif
