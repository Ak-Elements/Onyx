#pragma once

#include <onyx/engine/enginevariablesregistry.h>
#include <onyx/noncopyable.h>
#include <onyx/serialize/deserializer.h>

namespace onyx {
class IEngine;
class Deserializer;

class IEngineSystem : public NonCopyable {
  public:
    ONYX_NO_DISCARD virtual StringId32 getTypeId() const = 0;
};

template < typename T >
concept EngineSystem = std::is_base_of_v< IEngineSystem, T >;

class IEngine : public NonCopyable {
  public:
    template < typename T > // requires std::is_base_of_v<IEngineSystem, T>
    ONYX_NO_DISCARD bool hasSystem() const {
        return hasSystem( T::TypeId );
    }

    template < typename T > // requires std::is_base_of_v<IEngineSystem, T>
    ONYX_NO_DISCARD T& getSystem() {
        ONYX_ASSERT( HasSystem( T::TypeId ), "System is not registered." );
        return static_cast< T& >( getSystem( T::TypeId ) );
    }

    template < typename T > // requires std::is_base_of_v<IEngineSystem, T>
    ONYX_NO_DISCARD const T& getSystem() const {
        ONYX_ASSERT( HasSystem( T::TypeId ), "System is not registered." );
        return static_cast< const T& >( getSystem( T::TypeId ) );
    }

    ONYX_NO_DISCARD virtual bool hasSystem( StringId32 systemId ) const = 0;
    virtual IEngineSystem& getSystem( StringId32 systemId ) = 0;
    ONYX_NO_DISCARD virtual const IEngineSystem& getSystem( StringId32 systemId ) const = 0;

    static const EngineVariablesRegistry& getVariablesRegistry() { return s_engineVariables; }

    static void registerVariable( IEngineVariable& variable ) { s_engineVariables.registerVariable( variable ); }
    static void unregisterVariable( IEngineVariable& variable ) { s_engineVariables.unregisterVariable( variable ); }

  private:
    static inline EngineVariablesRegistry s_engineVariables;
};

struct EngineSystemCreateContext {
    IEngine& Engine;
    const Deserializer& Deserializer;

    template < typename T > requires std::is_base_of_v< IEngine, T >
    T& get() const {
        return static_cast< T& >( Engine );
    }

    template < typename T > requires std::is_base_of_v< IEngineSystem, T >
    T& get() const {
        return Engine.getSystem< T >();
    }

    template < typename T > requires( Deserializable< T > && ( std::is_base_of_v< IEngineSystem, T > == false ) )
    auto get() const {
        T obj;
        Deserializer.read< "settings" >( obj );
        return obj;
    }
};

struct EngineSystemUpdateContext {
    IEngine& Engine;
    DeltaGameTime Delta { 0 };
    GameTime Time { 0 };

    template < typename T > requires std::is_base_of_v< IEngine, T >
    T& get() const {
        return static_cast< T& >( Engine );
    }

    template < typename T > requires std::is_base_of_v< IEngineSystem, T >
    T& get() const {
        return Engine.getSystem< T >();
    }

    template < typename T > requires std::is_same_v< DeltaGameTime, T >
    ONYX_NO_DISCARD DeltaGameTime get() const {
        return Delta;
    }

    template < typename T > requires std::is_same_v< GameTime, T >
    ONYX_NO_DISCARD GameTime get() const {
        return Time;
    }
};
} // namespace onyx
