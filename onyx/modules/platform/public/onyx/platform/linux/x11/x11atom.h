#pragma once

#if ONYX_IS_LINUX && ONYX_USE_X11

#include <xcb/xproto.h>
namespace onyx::platform::x11 {

inline xcb_intern_atom_reply_t* queryAtom( xcb_connection_t* conn, bool onlyIfExists, const char* str ) {
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom( conn, onlyIfExists, strlen( str ), str );
    return xcb_intern_atom_reply( conn, cookie, NULL );
}

} // namespace onyx::platform::x11

#endif