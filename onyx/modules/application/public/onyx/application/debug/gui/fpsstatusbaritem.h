#pragma once

#if ONYX_UI_MODULE && ONYX_USE_IMGUI

#include <onyx/application/debug/gui/statusbaroverlay.h>

namespace Onyx::Application::Debug
{
    class FpsStatusBarItem : public StatusBarOverlayItem
    {
    public:
        StringView GetName() override { return "Fps"; }
        StringView GetValue() override { return Value; }

        void Update(onyxU64 deltaTime);

    private:
        onyxF64 AverageFrameTime = 0.0;
        String Value;
    };
}
#endif