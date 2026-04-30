#pragma once

#if ONYX_USE_IMGUI

#include <onyx/ui/windows/statusbaroverlay.h>

namespace onyx::ui {
class FpsStatusBarItem : public StatusBarOverlayItem {
  public:
    StringView getName() override { return "Fps"; }
    StringView getValue() override { return m_value; }

    void update( uint64_t deltaTime );

  private:
    float64 m_averageFrameTime = 0.0;
    String m_value;
};
} // namespace onyx::ui
#endif
