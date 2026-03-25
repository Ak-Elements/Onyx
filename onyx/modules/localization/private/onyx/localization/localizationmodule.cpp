#include <onyx/assets/assetsystem.h>
#include <onyx/localization/backends/gettextlocalizationbackend.h>
#include <onyx/localization/localization.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/localization/serialize/portableobjectserializer.h>
#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::localization {
LocalizationModule::LocalizationModule( const LocalizationSettings& settings, assets::AssetSystem& assetSystem )
    : m_Settings( settings ) {
    m_LocalizationBackend = makeUnique< GetTextLocalizationBackend >();
    m_LocalizationBackend->Init( assetSystem, m_Settings );

    InitLocalization( *this );
}

LocalizedString LocalizationModule::GetLocalized( LocalizationId id ) const {
    return LocalizedString{ id, *m_LocalizationBackend };
}

Optional< StringView > LocalizationModule::TryGetLocalized( LocalizationId id ) const {
    return m_LocalizationBackend->GetLocalized( id );
}

#if !ONYX_IS_RETAIL
void LocalizationModule::AddSecondaryDatabase( const assets::AssetHandle< GetTextLocalizationDatabase >& database ) {
    m_LocalizationBackend->AddSecondaryDatabase( database );
}

void LocalizationModule::RemoveSecondaryDatabase( const assets::AssetHandle< GetTextLocalizationDatabase >& database ) {
    m_LocalizationBackend->RemoveSecondaryDatabase( database );
}
#endif
} // namespace onyx::localization

namespace onyx {
bool Serialization< localization::LocalizationSettings >::serialize(
    Serializer& serializer,
    const localization::LocalizationSettings& localizationSettings ) {
    return serializer.write< "locale" >( localizationSettings.Locale ) &&
           serializer.write< "database" >( localizationSettings.Database ) &&
           serializer.write< "secondarydatabases" >( localizationSettings.SecondaryDatabases );
}

bool Serialization< localization::LocalizationSettings >::deserialize(
    const Deserializer& deserializer,
    localization::LocalizationSettings& outLocalizationSettings ) {
    return deserializer.readOptional< "locale" >( outLocalizationSettings.Locale ) &&
           deserializer.readOptional< "database" >( outLocalizationSettings.Database ) &&
           deserializer.readOptional< "secondarydatabases" >( outLocalizationSettings.SecondaryDatabases );
}
} // namespace onyx
