#include <onyx/localization/localizationbackend.h>
#include <onyx/localization/localizedstring.h>

namespace onyx::localization {
LocalizedString::LocalizedString( LocalizationId id, const ILocalizationBackend& backend )
    : m_LocalizationId( id )
    , m_LocalizationBackend( &backend ) {}

StringView LocalizedString::Get() const {
    Optional< StringView > localized = m_LocalizationBackend->GetLocalized( m_LocalizationId );
#if ONYX_IS_RETAIL
    return *localized;
#else
    if ( localized.has_value() ) {
        return *localized;
    } else {
        return format::format( "!!!{}:{}", m_LocalizationId.Id.getString(), m_LocalizationId.Id.getId() );
    }
#endif
}

StringView LocalizedString::Get( int32_t count ) const {
    Optional< StringView > localized = m_LocalizationBackend->GetLocalized( m_LocalizationId, count );
#if ONYX_IS_RETAIL
    return *localized;
#else
    if ( localized.has_value() ) {
        return *localized;
    } else {
        return format::format( "!!!{}:{}", m_LocalizationId.Id.getString(), m_LocalizationId.Id.getId() );
    }
#endif
}
} // namespace onyx::localization
