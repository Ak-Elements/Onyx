#include <onyx/assets/assetsystem.h>
#include <onyx/localization/localization.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/localization/backends/gettextlocalizationbackend.h>
#include <onyx/localization/serialize/portableobjectserializer.h>

namespace Onyx::Localization
{
    void LocalizationModule::Init(Assets::AssetSystem& assetSystem ,const LocalizationSettings& localizationSettings)
    {
        Assets::AssetSystem::Register<GetTextLocalizationDatabase, PortableObjectSerializer>(assetSystem);

        m_LocalizationBackend = MakeUnique<GetTextLocalizationBackend>();
        m_LocalizationBackend->Init(assetSystem, localizationSettings);

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
