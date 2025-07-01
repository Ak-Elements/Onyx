#include <onyx/localization/localization.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/localization/backends/gettextlocalizationbackend.h>

namespace Onyx::Localization
{
    void LocalizationModule::Init(Assets::AssetSystem& assetSystem ,const LocalizationSettings& localizationSettings)
    {
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
}
