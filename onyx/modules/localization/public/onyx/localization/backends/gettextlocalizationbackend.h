#pragma once
#include <onyx/localization/localizationbackend.h>
#include <onyx/filesystem/path.h>

namespace Onyx::Localization
{
    class GetTextLocalizationBackend : public ILocalizationBackend
    {
    public:
        void Init(Assets::AssetSystem& assetSystem, const LocalizationSettings& localizationSettings) override;
        Optional<StringView> GetLocalized(LocalizationId id) const override;
        Optional<StringView> GetLocalized(LocalizationId id, onyxS32 count) const override;

    private:
        void ParsePoFile(const FileSystem::Filepath& path);


    private:
        HashMap<LocalizationId, DynamicArray<String>> m_LocaleDatabase;
        InplaceFunction<onyxS32(onyxS32)> m_PluralFunction;
    };
}
