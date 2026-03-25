#pragma once

#include <onyx/rhi/lighting/lighting.h>
#include <onyx/rhi/viewconstants.h>

namespace onyx::rhi {
class GraphicsSystem;

struct FrameData {
    virtual ~FrameData() = default;
};

struct FrameContext {
    // TODO: Remove and pass as & parameter to functions along with the context
    GraphicsSystem* Api;

    ViewConstants ViewConstants;
    Lighting Lighting;

    float32 TimeOfDay = 15.0f;
    uint32_t TonemapFunctor = 5;

    uint64_t ComputeFrame = 0;
    uint64_t AbsoluteFrame = 0;
    uint8_t FrameIndex = 0;

    UniquePtr< FrameData > FrameData;
};
} // namespace onyx::rhi
