#include <onyx/platform/linux/wayland/waylandkeyboard.h>

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

#include <onyx/input/inputevent.h>
#include <onyx/input/inputsystem.h>
#include <onyx/platform/linux/linuxkeycodes.h>
#include <onyx/platform/linux/wayland/waylandinput.h>
#include <onyx/platform/linux/wayland/waylandplatformcontext.h>
#include <onyx/platform/linux/xkb.h>

#include <unistd.h>
#include <wayland-client.h>
#include <xkbcommon/xkbcommon-keysyms.h>
#include <xkbcommon/xkbcommon.h>

#include <xkbcommon/xkbcommon-compose.h>

#include <sys/mman.h>

namespace onyx::platform::wayland {
namespace {
xkb_keysym_t composeSymbol( linux::Xkb& xkb, xkb_keysym_t sym ) {
    if ( sym == XKB_KEY_NoSymbol || !xkb.ComposeState )
        return sym;
    if ( xkb_compose_state_feed( xkb.ComposeState, sym ) != XKB_COMPOSE_FEED_ACCEPTED )
        return sym;
    switch ( xkb_compose_state_get_status( xkb.ComposeState ) ) {
    case XKB_COMPOSE_COMPOSED:
        return xkb_compose_state_get_one_sym( xkb.ComposeState );
    case XKB_COMPOSE_COMPOSING:
    case XKB_COMPOSE_CANCELLED:
        return XKB_KEY_NoSymbol;
    case XKB_COMPOSE_NOTHING:
    default:
        return sym;
    }
}

uint32_t GetInputChar( linux::Xkb& xkb, uint32_t scancode ) {
    const xkb_keysym_t* keysyms;
    const xkb_keycode_t keycode = scancode + 8;

    if ( xkb_state_key_get_syms( xkb.getState(), keycode, &keysyms ) == 1 ) {
        const xkb_keysym_t keysym = composeSymbol( xkb, keysyms[ 0 ] );
        const uint32_t codepoint = ::xkb_keysym_to_utf32( keysym );
        if ( codepoint != 0 ) {
            // const int mods = _glfw.wl.xkb.modifiers;
            // const int plain = !(mods & (GLFW_MOD_CONTROL | GLFW_MOD_ALT));
            //_glfwInputChar(window, codepoint, mods, plain);
            return codepoint;
        }
    }

    return 0;
}
} // namespace

WaylandKeyboard::WaylandKeyboard( WaylandInput& input, wl_keyboard* keyboard )
    : m_Input( &input )
    , m_Keyboard( keyboard ) {
    static const struct wl_keyboard_listener keyboard_listener = {
        OnKeyMap,
        OnEnterSurface,
        OnLeaveSurface,
        OnKeyChange,
        OnModifierChange,
    };
    wl_keyboard_add_listener( m_Keyboard, &keyboard_listener, this );
}

WaylandKeyboard::~WaylandKeyboard() {
    if ( m_Keyboard != nullptr ) {
        wl_keyboard_destroy( m_Keyboard );
    }
}

/*static*/ void WaylandKeyboard::OnKeyMap( void* instance,
                                           wl_keyboard* keyboard,
                                           uint32_t format,
                                           int32_t fd,
                                           uint32_t size ) {
    WaylandKeyboard& keyboardInstance = *reinterpret_cast< WaylandKeyboard* >( instance );
    ONYX_ASSERT( keyboardInstance.m_Input != nullptr );

    WaylandInput& input = *keyboardInstance.m_Input;
    WaylandPlatformContext& context = input.GetContext();
    linux::Xkb& xkb = context.GetXkb();

    // inspired by glfw
    ::xkb_keymap* keymap;
    ::xkb_state* state;
    ::xkb_compose_table* composeTable;
    ::xkb_compose_state* composeState;

    char* mapStr;
    const char* locale;

    if ( format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1 ) {
        ::close( fd );
        return;
    }

    mapStr = reinterpret_cast< char* >( ::mmap( NULL, size, PROT_READ, MAP_SHARED, fd, 0 ) );
    if ( mapStr == MAP_FAILED ) {
        ::close( fd );
        return;
    }

    keymap = ::xkb_keymap_new_from_string( xkb.Context,
                                           mapStr,
                                           XKB_KEYMAP_FORMAT_TEXT_V1,
                                           XKB_KEYMAP_COMPILE_NO_FLAGS );
    ::munmap( mapStr, size );
    ::close( fd );

    ONYX_ASSERT( keymap != nullptr, "Wayland: Failed to compile keymap" );

    state = ::xkb_state_new( keymap );
    if ( !state ) {
        ::xkb_keymap_unref( keymap );
        ONYX_ASSERT( false, "Wayland: Failed to create XKB state" );
    }

    // Look up the preferred locale, falling back to "C" as default.
    locale = ::getenv( "LC_ALL" );
    if ( !locale )
        locale = ::getenv( "LC_CTYPE" );
    if ( !locale )
        locale = ::getenv( "LANG" );
    if ( !locale )
        locale = "C";

    composeTable = ::xkb_compose_table_new_from_locale( xkb.Context, locale, XKB_COMPOSE_COMPILE_NO_FLAGS );
    if ( composeTable ) {
        composeState = ::xkb_compose_state_new( composeTable, XKB_COMPOSE_STATE_NO_FLAGS );
        ::xkb_compose_table_unref( composeTable );
        if ( composeState )
            xkb.ComposeState = composeState;
        else
            ONYX_LOG_ERROR( "Wayland: Failed to create XKB compose state" );
    } else {
        ONYX_LOG_ERROR( "Wayland: Failed to create XKB compose table" );
    }
    xkb.setKeymap( keymap );
    xkb.setState( state );

    xkb.ControlIndex = ::xkb_keymap_mod_get_index( keymap, "Control" );
    xkb.AltIndex = ::xkb_keymap_mod_get_index( keymap, "Mod1" );
    xkb.ShiftIndex = ::xkb_keymap_mod_get_index( keymap, "Shift" );
    xkb.SuperIndex = ::xkb_keymap_mod_get_index( keymap, "Mod4" );
    xkb.CapsLockIndex = ::xkb_keymap_mod_get_index( keymap, "Lock" );
    xkb.NumLockIndex = ::xkb_keymap_mod_get_index( keymap, "Mod2" );
}

/*static*/ void WaylandKeyboard::OnEnterSurface( void* instance,
                                                 wl_keyboard* keyboard,
                                                 uint32_t serial,
                                                 wl_surface* surface,
                                                 wl_array* /*keys*/ ) {
    ONYX_UNUSED( instance );
    ONYX_UNUSED( keyboard );
    ONYX_UNUSED( serial );
    ONYX_UNUSED( surface );
}

/*static*/ void WaylandKeyboard::OnLeaveSurface( void* instance,
                                                 wl_keyboard* keyboard,
                                                 uint32_t serial,
                                                 wl_surface* surface ) {
    ONYX_UNUSED( instance );
    ONYX_UNUSED( keyboard );
    ONYX_UNUSED( serial );
    ONYX_UNUSED( surface );
}

/*static*/ void WaylandKeyboard::OnKeyChange( void* instance,
                                              wl_keyboard* keyboard,
                                              uint32_t /*serial*/,
                                              uint32_t /*time*/,
                                              uint32_t wayland_key,
                                              uint32_t state ) {
    WaylandKeyboard& keyboardInstance = *reinterpret_cast< WaylandKeyboard* >( instance );
    ONYX_ASSERT( keyboardInstance.m_Input != nullptr );

    WaylandInput& input = *keyboardInstance.m_Input;
    WaylandPlatformContext& context = input.GetContext();
    onyx::input::InputSystem& inputSystem = context.GetInputSystem();

    input::KeyboardEvent keyboardEvent;
    keyboardEvent.State = state == 0 ? input::ButtonState::Up : input::ButtonState::Down;
    keyboardEvent.Key = linux::convertKey( wayland_key );
    keyboardEvent.Char = GetInputChar( context.GetXkb(), wayland_key );

    inputSystem.AddEvent( keyboardEvent );
}

/*static*/ void WaylandKeyboard::OnModifierChange( void* instance,
                                                   wl_keyboard* /*keyboard*/,
                                                   uint32_t /*serial*/,
                                                   uint32_t modsDepressed,
                                                   uint32_t modsLatched,
                                                   uint32_t modsLocked,
                                                   uint32_t group ) {
    WaylandKeyboard& keyboardInstance = *reinterpret_cast< WaylandKeyboard* >( instance );
    ONYX_ASSERT( keyboardInstance.m_Input != nullptr );

    WaylandInput& input = *keyboardInstance.m_Input;
    WaylandPlatformContext& context = input.GetContext();

    linux::Xkb& xkb = context.GetXkb();
    ::xkb_state* xkbState = xkb.getState();

    ::xkb_state_update_mask( xkbState, modsDepressed, modsLatched, modsLocked, 0, 0, group );
}
} // namespace onyx::platform::wayland

#endif