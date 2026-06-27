#include <onyx/localization/backends/gettextlocalizationbackend.h>

#include <onyx/assets/assetsystem.h>

#include <onyx/localization/localizationmodule.h>
#include <onyx/localization/localizedstring.h>

namespace onyx::localization {
namespace {}
void GetTextLocalizationBackend::Init( assets::AssetSystem& assetSystem,
                                       const LocalizationSettings& localizationSettings ) {
    if( localizationSettings.Database.isValid() ) {
        assetSystem.getAsset( localizationSettings.Database, m_MainDatabase );
    }

#if !ONYX_IS_RETAIL
    for( const assets::AssetId& secondaryDatabaseId : localizationSettings.SecondaryDatabases ) {
        assets::AssetHandle< GetTextLocalizationDatabase > secondaryDb;
        assetSystem.getAsset( secondaryDatabaseId, secondaryDb );
        AddSecondaryDatabase( secondaryDb );
    }
#endif
}

Optional< StringView > GetTextLocalizationBackend::GetLocalized( LocalizationId id ) const {
#if ONYX_IS_RETAIL
    Optional< StringView > localization = GetLocalized( id, *m_MainDatabase );
    return localization;
#else
    Optional< StringView > localization;
    if( IsInitialized() ) {
        localization = GetLocalized( id, *m_MainDatabase );
    } else {
        // TODO: Add unique log
        // ONYX_LOG_WARNING("Missing main localization file for project.");
    }

    if( localization.has_value() == false ) {
        for( const auto& secondaryDb : m_SecondaryDatabases ) {
            if( ( secondaryDb.isValid() == false ) || ( secondaryDb->isLoaded() == false ) ) {
                continue;
            }

            localization = GetLocalized( id, *secondaryDb );
            if( localization.has_value() ) {
                break;
            }
        }
    }

    return localization;
#endif
}

Optional< StringView > GetTextLocalizationBackend::GetLocalized( LocalizationId id, int32_t count ) const {
#if ONYX_IS_RETAIL
    Optional< StringView > localization = GetLocalized( id, count, *m_MainDatabase );
    return localization;
#else
    Optional< StringView > localization;
    if( IsInitialized() ) {
        localization = GetLocalized( id, count, *m_MainDatabase );
    } else {
        // TODO: Add unique log
        // ONYX_LOG_WARNING("Missing main localization file for project.");
    }

    if( localization.has_value() == false ) {
        for( const auto& secondaryDb : m_SecondaryDatabases ) {
            if( ( secondaryDb.isValid() == false ) || ( secondaryDb->isLoaded() == false ) ) {
                continue;
            }

            localization = GetLocalized( id, count, *secondaryDb );
            if( localization.has_value() ) {
                break;
            }
        }
    }

    return localization;
#endif
}

Optional< StringView > GetTextLocalizationBackend::GetLocalized( LocalizationId id,
                                                                 const GetTextLocalizationDatabase& database ) const {
    const HashMap< LocalizationId, DynamicArray< String > >& databaseMap = database.getDatabase();
    auto it = databaseMap.find( id );
    if( it == databaseMap.end() ) {
        return std::nullopt;
    }

    return it->second.front();
}

Optional< StringView > GetTextLocalizationBackend::GetLocalized( LocalizationId id,
                                                                 int32_t count,
                                                                 const GetTextLocalizationDatabase& database ) const {
    const HashMap< LocalizationId, DynamicArray< String > >& databaseMap = database.getDatabase();
    auto it = databaseMap.find( id );
    if( it == databaseMap.end() ) {
        return std::nullopt;
    }

    int32_t index = database.getPluralFunction()( count );
    const DynamicArray< String >& localizedTexts = it->second;
    if( index > static_cast< int32_t >( localizedTexts.size() ) ) {
        return std::nullopt;
    }

    return localizedTexts[ index ];
}

#if !ONYX_IS_RETAIL
void GetTextLocalizationBackend::AddSecondaryDatabase(
    const assets::AssetHandle< GetTextLocalizationDatabase >& database ) {
    if( m_MainDatabase.isValid() && ( database == m_MainDatabase ) ) {
        return;
    }

    auto it = std::ranges::find_if( m_SecondaryDatabases,
                                    [ & ]( const assets::AssetHandle< GetTextLocalizationDatabase >& secondaryDb ) {
                                        return secondaryDb == database;
                                    } );

    if( it != m_SecondaryDatabases.end() ) {
        return;
    }

    m_SecondaryDatabases.push_back( assets::AssetHandle( database ) );
}

void GetTextLocalizationBackend::RemoveSecondaryDatabase(
    const assets::AssetHandle< GetTextLocalizationDatabase >& database ) {
    std::erase_if( m_SecondaryDatabases,
                   [ & ]( const assets::AssetHandle< GetTextLocalizationDatabase >& secondaryDb ) {
                       return secondaryDb == database;
                   } );
}
#endif
} // namespace onyx::localization
