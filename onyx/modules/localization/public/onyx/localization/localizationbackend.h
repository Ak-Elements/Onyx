#pragma once
#include <onyx/localization/localizedstring.h>

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::Localization
{
    struct LocalizationSettings;

    class ILocalizationBackend
    {
        using LocalizationChangedSignalT = Signal<void(const ILocalizationBackend&)>;

    public:
        virtual ~ILocalizationBackend() = default;

        virtual void Init(Assets::AssetSystem& assetSystem, const LocalizationSettings& localizationSettings) = 0;
        virtual Optional<StringView> Localize(StringId32 id) const = 0;
        virtual Optional<StringView> Localize(StringId32 id, onyxS32 count) const = 0;

        Sink<LocalizationChangedSignalT> GetOnLocalizationChanged() { return Sink(m_OnLocalizationChanged); }

    private:
        LocalizationChangedSignalT m_OnLocalizationChanged;
    };
}
