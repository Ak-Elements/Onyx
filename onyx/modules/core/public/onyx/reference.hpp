#pragma once

#include <onyx/typetraits.h>

namespace onyx {
// RefCounted
inline RefCounted::~RefCounted() = default;

inline uint32_t RefCounted::getRefCount() const {
    return m_refCount;
}

inline void RefCounted::addReference() {
    m_refCount.fetch_add( 1 );
}

inline bool RefCounted::removeReference() {
    return ( m_refCount.fetch_sub( 1, std::memory_order::acq_rel ) == 1 );
}

// Reference
template < typename T, typename D >
T* Reference< T, D >::raw() {
    ONYX_ASSERT( m_object != nullptr );
    return static_cast< T* >( m_object );
}

template < typename T, typename D >
const T* Reference< T, D >::raw() const {
    ONYX_ASSERT( m_object != nullptr );
    return static_cast< T* >( m_object );
}

template < typename T, typename D >
void Reference< T, D >::reset( T* ptr ) {
    decreaseRefCount();
    m_object = ptr;
    increaseRefCount();
}

template < typename T, typename D >
T* Reference< T, D >::release() {
    T* ptr = raw();
    m_object = nullptr;
    return ptr;
}

template < typename T, typename D >
bool Reference< T, D >::isValid() const {
    if constexpr ( HasIsValid< T > )
        return m_object != nullptr && static_cast< const T* >( m_object )->IsValid();
    else
        return m_object != nullptr;
}

template < typename T, typename D >
template < typename U >
const U& Reference< T, D >::as() const {
    static_assert( std::is_base_of_v< T, U > || std::is_base_of_v< U, T >,
                   "Classes are not convertible or inherited from each other" );
    ONYX_ASSERT( m_object != nullptr );
    return *static_cast< U* >( m_object );
}

template < typename T, typename D >
template < typename U >
U& Reference< T, D >::as() {
    static_assert( std::is_base_of_v< T, U > || std::is_base_of_v< U, T >,
                   "Classes are not convertible or inherited from each other" );
    ONYX_ASSERT( m_object != nullptr );
    return *static_cast< U* >( m_object );
}

template < typename T, typename D >
bool Reference< T, D >::operator==( const Reference< T >& other ) const {
    return m_object == other.m_object;
}

template < typename T, typename D >
bool Reference< T, D >::operator!=( const Reference< T >& other ) const {
    return !( *this == other );
}

template < typename T, typename D >
constexpr Reference< T, D > Reference< T, D >::invalid() {
    return nullptr;
}

template < typename T, typename D >
void Reference< T, D >::increaseRefCount() const {
    if ( m_object )
        m_object->addReference();
}

template < typename T, typename D >
void Reference< T, D >::decreaseRefCount() const {
    ONYX_ASSERT( ( m_Object == nullptr ) || ( m_Object->m_RefCount != 0 ) );
    if ( m_object && m_object->removeReference() ) {
        m_deleter( std::remove_const_t< T* >( raw() ) );
        m_object = nullptr;
    }
}
} // namespace onyx
