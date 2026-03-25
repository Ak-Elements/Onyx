#pragma once

#include <onyx/assets/assethandle.h>
#include <onyx/localization/assets/gettextlocalizationdatabase.h>
#include <onyx/localization/localizedstring.h>

namespace onyx::assets {
class AssetSystem;
}

namespace onyx::localization {
struct LocalizationSettings;

class ILocalizationBackend {
    using LocalizationChangedSignalT = Signal< void( const ILocalizationBackend& ) >;

  public:
    virtual ~ILocalizationBackend() = default;

    virtual void Init( assets::AssetSystem& assetSystem, const LocalizationSettings& localizationSettings ) = 0;

    virtual bool IsInitialized() const = 0;

    virtual Optional< StringView > GetLocalized( LocalizationId id ) const = 0;
    virtual Optional< StringView > GetLocalized( LocalizationId id, int32_t count ) const = 0;

#if !ONYX_IS_RETAIL
    virtual void AddSecondaryDatabase( const assets::AssetHandle< GetTextLocalizationDatabase >& database ) = 0;
    virtual void RemoveSecondaryDatabase( const assets::AssetHandle< GetTextLocalizationDatabase >& database ) = 0;
#endif

    Sink< LocalizationChangedSignalT > GetOnLocalizationChanged() { return Sink( m_OnLocalizationChanged ); }

  private:
    LocalizationChangedSignalT m_OnLocalizationChanged;
};
} // namespace onyx::localization
