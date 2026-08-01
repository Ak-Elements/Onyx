#pragma once
#include <onyx/assets/assetid.h>
#include <onyx/engine/enginesystem.h>

#include <onyx/localization/localizationbackend.h>

namespace onyx::assets {
class AssetSystem;
}

namespace onyx::localization {
class ILocalizationBackend;
}

namespace onyx::localization {
struct LocalizationSettings {
    int32_t Locale;
    // probably a flag indicating the backend to use
    assets::AssetId Database;

#if !ONYX_IS_RETAIL
    DynamicArray< assets::AssetId > SecondaryDatabases;
#endif
};

class LocalizationModule : public IEngineSystem {
    friend struct Serialization< LocalizationModule >;

  public:
    static constexpr StringId32 TypeId = "onyx::localization::LocalizationSystem";
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

    LocalizationModule( const LocalizationSettings& settings, assets::AssetSystem& assetSystem );

    [[nodiscard]] LocalizedString getLocalized( LocalizationId id ) const;
    [[nodiscard]] Optional< StringView > tryGetLocalized( LocalizationId id ) const;

#if !ONYX_IS_RETAIL
    virtual void addSecondaryDatabase( const assets::AssetHandle< GetTextLocalizationDatabase >& database );
    virtual void removeSecondaryDatabase( const assets::AssetHandle< GetTextLocalizationDatabase >& database );
#endif

  private:
    LocalizationSettings m_settings;
    UniquePtr< ILocalizationBackend > m_localizationBackend;
};
} // namespace onyx::localization

namespace onyx {
template <>
struct Serialization< localization::LocalizationSettings > {
    static bool serialize( Serializer& serializer, const localization::LocalizationSettings& localizationSettings );
    static bool deserialize( const Deserializer& deserializer,
                             localization::LocalizationSettings& outLocalizationSettings );
};
} // namespace onyx
