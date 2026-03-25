#pragma once

#include <onyx/assets/assethandle.h>
#include <onyx/localization/assets/gettextlocalizationdatabase.h>
#include <onyx/localization/localizationbackend.h>

namespace onyx::localization {
class GetTextLocalizationBackend : public ILocalizationBackend {
  public:
    void Init( assets::AssetSystem& assetSystem, const LocalizationSettings& localizationSettings ) override;

    bool IsInitialized() const override { return m_MainDatabase.isValid() && m_MainDatabase.isLoaded(); }

    Optional< StringView > GetLocalized( LocalizationId id ) const override;
    Optional< StringView > GetLocalized( LocalizationId id, int32_t count ) const override;

#if !ONYX_IS_RETAIL
    void AddSecondaryDatabase( const assets::AssetHandle< GetTextLocalizationDatabase >& database ) override;
    void RemoveSecondaryDatabase( const assets::AssetHandle< GetTextLocalizationDatabase >& database ) override;
#endif

  private:
    Optional< StringView > GetLocalized( LocalizationId id, const GetTextLocalizationDatabase& database ) const;
    Optional< StringView > GetLocalized( LocalizationId id,
                                         int32_t count,
                                         const GetTextLocalizationDatabase& database ) const;

  private:
    assets::AssetHandle< GetTextLocalizationDatabase > m_MainDatabase;

    // for the editor & debug builds we support additional databases
#if !ONYX_IS_RETAIL
    DynamicArray< assets::AssetHandle< GetTextLocalizationDatabase > > m_SecondaryDatabases;
#endif
};
} // namespace onyx::localization
