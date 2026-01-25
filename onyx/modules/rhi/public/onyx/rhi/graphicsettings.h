#pragma once

#include <onyx/rhi/graphicstypes.h>

namespace Onyx::Graphics
{
    struct GraphicSettings
    {
        ApiType Api = ApiType::Vulkan;
        Assets::AssetId DefaultRenderGraph{ "engine:/rendergraphs/default.orendergraph" };

        onyxU16 RefreshRate = 60;

        bool IsBindless = true;
        bool IsDynamicRenderingEnabled = true;

#if !ONYX_IS_RETAIL
        bool IsTimeSamplingEnabled = false;
        bool IsDebugEnabled = false;
        bool IsShaderDebugEnabled = false;
#endif
    };
}