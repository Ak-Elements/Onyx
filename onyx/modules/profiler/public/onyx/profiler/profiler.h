#pragma once

#include <tracy/Tracy.hpp>


#define ONYX_PROFILE_CONCAT_IMPL(a, b) a##b
#define ONYX_PROFILE_CONCAT(a, b) ONYX_PROFILE_CONCAT_IMPL(a, b)

#define ONYX_PROFILE_CREATE_TAG(name, color) \
    constexpr const char* const ONYX_PROFILE_CONCAT(g_ProfilerTag, name) = #name; \
    constexpr const Onyx::onyxU32 ONYX_PROFILE_CONCAT(g_ProfilerColor, name) = color;

#define ONYX_PROFILE(name) \
    ZoneNamedNC(ONYX_PROFILE_CONCAT(tracy_scope_, name), \
                ONYX_PROFILE_CONCAT(g_ProfilerTag, name), \
                ONYX_PROFILE_CONCAT(g_ProfilerColor, name), \
                true)

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
