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

    //LocalizedString LocalizationModule::Localize(LocalizationId id, onyxS32 count) const
    //{
    //    for (const UniquePtr<ILocalizationBackend>& localizationBackend : m_LocalizationBackends | std::views::values )
    //    {
    //        Optional<StringView> localizedText = localizationBackend->Localize(id, count);
    //        if (localizedText.has_value())
    //        {
    //            return LocalizedString{ id, localizedText, *localizationBackend };
    //        }
    //    }

    //    ONYX_LOG_WARNING("Failed finding localization id({}) in any context", id.Id);
    //    return LocalizedString{ id, std::nullopt, count, *m_LocalizationBackends.begin()->second };
    //}

    //LocalizedString LocalizationModule::Localize(LocalizationId id) const
    //{
    //    for (const UniquePtr<ILocalizationBackend>& localizationBackend : m_LocalizationBackends | std::views::values)
    //    {
    //        Optional<StringView> localizedText = localizationBackend->Localize(id);
    //        if (localizedText.has_value())
    //        {
    //            return LocalizedString{ id, localizedText, *localizationBackend };
    //        }
    //    }

    //    ONYX_LOG_WARNING("Failed finding localization id({}) in any context", id.Id);
    //    return LocalizedString{ id, std::nullopt, *m_LocalizationBackends.begin()->second };
    //}

    LocalizedString LocalizationModule::DoLocalize(ILocalizationBackend& localizationBackend, LocalizationId id) const
    {
        
        return LocalizedString{ id, localizationBackend.Localize(id), localizationBackend };
    }

    LocalizedString LocalizationModule::DoLocalize(ILocalizationBackend& localizationBackend, LocalizationId id, onyxS32 count) const
    {
        return LocalizedString{ id, localizationBackend.Localize(id), count, localizationBackend };
    }
}
