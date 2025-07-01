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
        DynamicArray<String> m_LocalizationFiles;
    };

    class LocalizationModule : public IEngineSystem
    {
    public:
        void Init(Assets::AssetSystem& assetSystem, const LocalizationSettings& localizationSettings);

        LocalizedString GetLocalized(LocalizationId id) const;
        Optional<StringView> TryGetLocalized(LocalizationId id) const;

    private:
        LocalizationSettings m_Settings;
        UniquePtr<ILocalizationBackend> m_LocalizationBackend;
    };
}
