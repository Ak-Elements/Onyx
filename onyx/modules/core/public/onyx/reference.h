#pragma once

#include <onyx/onyx_types.h>

namespace onyx {
template < typename T, typename D >
class Reference;

class RefCounted {
    template < typename U, typename D >
    friend class Reference;

  public:
    virtual ~RefCounted();
    ONYX_NO_DISCARD uint32_t getRefCount() const;

    void addReference();
    bool removeReference();

  private:
    Atomic< uint32_t > m_refCount = 0;
};

// TODO: Add Invalid handle
template < typename T, typename D = std::default_delete< T > >
class Reference {
    using ReferenceT = T;
    using DeleterT = D;

    template < typename U >
    struct RefCountedBase {
        typedef RefCounted Type;
    };

    template < typename U >
    struct RefCountedBase< U* > {
        typedef RefCounted* Type;
    };

    template < typename U >
    struct RefCountedBase< const U > {
        typedef const RefCounted Type;
    };

    template < typename U >
    struct RefCountedBase< const U* > {
        typedef const RefCounted* Type;
    };

    typedef typename RefCountedBase< T >::Type BaseT;

    template < typename U, typename F >
    friend class Reference;

  public:
    constexpr Reference() = default;

    constexpr Reference( std::nullptr_t ptr )
        : m_object( ptr ) {}

    Reference( T* obj )
        : m_object( obj ) {
        increaseRefCount();
    }

    Reference( T* obj, DeleterT deleter )
        : m_object( obj )
        , m_deleter( std::move( deleter ) ) {
        increaseRefCount();
    }

    Reference( const T* obj )
        : m_object( obj ) {
        increaseRefCount();
    }

    template < typename U >
    Reference( U* obj )
        : Reference( static_cast< T* >( obj ) ) {
        static_assert( std::is_base_of_v< T, U > || std::is_base_of_v< U, T >,
                       "Classes are not convertible or inherited from each other" );
    }

    template < typename U >
    Reference( const U* obj )
        : Reference( static_cast< T* >( std::remove_const_t< U* >( obj ) ) ) {
        static_assert( std::is_base_of_v< T, U > || std::is_base_of_v< U, T >,
                       "Classes are not convertible or inherited from each other" );
    }

    template < typename U >
    Reference( const Reference< U >& other ) {
        static_assert( std::is_base_of_v< T, U > || std::is_base_of_v< U, T >,
                       "Classes are not convertible or inherited from each other" );
        m_object = static_cast< T* >( other.m_object );
        m_deleter = other.m_deleter;
        increaseRefCount();
    }

    template < typename U >
    Reference( Reference< U >&& other ) {
        // TODO: Move ctor does not seem to work properly
        static_assert( std::is_base_of_v< T, U > || std::is_base_of_v< U, T >,
                       "Classes are not convertible or inherited from each other" );
        std::swap( m_object, other.m_object );
        // m_deleter = DeleterT(other.m_deleter);
        other.m_object = nullptr;
    }

    ~Reference() { decreaseRefCount(); }

    Reference( const Reference& other )
        : m_object( other.m_object )
        , m_deleter( other.m_deleter ) {
        increaseRefCount();
    }

    Reference( Reference&& other ) noexcept
        : m_object( std::move( other.m_object ) )
        , m_deleter( std::move( other.m_deleter ) ) {
        other.m_object = nullptr;
    }

    Reference& operator=( std::nullptr_t ) {
        decreaseRefCount();
        m_object = nullptr;
        return *this;
    }

    Reference& operator=( const Reference& other ) {
        if ( this == &other )
            return *this;

        other.increaseRefCount();
        decreaseRefCount();

        m_object = other.m_object;
        m_deleter = other.m_deleter;
        return *this;
    }

    Reference& operator=( Reference&& other ) noexcept {
        decreaseRefCount();

        m_object = std::move( other.m_object );
        m_deleter = std::move( other.m_deleter );

        other.m_object = nullptr;

        return *this;
    }

    template < typename U, typename UDeleter >
    Reference& operator=( const Reference< U, UDeleter >& other ) {
        static_assert( std::is_base_of_v< T, U > || std::is_base_of_v< U, T >,
                       "Classes are not convertible or inherited from each other" );
        other.increaseRefCount();
        decreaseRefCount();

        m_object = other.m_object;
        return *this;
    }

    template < typename U, typename UDeleter >
    Reference& operator=( Reference< U, UDeleter >&& other ) {
        static_assert( std::is_base_of_v< T, U > || std::is_base_of_v< U, T >,
                       "Classes are not convertible or inherited from each other" );
        decreaseRefCount();

        m_object = std::move( other.m_object );
        m_deleter = std::move( other.m_deleter );

        other.m_object = nullptr;

        return *this;
    }

    explicit operator bool() { return isValid(); }

    explicit operator bool() const { return isValid(); }

    T* operator->() {
        ONYX_ASSERT( m_object != nullptr );
        return raw();
    }

    const T* operator->() const {
        ONYX_ASSERT( m_object != nullptr );
        return raw();
    }

    T& operator*() {
        ONYX_ASSERT( m_object != nullptr );
        return *raw();
    }

    const T& operator*() const {
        ONYX_ASSERT( m_object != nullptr );
        return *raw();
    }

    T* raw();
    const T* raw() const;

    void reset( T* ptr = nullptr );

    T* release();

    bool isValid() const;

    template < typename U >
    const U& as() const;

    template < typename U >
    U& as();

    template < typename... Args > requires( !std::is_abstract_v< T > )
    static Reference create( Args&&... args ) {
        return Reference( new T( std::forward< Args >( args )... ) );
    }

    bool operator==( const Reference< T >& other ) const;
    bool operator!=( const Reference< T >& other ) const;

    template < typename U, typename DU >
    bool operator==( const Reference< U, DU >& other ) const {
        return m_object == other.m_object;
    }

    template < typename U, typename DU >
    bool operator!=( const Reference< U, DU >& other ) const {
        return !( *this == other );
    }

    static constexpr Reference invalid();

  private:
    void increaseRefCount() const;
    void decreaseRefCount() const;

  private:
    mutable BaseT* m_object = nullptr;
    // Use EBO to store the deleter
    [[no_unique_address]] DeleterT m_deleter{};
};
} // namespace onyx

#include <onyx/reference.hpp>