#pragma once

namespace onyx::localization {
class ILocalizationBackend;

struct LocalizationId {
    LocalizationId() = default;

    LocalizationId( StringId32 id )
        : Context( 0 )
        , Id( id ) {}

    LocalizationId( StringId32 context, StringId32 id )
        : Context( context )
        , Id( id ) {}

    template < uint64_t N >
    consteval LocalizationId( const CompileTimeString< N >& str )
        : Context( 0 )
        , Id( str ) {}

    template < uint64_t N, uint64_t M >
    consteval LocalizationId( const CompileTimeString< N >& contextId, const CompileTimeString< M >& localizationId )
        : Context( contextId )
        , Id( localizationId ) {}

    template < uint64_t N >
    consteval LocalizationId( const char ( &str )[ N ] )
        : Id( StringView( str, StringView::traits_type::length( str ) ) ) {}

    template < uint64_t N, uint64_t M >
    consteval LocalizationId( const char ( &contextId )[ N ], const char ( &localizationId )[ N ] )
        : Context( StringView( contextId, StringView::traits_type::length( contextId ) ) )
        , Id( StringView( localizationId, StringView::traits_type::length( localizationId ) ) ) {}

    constexpr bool operator==( const LocalizationId& other ) const {
        return Context == other.Context && Id == other.Id;
    }
    constexpr bool IsValid() const { return Id.isValid(); }

    StringId32 Context;
    StringId32 Id;
};

// TODO: Investigate if caching the localized string would be better for performance
// Problem with caching is, how to realize count for plural forms
struct LocalizedString {
    LocalizedString() = default;
    LocalizedString( LocalizationId id, const ILocalizationBackend& backend );

    StringView Get() const;
    StringView Get( int32_t count ) const;

    bool IsValid() const { return m_LocalizationId.IsValid(); }

  private:
    LocalizationId m_LocalizationId;
    const ILocalizationBackend* m_LocalizationBackend = nullptr;
};
} // namespace onyx::localization

template <>
struct std::formatter< onyx::localization::LocalizedString > : std::formatter< std::string > {
    auto format( onyx::localization::LocalizedString localizedString, format_context& ctx ) const {
        return std::format_to( ctx.out(), "{}", localizedString.Get() );
    }
};

template <>
struct std::hash< onyx::localization::LocalizationId > {
    size_t operator()( const onyx::localization::LocalizationId& id ) const noexcept {
        uint64_t hash = onyx::hash::fnV1aHash( id.Id.getId(), 0 );
        return onyx::hash::fnV1aHash( id.Context.getId(), hash );
    }
};
