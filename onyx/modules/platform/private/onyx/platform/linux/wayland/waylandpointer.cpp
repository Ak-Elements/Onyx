#include <onyx/platform/linux/wayland/waylandpointer.h>

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

#include <onyx/input/inputevent.h>
#include <onyx/input/inputsystem.h>
#include <onyx/input/mouse.h>
#include <onyx/platform/linux/wayland/waylandinput.h>
#include <onyx/platform/linux/wayland/waylandplatformcontext.h>

#include <linux/input-event-codes.h>
#include <wayland-client.h>

namespace onyx::platform::wayland {
namespace {
onyx::input::MouseButton WaylandToMouseButton( uint32_t button ) {
    constexpr uint16_t leftMouseButton = enums::toIntegral( input::MouseButton::Button_1 );
    uint16_t mouseButton = button - BTN_LEFT + leftMouseButton;
    ONYX_ASSERT( mouseButton > enums::toIntegral( onyx::input::MouseButton::First ) );
    ONYX_ASSERT( mouseButton < enums::toIntegral( onyx::input::MouseButton::Last ) );

    // offset so we start from left mouse button
    return static_cast< onyx::input::MouseButton >( mouseButton );
}

} // namespace
WaylandPointer::WaylandPointer( WaylandInput& input, wl_pointer* pointer )
    : m_Input( &input )
    , m_Pointer( pointer ) {
    static const struct wl_pointer_listener pointer_listener = {
        OnEnterSurface,
        OnLeaveSurface,
        OnMove,
        OnButton,
        OnAxis,
    };
    wl_pointer_add_listener( m_Pointer, &pointer_listener, this );
}

WaylandPointer::~WaylandPointer() {
    if ( m_Pointer != nullptr ) {
        wl_pointer_destroy( m_Pointer );
    }
}

/*static*/ void WaylandPointer::OnEnterSurface( void* instance,
                                                wl_pointer* pointer,
                                                uint32_t serial,
                                                wl_surface* surface,
                                                wl_fixed_t x,
                                                wl_fixed_t y ) {
    ONYX_UNUSED( instance );
    ONYX_UNUSED( pointer );
    ONYX_UNUSED( serial );
    ONYX_UNUSED( x );
    ONYX_UNUSED( y );
}

/*static*/ void WaylandPointer::OnLeaveSurface( void* instance,
                                                wl_pointer* pointer,
                                                uint32_t serial,
                                                wl_surface* surface ) {
    ONYX_UNUSED( instance );
    ONYX_UNUSED( pointer );
    ONYX_UNUSED( serial );
    ONYX_UNUSED( serial );
    ONYX_UNUSED( surface );
}

/*static*/ void WaylandPointer::OnMove( void* instance,
                                        wl_pointer* pointer,
                                        uint32_t time,
                                        wl_fixed_t x,
                                        wl_fixed_t y ) {
    WaylandPointer& pointerInstance = *reinterpret_cast< WaylandPointer* >( instance );
    ONYX_ASSERT( pointerInstance.m_Input != nullptr );

    WaylandInput& waylandInput = *pointerInstance.m_Input;
    WaylandPlatformContext& context = waylandInput.GetContext();
    input::InputSystem& inputSystem = context.GetInputSystem();

    input::MousePositionEvent event;
    event.Position.X = wl_fixed_to_int( x );
    event.Position.Y = wl_fixed_to_int( y );

    inputSystem.AddEvent( event );
}

/*static*/ void WaylandPointer::OnButton( void* instance,
                                          wl_pointer* /*pointer*/,
                                          uint32_t /*serial*/,
                                          uint32_t /*time*/,
                                          uint32_t button,
                                          uint32_t state ) {
    WaylandPointer& pointerInstance = *reinterpret_cast< WaylandPointer* >( instance );
    ONYX_ASSERT( pointerInstance.m_Input != nullptr );

    WaylandInput& waylandInput = *pointerInstance.m_Input;
    WaylandPlatformContext& context = waylandInput.GetContext();
    input::InputSystem& inputSystem = context.GetInputSystem();

    input::MouseButtonEvent event;
    event.State = state == 0 ? input::ButtonState::Up : input::ButtonState::Down;
    event.Button = WaylandToMouseButton( button );

    inputSystem.AddEvent( event );
}

/*static*/ void WaylandPointer::OnAxis( void* instance,
                                        wl_pointer* pointer,
                                        uint32_t time,
                                        uint32_t axis,
                                        wl_fixed_t value ) {
    WaylandPointer& pointerInstance = *reinterpret_cast< WaylandPointer* >( instance );
    ONYX_ASSERT( pointerInstance.m_Input != nullptr );

    WaylandInput& waylandInput = *pointerInstance.m_Input;
    WaylandPlatformContext& context = waylandInput.GetContext();
    input::InputSystem& inputSystem = context.GetInputSystem();

    input::MouseAxisEvent event;
    // NOTE: 10 units of motion per mouse wheel step seems to be a common ratio
    event.Value = -wl_fixed_to_double( value ) / 10.0f;
    inputSystem.AddEvent( event );
}
} // namespace onyx::platform::wayland

#endif