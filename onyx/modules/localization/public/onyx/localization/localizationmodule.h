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
        Assets::AssetId m_Localization;
    };

    class LocalizationModule : public IEngineSystem
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Localization::LocalizationModule";
        StringId32 GetTypeId() const override { return TypeId; }

        void Init(Assets::AssetSystem& assetSystem, const LocalizationSettings& localizationSettings);

        LocalizedString GetLocalized(LocalizationId id) const;
        Optional<StringView> TryGetLocalized(LocalizationId id) const;

#if !ONYX_IS_RETAIL
        virtual void AddSecondaryDatabase(const Reference<GetTextLocalizationDatabase>& database);
        virtual void RemoveSecondaryDatabase(const Reference<GetTextLocalizationDatabase>& database);
#endif

    private:
        LocalizationSettings m_Settings;
        UniquePtr<ILocalizationBackend> m_LocalizationBackend;
    };
}
