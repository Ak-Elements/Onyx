#pragma once
#include <onyx/engine/enginesystem.h>
#include <onyx/assets/asset.h>

#include "localizedstring.h"

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
        void Init(Assets::AssetSystem& assetSystem, const LocalizationSettings& localizationSettings);

        LocalizedString Localize(LocalizedStringId id) const;

    private:
        LocalizationSettings m_Settings;
        UniquePtr<ILocalizationBackend> m_LocalizationBackend;
    };
}
