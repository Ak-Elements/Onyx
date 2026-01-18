#pragma once

#include <onyx/assets/assethandle.h>
#include <onyx/localization/localizationbackend.h>
#include <onyx/localization/assets/gettextlocalizationdatabase.h>

namespace Onyx::Localization
{
    class GetTextLocalizationBackend : public ILocalizationBackend
    {
    public:
        void Init(Assets::AssetSystem& assetSystem, const LocalizationSettings& localizationSettings) override;

        bool IsInitialized() const override { return m_MainDatabase.IsValid() && m_MainDatabase.IsLoaded(); }

        Optional<StringView> GetLocalized(LocalizationId id) const override;
        Optional<StringView> GetLocalized(LocalizationId id, onyxS32 count) const override;

#if !ONYX_IS_RETAIL
        void AddSecondaryDatabase(const Assets::AssetHandle<GetTextLocalizationDatabase>& database) override;
        void RemoveSecondaryDatabase(const Assets::AssetHandle<GetTextLocalizationDatabase>& database) override;
#endif

    private:
        Optional<StringView> GetLocalized(LocalizationId id, const GetTextLocalizationDatabase& database) const;
        Optional<StringView> GetLocalized(LocalizationId id, onyxS32 count, const GetTextLocalizationDatabase& database) const;

    private:
        Assets::AssetHandle<GetTextLocalizationDatabase> m_MainDatabase;

        // for the editor & debug builds we support additional databases
#if !ONYX_IS_RETAIL
        DynamicArray<Assets::AssetHandle<GetTextLocalizationDatabase>> m_SecondaryDatabases;
#endif
    };
}
