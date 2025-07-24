#pragma once

#include <onyx/assets/asset.h>

namespace Onyx::Input
{
    struct InputModuleSettings
    {
        Assets::AssetId ActionsMap = "engine:/inputcontexts.oinput";
    };
}
