#include <onyx/assets/assetsystem.h>
#include <onyx/localization/localization.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/localization/backends/gettextlocalizationbackend.h>
#include <onyx/localization/serialize/portableobjectserializer.h>
#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx::Localization
{
    LocalizationModule::LocalizationModule(const LocalizationSettings& settings, Assets::AssetSystem& assetSystem)
        : m_Settings(settings)
    {
        m_LocalizationBackend = MakeUnique<GetTextLocalizationBackend>();
        m_LocalizationBackend->Init(assetSystem, m_Settings);

        InitLocalization(*this);
    }

    LocalizedString LocalizationModule::GetLocalized(LocalizationId id) const
    {
        return LocalizedString{ id, *m_LocalizationBackend };
    }

    Optional<StringView> LocalizationModule::TryGetLocalized(LocalizationId id) const
    {
        return m_LocalizationBackend->GetLocalized(id);
    }

#if !ONYX_IS_RETAIL
    void LocalizationModule::AddSecondaryDatabase(const Assets::AssetHandle<GetTextLocalizationDatabase>& database)
    {
        m_LocalizationBackend->AddSecondaryDatabase(database);
    }

    void LocalizationModule::RemoveSecondaryDatabase(const Assets::AssetHandle<GetTextLocalizationDatabase>& database)
    {
        m_LocalizationBackend->RemoveSecondaryDatabase(database);
    }
#endif
}

namespace Onyx
{
    bool Serialization<Localization::LocalizationSettings>::Serialize(Serializer& serializer, const Localization::LocalizationSettings& localizationSettings)
    {
        return serializer.Write<"locale">(localizationSettings.Locale) &&
            serializer.Write<"database">(localizationSettings.Database) &&
            serializer.Write<"secondarydatabases">(localizationSettings.SecondaryDatabases);

    }

    bool Serialization<Localization::LocalizationSettings>::Deserialize(const Deserializer& deserializer, Localization::LocalizationSettings& outLocalizationSettings)
    {
        return deserializer.ReadOptional<"locale">(outLocalizationSettings.Locale) &&
            deserializer.ReadOptional<"database">(outLocalizationSettings.Database) &&
            deserializer.ReadOptional<"secondarydatabases">(outLocalizationSettings.SecondaryDatabases);
    }
}
