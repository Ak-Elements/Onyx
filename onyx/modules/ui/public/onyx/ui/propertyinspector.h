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
    template < typename T > requires HasPropertyInspector< T >
    static void registerInspector() {
        constexpr uint32_t typeId = TypeHash< T >();
        s_PropertyInspectors[ typeId ] = []( void* instance, bool forceShow ) {
            return PropertyInspector< T >::draw( *static_cast< T* >( instance ), forceShow );
        };
    }

    static bool draw( uint32_t runtimeTypeId, void* instance, bool forceShow ) {
        auto it = s_PropertyInspectors.find( runtimeTypeId );
        if ( it == s_PropertyInspectors.end() ) {
            ONYX_LOG_WARNING( "Type {} is missing an inspector. Should it be marked as runtime only?", runtimeTypeId );
            return false;
        }

        return it->second( instance, forceShow );
    }

    template < typename T >
    static bool isTypeRegistered() {
        constexpr uint32_t typeId = TypeHash< T >();
        return isTypeRegistered( typeId );
    }

    static bool isTypeRegistered( uint32_t runtimeTypeId ) { return s_PropertyInspectors.contains( runtimeTypeId ); }

  private:
    static inline HashMap< uint32_t, bool ( * )( void*, bool ) > s_PropertyInspectors;
};
} // namespace onyx::ui
#endif