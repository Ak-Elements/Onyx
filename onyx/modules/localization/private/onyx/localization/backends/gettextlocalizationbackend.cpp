#include <onyx/localization/backends/gettextlocalizationbackend.h>

#include <onyx/assets/assetsystem.h>

#include <onyx/localization/localizationmodule.h>
#include <onyx/localization/localizedstring.h>

namespace Onyx::Localization
{
    namespace
    {
        

    }
    void GetTextLocalizationBackend::Init(Assets::AssetSystem& assetSystem, const LocalizationSettings& localizationSettings)
    {
        assetSystem.GetAsset(localizationSettings.m_Localization, m_MainDatabase);
    }

    Optional<StringView> GetTextLocalizationBackend::GetLocalized(LocalizationId id) const
    {
#if ONYX_IS_RETAIL
        Optional<StringView> localization = GetLocalized(id, *m_MainDatabase);
        return localization;
#else
        Optional<StringView> localization;
        if (IsInitialized())
        {
            localization = GetLocalized(id, *m_MainDatabase);
        }
        else
        {
            // TODO: Add unique log
            //ONYX_LOG_WARNING("Missing main localization file for project.");
        }

        if (localization.has_value() == false)
        {
            for (const auto& secondaryDb : m_SecondaryDatabases)
            {
                if ((secondaryDb.IsValid() == false) || (secondaryDb->IsLoaded() == false))
                {
                    continue;
                }

                localization = GetLocalized(id, secondaryDb);
                if (localization.has_value())
                {
                    break;
                }
            }
        }

        return localization;
#endif
    }

    Optional<StringView> GetTextLocalizationBackend::GetLocalized(LocalizationId id, onyxS32 count) const
    {
#if ONYX_IS_RETAIL
        Optional<StringView> localization = GetLocalized(id, count, *m_MainDatabase);
        return localization;
#else
        Optional<StringView> localization;
        if (IsInitialized())
        {
            localization = GetLocalized(id, count, *m_MainDatabase);
        }
        else
        {
            // TODO: Add unique log
            //ONYX_LOG_WARNING("Missing main localization file for project.");
        }

        if (localization.has_value() == false)
        {
            for (const auto& secondaryDb : m_SecondaryDatabases)
            {
                if ((secondaryDb.IsValid() == false) || (secondaryDb->IsLoaded() == false))
                {
                    continue;
                }

                localization = GetLocalized(id, count, *secondaryDb);
                if (localization.has_value())
                {
                    break;
                }
            }
        }

        return localization;
#endif

    }

    Optional<StringView> GetTextLocalizationBackend::GetLocalized(LocalizationId id, const GetTextLocalizationDatabase& database) const
    {
        const HashMap<LocalizationId, DynamicArray<String>>& databaseMap = database.GetDatabase();
        auto it = databaseMap.find(id);
        if (it == databaseMap.end())
        {
            return std::nullopt;
        }

        return it->second.front();
    }

    Optional<StringView> GetTextLocalizationBackend::GetLocalized(LocalizationId id, onyxS32 count, const GetTextLocalizationDatabase& database) const
    {
        const HashMap<LocalizationId, DynamicArray<String>>& databaseMap = database.GetDatabase();
        auto it = databaseMap.find(id);
        if (it == databaseMap.end())
        {
            return std::nullopt;
        }

        onyxS32 index = database.GetPluralFunction()(count);
        const DynamicArray<String>& localizedTexts = it->second;
        if (index > static_cast<onyxS32>(localizedTexts.size()))
        {
            return std::nullopt;
        }

        return localizedTexts[index];
    }

#if !ONYX_IS_RETAIL
    void GetTextLocalizationBackend::AddSecondaryDatabase(const Reference<GetTextLocalizationDatabase>& database)
    {
        if (m_MainDatabase.IsValid() && (database->GetId() == m_MainDatabase->GetId()))
        {
            return;
        }

        auto it = std::ranges::find_if(m_SecondaryDatabases, [&](const Reference<Localization::GetTextLocalizationDatabase>& secondaryDb)
        {
            return secondaryDb->GetId() == database->GetId();
        });

        if (it != m_SecondaryDatabases.end())
        {
            return;
        }

        m_SecondaryDatabases.push_back(Reference<GetTextLocalizationDatabase>(database));
    }

    void GetTextLocalizationBackend::RemoveSecondaryDatabase(const Reference<GetTextLocalizationDatabase>& database)
    {
        std::erase_if(m_SecondaryDatabases, [&](const Reference<Localization::GetTextLocalizationDatabase>& secondaryDb)
        {
            return secondaryDb->GetId() == database->GetId();
        });
    }
#endif
}
