#pragma once

#include <onyx/onyx_types.h>
#include <onyx/serialize/serialization.h>

namespace onyx {
class Serializer;

class StringIdCache {
  public:
    ~StringIdCache() {}
    StringView store( StringView string ) {
        Optional< StringView > cachedString = tryGet( string );
        if ( cachedString.has_value() ) {
            return cachedString.value();
        }

        std::lock_guard lock( m_mutex );
        cachedString = tryGet( string );
        if ( cachedString.has_value() ) {
            return cachedString.value();
        }

        return m_cache.emplace_back( string );
    }

  private:
    Optional< StringView > tryGet( StringView string ) {
        auto it = std::ranges::find_if( m_cache,
                                        [ & ]( const String& cachedString ) { return string == cachedString; } );

        if ( it == m_cache.end() ) {
            return std::nullopt;
        }

        return { *it };
    }

  private:
    std::mutex m_mutex;
    std::deque< String > m_cache;
};

inline StringIdCache& getIdCache() {
    static StringIdCache Instance;
    return Instance;
}

template < typename T > requires std::is_integral_v< T >
struct StringId {
  private:
    friend struct Serialization< StringId >;

  public:
    using IdType = T;
    static constexpr T Invalid = 0;

    constexpr StringId()
#if ONYX_IS_RETAIL
        : id( T( Invalid ) )
#else
        : m_idString( "Invalid" )
        , m_id( T( Invalid ) )
#endif
    {
    }

    constexpr StringId( T id )
#if ONYX_IS_RETAIL
        : id( id )
#else
        : m_idString( "IdString not provided" )
        , m_id( id )
#endif
    {
    }

    constexpr StringId( StringView string )
#if ONYX_IS_RETAIL
        : id( hash::FNV1aHash< T >( string ) )
#else
        : m_idString( [ & ]() -> StringView {
            if ( std::is_constant_evaluated() ) {
                return string; // Safe to store view
            } else {
                return getIdCache().store( string );
            }
        }() )
        , m_id( hash::fnV1aHash< T >( string ) )
#endif
    {
    }

    template < uint64_t N >
    constexpr StringId( const char ( &str )[ N ] )
        : StringId( StringView( str, StringView::traits_type::length( str ) ) ) {}

    template < uint64_t N >
    consteval StringId( const CompileTimeString< N >& str )
        : StringId( str.stringView() ) {}

#if !ONYX_IS_RETAIL
    StringId( T id, const String& string )
        : m_idString( getIdCache().store( string ) )
        , m_id( id ) {}

    StringId( T id, StringView string )
        : m_idString( getIdCache().store( string ) )
        , m_id( id )

    {}
#endif

    constexpr operator T() const { return m_id; }

    ONYX_NO_DISCARD constexpr bool isValid() const { return m_id != Invalid; }
    constexpr void reset() {
        m_id = Invalid;
        m_idString = "";
    }

    constexpr bool operator==( const StringId& other ) const { return m_id == other.m_id; }
    constexpr bool operator!=( const StringId& other ) const { return m_id != other.m_id; }

    constexpr T getId() const { return m_id; }
    constexpr StringView getString() const { return m_idString; }

  private:
#if !ONYX_IS_RETAIL
    StringView m_idString;
#endif
    T m_id;
};

using StringId32 = StringId< uint32_t >;
using StringId64 = StringId< uint64_t >;

template < typename T >
constexpr bool IsStringId = is_specialization_of_v< StringId, T >;

consteval StringId32 operator""_id32( const char* deg, std::size_t len ) {
    return StringId32( StringView( deg, len ) );
}

consteval StringId64 operator""_id64( const char* deg, std::size_t len ) {
    return StringId64( StringView( deg, len ) );
}

template <>
struct Serialization< StringId32 > {
    static bool serialize( Serializer& serializer, const StringId32& id );
    static bool deserialize( const Deserializer& deserializer, StringId32& id );
};

template <>
struct Serialization< StringId64 > {
    static bool serialize( Serializer& serializer, const StringId64& id );
    static bool deserialize( const Deserializer& deserializer, StringId64& id );
};
} // namespace onyx

namespace std {
template <>
struct hash< onyx::StringId32 > {
    size_t operator()( const onyx::StringId32& id ) const noexcept { return id.getId(); }
};

template <>
struct hash< onyx::StringId64 > {
    size_t operator()( const onyx::StringId64& id ) const noexcept { return id.getId(); }
};

template <>
struct formatter< onyx::StringId32 > : std::formatter< std::string > {
    auto format( onyx::StringId32 id, format_context& ctx ) const {
#if ONYX_IS_RETAIL
        return std::format_to( ctx.out(), "{}({})", id.getId() );
#else
        return std::format_to( ctx.out(), "{}({})", id.getId(), id.getString() );
#endif
    }
};

template <>
struct formatter< onyx::StringId64 > : std::formatter< std::string > {
    auto format( onyx::StringId64 id, format_context& ctx ) const {
#if ONYX_IS_RETAIL
        return std::format_to( ctx.out(), "{}", id.getId() );
#else
        return std::format_to( ctx.out(), "{}({})", id.getId(), id.getString() );
#endif
    }
};
} // namespace std