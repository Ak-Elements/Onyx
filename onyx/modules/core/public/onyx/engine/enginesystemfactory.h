#pragma once
#include <onyx/engine/enginesystem.h>

#include <onyx/serialize/deserializer.h>

namespace onyx {
template < typename T >
concept IsMemberFunctionPointer = std::is_member_function_pointer_v< T >;

template < typename T >
concept Initializable = requires( T type ) {
    { &T::init } -> IsMemberFunctionPointer;
};

template < typename T >
concept Shutdownable = requires( T type ) {
    { &T::shutdown } -> IsMemberFunctionPointer;
};

template < typename T >
concept Updatable = requires( T type ) {
    { &T::update } -> IsMemberFunctionPointer;
};

template < typename T >
concept HasSettingsT = requires { typename T::SettingsT; };

template < typename T >
struct EngineSystemMeta {};

struct EngineSystemFactory {
    using CreateFunction = UniquePtr< IEngineSystem > ( * )( const EngineSystemCreateContext& );
    using UpdateFunction = void ( * )( IEngineSystem&, const EngineSystemUpdateContext& );

    template < typename T > requires std::is_base_of_v< IEngineSystem, T >
    static bool registerSystem() {
        s_createFunctions[ T::TypeId ] = &EngineSystemMeta< T >::create;

        if constexpr ( Updatable< T > ) {
            s_updateFunctions[ T::TypeId ] = &EngineSystemMeta< T >::update;
        }

        return true;
    }

    static UniquePtr< IEngineSystem > create( StringId32 moduleId, const EngineSystemCreateContext& context ) {
        ONYX_ASSERT( s_CreateFunctions.contains( moduleId ) );
        return s_createFunctions.at( moduleId )( context );
    }

    static Optional< UpdateFunction > getUpdate( StringId32 moduleId ) {
        auto it = s_updateFunctions.find( moduleId );
        if ( it == s_updateFunctions.end() )
            return std::nullopt;

        return it->second;
    }

  private:
    static inline HashMap< StringId32, CreateFunction > s_createFunctions;
    static inline HashMap< StringId32, UpdateFunction > s_updateFunctions;
};
} // namespace onyx
