#include <onyx/localization/localizationmodule.h>
#include <onyx/localization/backends/gettextlocalizationbackend.h>

namespace Onyx::Localization
{
    void LocalizationModule::Init(Assets::AssetSystem& assetSystem /*,const LocalizationSettings& localizationSettings*/)
    {
        LocalizationSettings localizationSettings;
        m_LocalizationBackends[StringId32("Editor")] = MakeUnique<GetTextLocalizationBackend>();
        m_LocalizationBackends[StringId32("Editor")]->Init(assetSystem, localizationSettings);
    }

    LocalizedString LocalizationModule::Localize(ILocalizationBackend& localizationBackend, StringId32 id) const
    {
        return LocalizedString{ id, localizationBackend };
    }

    LocalizedString LocalizationModule::Localize(ILocalizationBackend& localizationBackend, StringId32 id, onyxS32 count) const
    {
        return LocalizedString{ id, count, localizationBackend };
    }
}
