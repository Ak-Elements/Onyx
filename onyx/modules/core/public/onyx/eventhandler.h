#pragma once

#include <onyx/assert.h>
#include <onyx/inplacefunction.h>

namespace onyx {
template < typename... Args >
class EventDispatcher {
    using EventHandlerFunction = InplaceFunction< void( Args... ) >;

  public:
#if ONYX_IS_DEBUG
    ~EventDispatcher() { ONYX_ASSERT( m_EventHandlers.empty(), "Event Handlers did not get released" ); }
#endif

    void addDelegate( EventHandlerFunction&& delegate ) { m_eventHandlers.push_back( delegate ); }

    template < class Obj >
    void addDelegate( Obj* obj, void ( Obj::*memberFunction )( Args... ) ) {
        m_eventHandlers.emplace_back( obj, memberFunction );
    }

    void removeDelegate( EventHandlerFunction delegate ) {
        for ( auto it = m_eventHandlers.cbegin(); it != m_eventHandlers.cend(); ++it ) {
            const EventHandlerFunction& handler = *it;
            if ( delegate == handler ) {
                m_eventHandlers.erase( it );
                return;
            }
        }

        ONYX_ASSERT( false, "Failed removing event handler" );
    }

    template < class Obj >
    void removeDelegate( Obj* obj, void ( Obj::*memberFunction )( Args... ) ) {
        removeDelegate( { obj, memberFunction } );
    }

    void removeAll() { m_eventHandlers.clear(); }

    void operator()( Args... args ) const {
        for ( const EventHandlerFunction& eventHandler : m_eventHandlers ) {
            eventHandler( std::forward< Args >( args )... );
        }
    }

  private:
    DynamicArray< EventHandlerFunction > m_eventHandlers;
};

#define ONYX_EVENT( name, ... )                                                                                        \
  private:                                                                                                             \
    using name##EventHandler = InplaceFunction< void( __VA_ARGS__ ) >;                                                 \
                                                                                                                       \
  public:                                                                                                              \
    void Add##name##Handler( name##EventHandler handler ) {                                                            \
        m_##name.addDelegate( std::forward< name##EventHandler >( handler ) );                                         \
    }                                                                                                                  \
    template < class Obj >                                                                                             \
    void Add##name##Handler( Obj* obj, void ( Obj::*memberFunction )( __VA_ARGS__ ) ) {                                \
        m_##name.addDelegate( obj, memberFunction );                                                                   \
    }                                                                                                                  \
    void Remove##name##Handler( name##EventHandler handler ) {                                                         \
        m_##name.removeDelegate( std::forward< name##EventHandler >( handler ) );                                      \
    }                                                                                                                  \
    template < class Obj >                                                                                             \
    void Remove##name##Handler( Obj* obj, void ( Obj::*memberFunction )( __VA_ARGS__ ) ) {                             \
        m_##name.removeDelegate( obj, memberFunction );                                                                \
    }                                                                                                                  \
                                                                                                                       \
    void RemoveAll##name##Handlers() {                                                                                 \
        m_##name.removeAll();                                                                                          \
    }                                                                                                                  \
                                                                                                                       \
  private:                                                                                                             \
    EventDispatcher< __VA_ARGS__ > m_##name;
} // namespace onyx
