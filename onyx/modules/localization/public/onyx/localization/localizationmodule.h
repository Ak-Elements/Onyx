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
    StringId32 getTypeId() const override { return TypeId; }

    LocalizationModule( const LocalizationSettings& settings, assets::AssetSystem& assetSystem );

    LocalizedString GetLocalized( LocalizationId id ) const;
    Optional< StringView > TryGetLocalized( LocalizationId id ) const;

#if !ONYX_IS_RETAIL
    virtual void AddSecondaryDatabase( const assets::AssetHandle< GetTextLocalizationDatabase >& database );
    virtual void RemoveSecondaryDatabase( const assets::AssetHandle< GetTextLocalizationDatabase >& database );
#endif

  private:
    LocalizationSettings m_Settings;
    UniquePtr< ILocalizationBackend > m_LocalizationBackend;
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
