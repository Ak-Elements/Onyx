#pragma once

#if ONYX_IS_LINUX

struct xkb_context;
struct xkb_compose_state;
struct xkb_keymap;
struct xkb_state;

namespace onyx::platform::linux {

struct Xkb {
    xkb_context* Context = nullptr;
    xkb_compose_state* ComposeState = nullptr;

    uint32_t ControlIndex = 0;
    uint32_t AltIndex = 0;
    uint32_t ShiftIndex = 0;
    uint32_t SuperIndex = 0;
    uint32_t CapsLockIndex = 0;
    uint32_t NumLockIndex = 0;

    void setState( xkb_state* state );
    xkb_state* getState() { return m_state; }
    ONYX_NO_DISCARD const xkb_state* getState() const { return m_state; }

    void setKeymap( xkb_keymap* keymap );
    xkb_keymap* getKeymap() { return m_keymap; }
    ONYX_NO_DISCARD const xkb_keymap* getKeymap() const { return m_keymap; }

  private:
    xkb_keymap* m_keymap = nullptr;
    xkb_state* m_state = nullptr;
};

} // namespace onyx::platform::linux

#endif