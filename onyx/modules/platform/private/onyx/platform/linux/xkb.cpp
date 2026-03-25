#include <onyx/platform/linux/xkb.h>

#if ONYX_IS_LINUX

#include <xkbcommon/xkbcommon.h>

namespace onyx::platform::linux {

void Xkb::setState( xkb_state* state ) {
    ::xkb_state_unref( m_state );
    m_state = state;
}

void Xkb::setKeymap( xkb_keymap* keymap ) {
    ::xkb_keymap_unref( m_keymap );
    m_keymap = keymap;
}

} // namespace onyx::platform::linux

#endif