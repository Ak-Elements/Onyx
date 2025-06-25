#include <onyx/localization/localizationmodule.h>
#include <onyx/localization/backends/pobackend.h>

namespace Onyx::Localization
{
    void LocalizationModule::Init(Assets::AssetSystem& assetSystem, const LocalizationSettings& localizationSettings)
    {
        m_LocalizationBackend = MakeUnique<PoLocalizationBackend>();
        m_LocalizationBackend->Init(assetSystem, localizationSettings);
    }
}
