#pragma once

#include <onyx/rhi/viewconstants.h>
#include <onyx/rhi/lighting/lighting.h>

namespace Onyx::Graphics
{
    class GraphicsSystem;

    struct FrameData { virtual ~FrameData() = default; };

    struct FrameContext
    {
        //TODO: Remove and pass as & parameter to functions along with the context
        GraphicsSystem* Api;

        ViewConstants ViewConstants;
        Lighting Lighting;

        onyxF32 TimeOfDay = 15.0f;
        onyxU32 TonemapFunctor = 5;

        onyxU64 ComputeFrame = 0;
        onyxU64 AbsoluteFrame = 0;
        onyxU8 FrameIndex = 0;

        UniquePtr<FrameData> FrameData;
    };
}
