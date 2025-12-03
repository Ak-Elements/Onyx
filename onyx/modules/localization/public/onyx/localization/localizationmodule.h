#pragma once
#include <onyx/engine/enginesystem.h>
#include <onyx/assets/assetid.h>

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
        Assets::AssetId Database;
    };

    class LocalizationModule : public IEngineSystem
    {
        friend struct Serialization<LocalizationModule>;

    public:
        static constexpr StringId32 TypeId = "Onyx::Localization::LocalizationModule";
        StringId32 GetTypeId() const override { return TypeId; }

        LocalizationModule(const LocalizationSettings& settings, Assets::AssetSystem& assetSystem);

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

namespace Onyx
{
    template <>
    struct Serialization<Localization::LocalizationSettings>
    {
        static bool Serialize(Serializer& serializer, const Localization::LocalizationSettings& localizationSettings);
        static bool Deserialize(const Deserializer& deserializer, Localization::LocalizationSettings& outLocalizationSettings);
    };
}
