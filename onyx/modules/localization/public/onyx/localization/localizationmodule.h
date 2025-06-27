#pragma once
#include <onyx/engine/enginesystem.h>
#include <onyx/assets/asset.h>

#include <onyx/localization/localizationbackend.h>

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::Localization
{
    class ILocalizationBackend;
}

namespace Onyx::Localization
{
    struct LocalizationSettings
    {
        onyxS32 Locale;
        // probably a flag indicating the backend to use
        Assets::AssetId AssetId;
    };

    class LocalizationModule : public IEngineSystem
    {
    public:
        void Init(Assets::AssetSystem& assetSystem/*, const LocalizationSettings& localizationSettings*/);

        template <CompileTimeString Context>
        LocalizedString Localize(LocalizationId id, onyxS32 count) const
        {
            const UniquePtr<ILocalizationBackend>& localizationBackend = m_LocalizationBackends.at(Context);
            return DoLocalize(*localizationBackend, id, count);
        }

        template <CompileTimeString Context>
        LocalizedString Localize(LocalizationId id) const
        {
            const UniquePtr<ILocalizationBackend>& localizationBackend = m_LocalizationBackends.at(Context);
            return DoLocalize(*localizationBackend, id);
        }

        // Search in all contexts, not sure if this is needed
        //LocalizedString Localize(LocalizationId id, onyxS32 count) const;
        //LocalizedString Localize(LocalizationId id) const;

    private:
        LocalizedString DoLocalize(ILocalizationBackend& localizationBackend, LocalizationId id) const;
        LocalizedString DoLocalize(ILocalizationBackend& localizationBackend, LocalizationId id, onyxS32 count) const;

    private:
        LocalizationSettings m_Settings;
        HashMap<StringId32, UniquePtr<ILocalizationBackend>> m_LocalizationBackends;
    };
}
