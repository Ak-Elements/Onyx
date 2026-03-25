#pragma once

#include <onyx/filesystem/path.h>

namespace onyx::assets {
// AssetId is the hash of the asset full path e.g.: C:/MyProject/data/mytext.txt
struct AssetId {
    static constexpr AssetId invalid() { return {}; };

    constexpr AssetId() = default;
    constexpr explicit AssetId( uint64_t id )
        : m_id( id ) {}

    constexpr explicit AssetId( const char* path )
        : AssetId( StringView( path ) ) {}

    template < uint64_t N >
    constexpr AssetId( const char ( &str )[ N ] ) // NOLINT
        : AssetId( StringView( str, N - 1 ) ) {}

    constexpr explicit AssetId( StringView path )
        : m_id( path.empty() ? 0 : hash::fnV1aHash< uint64_t >( path ) )
#if ONYX_IS_DEBUG
        , m_path( path )
#endif
    {
    }

    constexpr explicit AssetId( const FilePath& path )
        : m_id( path.empty() ? 0 : hash::fnV1aHash< uint64_t >( path.generic_string() ) )
#if ONYX_IS_DEBUG
        , m_path( path.generic_string() )
#endif
    {
    }

    ONYX_NO_DISCARD uint64_t get() const { return m_id; }
    explicit operator uint64_t() const { return m_id; }

    template < uint64_t N >
    constexpr AssetId& operator=( const char ( &str )[ N ] ) {
        m_id = hash::fnV1aHash< uint64_t >( str );
#if ONYX_IS_DEBUG
        m_path = String( str );
#endif
        return *this;
    }

    bool operator==( const AssetId& other ) const { return m_id == other.m_id; }
    bool operator!=( const AssetId& other ) const { return m_id != other.m_id; }

    ONYX_NO_DISCARD bool isValid() const { return m_id != 0; }

#if ONYX_IS_DEBUG
    ONYX_NO_DISCARD StringView getPath() const { return StringView( m_path ); }
#endif

  private:
    uint64_t m_id = 0;
#if ONYX_IS_DEBUG
    String m_path;
#endif
};
} // namespace onyx::assets

namespace onyx {
template <> struct Serialization< assets::AssetId > {
    static bool serialize( Serializer& serializer, const assets::AssetId& assetId );
    static bool deserialize( const Deserializer& deserializer, assets::AssetId& outAssetId );
};
} // namespace onyx

template <> struct std::hash< onyx::assets::AssetId > {
    std::size_t operator()( const onyx::assets::AssetId& s ) const noexcept {
        std::size_t h1 = std::hash< uint64_t >{}( s.get() );
        return h1;
    }
};

template <> struct std::formatter< onyx::assets::AssetId > : std::formatter< std::string > {
    static auto format( onyx::assets::AssetId id, std::format_context& ctx ) {
        return std::format_to( ctx.out(), "{:x}", id.get() );
    }
};