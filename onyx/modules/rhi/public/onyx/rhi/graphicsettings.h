#pragma once

#include <onyx/rhi/graphicstypes.h>

namespace onyx::rhi
{
    struct GraphicSettings
    {
        ApiType Api = ApiType::Vulkan;
        assets::AssetId DefaultRenderGraph{ "engine:/rendergraphs/default.orendergraph" };

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