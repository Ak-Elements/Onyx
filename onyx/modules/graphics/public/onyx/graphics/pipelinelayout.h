#pragma once

#if ONYX_USE_VULKAN
#include <onyx/graphics/vulkan/pipelinelayout.h>
namespace Onyx::Graphics
{
    using PipelineLayout = Vulkan::PipelineLayout;
}
#else
static_assert("Missing implementation");
#endif