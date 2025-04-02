#pragma once

#include <tracy/Tracy.hpp>


#define ONYX_PROFILE_CREATE_TAG(name, color) \
const char* const g_ProfilerTag##name## = #name;     \
const Onyx::onyxU32 g_ProfilerColor##name## = color;


#define ONYX_PROFILE(name) ZoneNamedNC(tracy_scope_##name##, g_ProfilerTag##name##, g_ProfilerColor##name##, true)
#define ONYX_PROFILE_FUNCTION ZoneScoped

#define ONYX_PROFILE_SECTION(name) ZoneScopedN(#name);
#define ONYX_PROFILE_MARK_FRAME(name) FrameMarkNamed(#name);
#define ONYX_PROFILE_MARK_FRAME_START(name) FrameMarkStart(name);
#define ONYX_PROFILE_MARK_FRAME_END(name) FrameMarkEnd(name);
#define ONYX_PROFILE_TAG(name) ZoneText(#name, strlen(#name));
#define ONYX_PROFILE_SET_THREAD(name) tracy::SetThreadName(#name);

namespace Onyx::Profiler
{
    struct Profiler
    {
    public:
        void Update();
    private:
    };
}
