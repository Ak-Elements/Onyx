#pragma once

#include <onyx/filesystem/path.h>

namespace onyx::assets {
// AssetId is UUID
struct AssetId {
    constexpr AssetId() = default;
    constexpr explicit AssetId( Guid64 id )
        : m_id( id ) {}

    static constexpr AssetId invalid() { return {}; };

    [[nodiscard]] uint64_t asUint64() const { return m_id.get(); }
    [[nodiscard]] Guid64 get() const { return m_id; }
    explicit operator Guid64() const { return m_id; }

    bool operator==( const AssetId& other ) const { return m_id == other.m_id; }
    bool operator!=( const AssetId& other ) const { return m_id != other.m_id; }

    [[nodiscard]] bool isValid() const { return m_id != Guid64::invalid(); }

  private:
    Guid64 m_id;
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
        return std::hash< onyx::Guid64 >()( s.get() );
    }
};

template <> struct std::formatter< onyx::assets::AssetId > : std::formatter< std::string > {
    static auto format( onyx::assets::AssetId id, std::format_context& ctx ) {
        return std::format_to( ctx.out(), "{}", id.get() );
    }
};
