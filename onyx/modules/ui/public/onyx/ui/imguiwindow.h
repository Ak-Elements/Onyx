#pragma once

#include "onyx/defines.h"
#include <onyx/engine/enginesystem.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <onyx/ui/controls/dockspace.h>

#include <utility>

namespace onyx::ui {
class ImGuiSystem;

enum class WindowPosition {
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

class ImGuiWindow {
  public:
    virtual ~ImGuiWindow() = default;

    void open();
    void close();

    void render( ImGuiSystem& imguiSystem );

    ONYX_NO_DISCARD virtual constexpr StringId32 getWindowCategory() { return "Default"; }

    void setWindowId( String windowId ) { m_id = std::move( windowId ); }
    virtual StringView getWindowId() { return m_id; }

    void setName( const String& newName ) { m_name = newName; }
    void setWindowClass( ImGuiWindowClass* windowClass ) { m_windowClass = windowClass; }

    void setEngine( IEngine& engine ) { m_engine = &engine; }
    void setParent( ImGuiWindow& parent ) { m_parent = &parent; }

    ONYX_NO_DISCARD bool isOpen() const { return enums::all( m_state, State::Open ); }
    ONYX_NO_DISCARD bool isCollapsed() const { return m_isCollapsed; }
    ONYX_NO_DISCARD bool isDocked() const { return enums::all( m_state, State::Docked ); }
    ONYX_NO_DISCARD bool isFocused() const { return m_isFocused; }

    void setIsCollapsed( bool isCollapsed );
    void setDockId( uint32_t dockId ) { m_dockId = dockId; }

    void bringToFront();

  protected:
    enum class State : uint8_t { Closed = 0, Opening = 1 << 0, Open = 1 << 1, Closing = 1 << 2, Docked = 1 << 3 };

    ONYX_NO_DISCARD bool beginMenuBar() const;
    void endMenuBar() const;

    void setWindowFlags( ImGuiWindowFlags newFlags ) { m_flags = newFlags; }
    ONYX_NO_DISCARD ImGuiWindowFlags getWindowFlags() const { return m_flags; }

    void createDockspace( uint32_t id, const ImGuiWindowClass* windowClass, const DynamicArray< DockSplit >& splits );

    void setDefaultPosition( Vector2s32 position );
    void setDefaultPosition( Vector2s32 position, Vector2f32 pivot );
    void setDefaultPosition( WindowPosition position );

    void setDefaultSize( Vector2s32 size );

    ONYX_NO_DISCARD const ImGuiWindowClass& getWindowClass() const { return *m_windowClass; }

    template < typename T >
    T& getEngineSystem() {
        return m_engine->getSystem< T >();
    }

    template < typename T >
    const T& getEngineSystem() const {
        return m_engine->getSystem< T >();
    }

    template < typename T >
    Optional< T* > getParent() {
        if( ImGuiWindow* parent = m_parent.value_or( nullptr ) ) {
            return static_cast< T* >( parent );
        }

        return std::nullopt;
    }

  private:
    bool begin();
    void end();

    virtual void onOpen();
    virtual void onRender( ImGuiSystem& imguiSystem ) = 0;
    virtual void onClose();

    virtual void onRenderMainMenuBar();

  private:
    String m_id;
    String m_name;

    Dockspace m_dockspace;
    uint32_t m_dockId = std::numeric_limits< uint32_t >::max();

    ImGuiWindowClass* m_windowClass = nullptr;
    ImGuiWindowFlags m_flags = ImGuiWindowFlags_None;

    Optional< ImGuiWindow* > m_parent;
    IEngine* m_engine = nullptr;

    State m_state = State::Closed;

    bool m_isCollapsed = false;
    bool m_isFocused = false;
};
} // namespace onyx::ui
