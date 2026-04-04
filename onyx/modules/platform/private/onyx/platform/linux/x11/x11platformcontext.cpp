#include <onyx/platform/linux/x11/x11platformcontext.h>

#if ONYX_IS_LINUX && ONYX_USE_X11

#include <onyx/platform/linux/linuxkeycodes.h>
#include <onyx/platform/linux/x11/x11atom.h>
#include <onyx/platform/platformsystem.h>

#include <onyx/input/inputevent.h>
#include <onyx/input/inputsystem.h>

#include <xcb/xcb.h>

#define explicit explicit_
#include <xcb/xkb.h>
#undef explicit

#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon.h>

namespace onyx::platform::x11 {

namespace {

constexpr uint8_t XcbButtonMouse4 = 8;
constexpr uint8_t XcbMouseButton5 = 9;
constexpr int16_t ScrollValue = 3;

input::MouseButton getMouseButton( xcb_button_t button ) {
    ONYX_ASSERT( button < XCB_BUTTON_INDEX_4 || button > XCB_BUTTON_INDEX_5 );

    switch( button ) {
    case XCB_BUTTON_INDEX_1:
        return input::MouseButton::Button_1; // left button
    case XCB_BUTTON_INDEX_2:
        return input::MouseButton::Button_3; // middle button
    case XCB_BUTTON_INDEX_3:
        return input::MouseButton::Button_2; // right button
    case XcbButtonMouse4:
        return input::MouseButton::Button_4;
    case XcbMouseButton5:
        return input::MouseButton::Button_5;
    }

    ONYX_ASSERT( false, "Unhandled mouse button" );
    return input::MouseButton::Button_1;
}

void initKeyboard( xcb_connection_t* connection, linux::Xkb& xkb ) {
    int32_t deviceId = ::xkb_x11_get_core_keyboard_device_id( connection );
    xkb_keymap* keymap = ::xkb_x11_keymap_new_from_device( xkb.Context,
                                                           connection,
                                                           deviceId,
                                                           XKB_KEYMAP_COMPILE_NO_FLAGS );
    xkb_state* state = ::xkb_x11_state_new_from_device( keymap, connection, deviceId );
    xkb.setKeymap( keymap );
    xkb.setState( state );
}

void handleXkbEvent( X11PlatformContext& context,
                     linux::Xkb& xkb,
                     input::InputSystem& inputSystem,
                     const xcb_generic_event_t& event ) {
    auto& xkbEvent = reinterpret_cast< const xcb_xkb_state_notify_event_t& >( event );
    switch( xkbEvent.xkbType ) {
    case XCB_XKB_EVENT_TYPE_NEW_KEYBOARD_NOTIFY: { // full rebuild only when keyboard device changes
        break;
    }
    case XCB_XKB_EVENT_TYPE_MAP_NOTIFY: {
        auto& xkbMapEvent = reinterpret_cast< const xcb_xkb_map_notify_event_t& >( event );
        if( ( xkbMapEvent.changed & ( XCB_XKB_MAP_PART_KEY_SYMS ) ) == 0 )
            break;

        initKeyboard( context.getConnectionHandle(), xkb );
        break;
    }
    case XCB_XKB_EVENT_TYPE_STATE_NOTIFY:
        ::xkb_state_update_mask( xkb.getState(),
                                 xkbEvent.baseMods,
                                 xkbEvent.latchedMods,
                                 xkbEvent.lockedMods,
                                 xkbEvent.baseGroup,
                                 xkbEvent.latchedGroup,
                                 xkbEvent.lockedGroup );
        break;
    }
}

void handleEvents( PlatformSystem& platformSystem,
                   X11PlatformContext& context,
                   linux::Xkb& xkb,
                   input::InputSystem& inputSystem,
                   const xcb_generic_event_t& event ) {
    const uint8_t eventType = event.response_type & 0x7f;
    if( eventType == context.getXkbFirstEvent() ) {
        handleXkbEvent( context, xkb, inputSystem, event );
        return;
    }

    switch( eventType ) {
    case XCB_CLIENT_MESSAGE: {
        auto& clientEvent = reinterpret_cast< const xcb_client_message_event_t& >( event );

        ONYX_LOG_INFO( "atom delete" );
        if( clientEvent.data.data32[ 0 ] == context.getAtomDelete().atom ) {
            Window& window = platformSystem.GetWindow( clientEvent.window );
            window.SetState( WindowState::Closed );
            // clientEvent.window; Look for window with that id and close it
            // window.Hide();
        }
        break;
    }
    case XCB_MOTION_NOTIFY: {
        auto& motionEvent = reinterpret_cast< const xcb_motion_notify_event_t& >( event );
        inputSystem.AddEvent( input::MousePositionEvent( { motionEvent.event_x, motionEvent.event_y } ) );
        break;
    }
    case XCB_BUTTON_PRESS: {
        auto& buttonEvent = reinterpret_cast< const xcb_button_press_event_t& >( event );
        if( ( buttonEvent.detail == XCB_BUTTON_INDEX_4 ) || ( buttonEvent.detail == XCB_BUTTON_INDEX_5 ) ) {
            int16_t scrollDirection = ( buttonEvent.detail == XCB_BUTTON_INDEX_4 ) ? int16_t( 1 ) : int16_t( -1 );
            input::MouseAxisEvent event{ static_cast< int16_t >( ScrollValue * scrollDirection ) };
            inputSystem.AddEvent( event );
        } else {
            input::MouseButtonEvent event{ getMouseButton( buttonEvent.detail ), input::ButtonState::Down };
            inputSystem.AddEvent( event );
        }
        break;
    }
    case XCB_BUTTON_RELEASE: {
        auto& buttonEvent = reinterpret_cast< const xcb_button_press_event_t& >( event );
        if( ( buttonEvent.detail == XCB_BUTTON_INDEX_4 ) || ( buttonEvent.detail == XCB_BUTTON_INDEX_5 ) ) {
            break;
        }
        input::MouseButtonEvent event{ getMouseButton( buttonEvent.detail ), input::ButtonState::Up };
        inputSystem.AddEvent( event );
        break;
    }
    case XCB_KEY_PRESS:
    case XCB_KEY_RELEASE: {
        if( xkb.getState() == nullptr )
            break;

        auto& keyEvent = reinterpret_cast< const xcb_key_press_event_t& >( event );

        const bool isDown = eventType == XCB_KEY_PRESS;
        ::xkb_state_update_key( xkb.getState(), keyEvent.detail, isDown ? XKB_KEY_DOWN : XKB_KEY_UP );

        input::KeyboardEvent keyboardEvent{};
        keyboardEvent.State = isDown ? input::ButtonState::Down : input::ButtonState::Up;
        keyboardEvent.Key = linux::convertKey( keyEvent.detail - linux::XcbKeyOffset );
        keyboardEvent.Char = ::xkb_state_key_get_utf32( xkb.getState(), keyEvent.detail );
        inputSystem.AddEvent( keyboardEvent );
        break;
    }
    case XCB_DESTROY_NOTIFY: {
        ONYX_LOG_INFO( "destroy" );
        break;
    }
    case XCB_CONFIGURE_NOTIFY: {
        auto& configureEvent = reinterpret_cast< const xcb_configure_notify_event_t& >( event );
        Window& window = platformSystem.GetWindow( configureEvent.window );
        window.SetSize( configureEvent.width, configureEvent.height );
        break;
    }
    default:
        break;
    }
}
} // namespace

X11PlatformContext::X11PlatformContext( PlatformSystem& platformSystem )
    : m_platformSystem( &platformSystem ) {
    if( initConnection() ) {
        /* Magic code that will send notification when window is destroyed */
        m_atomWmDeleteWindow = queryAtom( m_connection, false, "WM_DELETE_WINDOW" );

        start();
    }
}

input::InputSystem& PlatformContext::getInputSystem() {
    ONYX_ASSERT( m_platformSystem != nullptr );
    return m_platformSystem->GetInputSystem();
}

void X11PlatformContext::onUpdate() {
    while( isRunning() ) {
        xcb_generic_event_t* event;
        while( ( event = xcb_poll_for_event( m_connection ) ) ) {
            handleEvents( *m_platformSystem, *this, m_xkb, getInputSystem(), *event );
            free( event );
        }
    }
}

bool X11PlatformContext::initConnection() {
    const xcb_setup_t* setup;
    xcb_screen_iterator_t iter;
    int scr;

    m_connection = xcb_connect( nullptr, &scr );
    ONYX_ASSERT( m_connection != nullptr );
    if( xcb_connection_has_error( m_connection ) ) {
        ONYX_LOG_ERROR( "Could not create xcb connection." );
        return false;
    }

    setup = xcb_get_setup( m_connection );
    iter = xcb_setup_roots_iterator( setup );
    while( scr-- > 0 )
        xcb_screen_next( &iter );
    m_screen = iter.data;

    // Must call this first
    m_xkb.Context = ::xkb_context_new( XKB_CONTEXT_NO_FLAGS );
    ::xcb_xkb_use_extension( m_connection, XKB_X11_MIN_MAJOR_XKB_VERSION, XKB_X11_MIN_MINOR_XKB_VERSION );

    const xcb_query_extension_reply_t* reply = xcb_get_extension_data( m_connection, &::xcb_xkb_id );
    m_xkbFirstEvent = reply->first_event;

    uint16_t affectMask = XCB_XKB_EVENT_TYPE_NEW_KEYBOARD_NOTIFY | // device/keymap replaced
                          XCB_XKB_EVENT_TYPE_MAP_NOTIFY |          // keymap
                          XCB_XKB_EVENT_TYPE_STATE_NOTIFY;         // modifier state changed

    uint16_t selectMask = XCB_XKB_EVENT_TYPE_NEW_KEYBOARD_NOTIFY | XCB_XKB_EVENT_TYPE_MAP_NOTIFY |
                          XCB_XKB_EVENT_TYPE_STATE_NOTIFY;

    // clang-format off
    ::xcb_xkb_select_events_checked( m_connection,
                                     XCB_XKB_ID_USE_CORE_KBD,
                                     affectMask,
                                     0,
                                     selectMask,
                                     0,
                                     0,
                                     nullptr );
    //clang-format on

    initKeyboard( m_connection, m_xkb );

    return true;
}

} // namespace onyx::platform::x11

#endif
