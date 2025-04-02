#include <onyx/application/debug/gui/fpsstatusbaritem.h>

#if ONYX_UI_MODULE && ONYX_USE_IMGUI

namespace Onyx::Application::Debug
{
    void FpsStatusBarItem::Update(onyxU64 deltaTime)
    {
        AverageFrameTime += (deltaTime - AverageFrameTime) * 0.03f;
        onyxU64 avgFps = static_cast<onyxU64>(1000.0 / AverageFrameTime);
        Value = Format::Format("{}({:.2} ms)", avgFps, AverageFrameTime);
    }
}
#endif
