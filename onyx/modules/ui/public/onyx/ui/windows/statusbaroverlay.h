#pragma once

#if ONYX_UI_MODULE
#include <onyx/ui/imguiwindow.h>

namespace onyx::ui {
class StatusBarOverlayItem {
  public:
    virtual ~StatusBarOverlayItem() = default;
    virtual StringView getValue() = 0;
    virtual StringView getName() = 0;
};

class StatusBarOverlay : public ImGuiWindow {
  public:
    static constexpr StringView WindowId = "StatusBar";
    static constexpr StringView WindowCategory = "Overlay";

    StringView getWindowId() override { return WindowId; }

    template < typename T, class... Types >
    T& addOverlay( Types&&... args ) {
        return static_cast< T& >( *m_items.emplace_back( makeUnique< T >( std::forward< Types >( args )... ) ) );
    }

  private:
    void onOpen() override;
    void onRender( ui::ImGuiSystem& imguiSystem ) override;

  private:
    DynamicArray< UniquePtr< StatusBarOverlayItem > > m_items;
    Vector2u32 m_position;
};
} // namespace onyx::ui

#endif
