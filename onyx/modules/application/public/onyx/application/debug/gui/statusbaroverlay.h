#pragma once

#if ONYX_UI_MODULE
#include <onyx/ui/imguiwindow.h>

namespace onyx::application::debug {
class StatusBarOverlayItem {
  public:
    virtual ~StatusBarOverlayItem() = default;
    virtual StringView getValue() = 0;
    virtual StringView getName() = 0;
};

class StatusBarOverlay : public ui::ImGuiWindow {
  public:
    static constexpr StringView WindowId = "StatusBar";

    StringView getWindowId() override { return WindowId; }

    template < typename T, class... Types > T& addOverlay( Types&&... args ) {
        return static_cast< T& >( *m_items.emplace_back( makeUnique< T >( std::forward< Types >( args )... ) ) );
    }

  private:
    void onRender( ui::ImGuiSystem& imguiSystem ) override;

  private:
    DynamicArray< UniquePtr< StatusBarOverlayItem > > m_items;
    Vector2u32 m_position;
};
} // namespace onyx::application::debug

#endif
