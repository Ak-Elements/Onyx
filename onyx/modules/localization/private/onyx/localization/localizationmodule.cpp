#include <onyx/assets/assetsystem.h>
#include <onyx/localization/localization.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/localization/backends/gettextlocalizationbackend.h>
#include <onyx/localization/serialize/portableobjectserializer.h>
#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx::Localization
{
    void LocalizationModule::Init(Assets::AssetSystem& assetSystem)
    {
        Assets::AssetSystem::Register<GetTextLocalizationDatabase>();
        Assets::AssetSystem::Register<PortableObjectSerializer>(assetSystem);

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
    void LocalizationModule::AddSecondaryDatabase(const Reference<GetTextLocalizationDatabase>& database)
    {
        m_LocalizationBackend->AddSecondaryDatabase(database);
    }

    void LocalizationModule::RemoveSecondaryDatabase(const Reference<GetTextLocalizationDatabase>& database)
    {
        m_LocalizationBackend->RemoveSecondaryDatabase(database);
    }
#endif
}

namespace Onyx
{
    bool Serialization<Localization::LocalizationModule>::Serialize(Serializer& /*serializer*/, const Localization::LocalizationModule& /*localizationModule*/)
    {
        return true;
    }

    bool Serialization<Localization::LocalizationModule>::Deserialize(const Deserializer& deserializer, Localization::LocalizationModule& outLocalizationModule)
    {
        return deserializer.ReadOptional<"localization">(outLocalizationModule.m_Settings);
    }

    bool Serialization<Localization::LocalizationSettings>::Serialize(Serializer& serializer, const Localization::LocalizationSettings& localizationSettings)
    {
        return serializer.Write<"locale">(localizationSettings.Locale) &&
            serializer.Write<"database">(localizationSettings.Database);

    }

    bool Serialization<Localization::LocalizationSettings>::Deserialize(const Deserializer& deserializer, Localization::LocalizationSettings& outLocalizationSettings)
    {
        return deserializer.Read<"locale">(outLocalizationSettings.Locale) &&
            deserializer.Read<"database">(outLocalizationSettings.Database);
    }
}
