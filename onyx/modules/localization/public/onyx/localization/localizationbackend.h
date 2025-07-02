#pragma once
#include <onyx/localization/localizedstring.h>

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::Localization
{
    class GetTextLocalizationDatabase;
    struct LocalizationSettings;

    class ILocalizationBackend
    {
        using LocalizationChangedSignalT = Signal<void(const ILocalizationBackend&)>;

    public:
        virtual ~ILocalizationBackend() = default;

        virtual void Init(Assets::AssetSystem& assetSystem, const LocalizationSettings& localizationSettings) = 0;

        virtual bool IsInitialized() const = 0;

        virtual Optional<StringView> GetLocalized(LocalizationId id) const = 0;
        virtual Optional<StringView> GetLocalized(LocalizationId id, onyxS32 count) const = 0;

#if !ONYX_IS_RETAIL
        virtual void AddSecondaryDatabase(const Reference<GetTextLocalizationDatabase>& database) = 0;
        virtual void RemoveSecondaryDatabase(const Reference<GetTextLocalizationDatabase>& database) = 0;
#endif

        Sink<LocalizationChangedSignalT> GetOnLocalizationChanged() { return Sink(m_OnLocalizationChanged); }

    private:
        LocalizationChangedSignalT m_OnLocalizationChanged;
    };
}
