#pragma once
#include <onyx/localization/localizationbackend.h>

namespace Onyx::Localization
{
    class GetTextLocalizationBackend : public ILocalizationBackend
    {
    public:
        virtual void Init(Assets::AssetSystem& assetSystem, const LocalizationSettings& localizationSettings) override;
        Optional<StringView> Localize(StringId32 id) const override;
        Optional<StringView> Localize(StringId32 id, onyxS32 count) const override;

    private:
        HashMap<StringId32, DynamicArray<String>> m_LocaleDatabase;
        InplaceFunction<onyxS32(onyxS32)> m_PluralFunction;
    };
}
