#pragma once

#if ONYX_IS_LINUX && ONYX_USE_X11

#include <onyx/thread/thread.h>

#include <onyx/platform/linux/xkb.h>

struct xcb_connection_t;
struct xcb_screen_t;
struct xcb_intern_atom_reply_t;

namespace onyx::input {
class InputSystem;
} // namespace onyx::input

namespace onyx::platform {

class PlatformSystem;

namespace x11 {

class X11PlatformContext : public Thread {
  public:
    explicit X11PlatformContext( PlatformSystem& platformSystem );

    input::InputSystem& getInputSystem();

    ONYX_NO_DISCARD xcb_connection_t* getConnectionHandle() {
        ONYX_ASSERT( m_connection != nullptr );
        return m_connection;
    }

    ONYX_NO_DISCARD const xcb_connection_t* getConnectionHandle() const {
        ONYX_ASSERT( m_connection != nullptr );
        return m_connection;
    }

    ONYX_NO_DISCARD xcb_screen_t& getScreen() { return *m_screen; }
    ONYX_NO_DISCARD const xcb_screen_t& getScreen() const { return *m_screen; }

    ONYX_NO_DISCARD xcb_intern_atom_reply_t& getAtomDelete() { return *m_atomWmDeleteWindow; }
    ONYX_NO_DISCARD const xcb_intern_atom_reply_t& getAtomDelete() const { return *m_atomWmDeleteWindow; }

    ONYX_NO_DISCARD uint8_t getXkbFirstEvent() const { return m_xkbFirstEvent; }

    PlatformSystem& getPlatformSystem() { return *m_platformSystem; }

  private:
    void onUpdate() override;

    bool initConnection();

  private:
    PlatformSystem* m_platformSystem = nullptr;

    linux::Xkb m_xkb;

    xcb_connection_t* m_connection = nullptr;
    xcb_screen_t* m_screen = nullptr;
    xcb_intern_atom_reply_t* m_atomWmDeleteWindow = nullptr;

    uint8_t m_xkbFirstEvent = std::numeric_limits< uint8_t >::max();
};

} // namespace x11

} // namespace onyx::platform

#endif