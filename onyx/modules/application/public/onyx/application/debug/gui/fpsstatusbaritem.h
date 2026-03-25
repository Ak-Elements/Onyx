#pragma once

#if ONYX_UI_MODULE && ONYX_USE_IMGUI

#include <onyx/application/debug/gui/statusbaroverlay.h>

namespace onyx::application::debug {
class FpsStatusBarItem : public StatusBarOverlayItem {
  public:
    StringView getName() override { return "Fps"; }
    StringView getValue() override { return m_value; }

    void update( uint64_t deltaTime );

  private:
    float64 m_averageFrameTime = 0.0;
    String m_value;
};
} // namespace onyx::application::debug
#endif