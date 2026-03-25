#pragma once

#if ONYX_USE_IMGUI

namespace onyx::ui {
struct ScopedImGuiIndent {
    ScopedImGuiIndent();
    ScopedImGuiIndent( float32 indent );
    ~ScopedImGuiIndent();

  private:
    float32 m_Indent = 0.0f;
};
} // namespace onyx::ui

#endif