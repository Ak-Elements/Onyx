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
onyx::input::MouseButton waylandToMouseButton( uint32_t button ) {
    constexpr uint16_t LeftMouseButton = enums::toIntegral( input::MouseButton::Button_1 );
    uint16_t mouseButton = button - BTN_LEFT + LeftMouseButton;
    ONYX_ASSERT( mouseButton > enums::toIntegral( onyx::input::MouseButton::First ) );
    ONYX_ASSERT( mouseButton < enums::toIntegral( onyx::input::MouseButton::Last ) );

    // offset so we start from left mouse button
    return static_cast< onyx::input::MouseButton >( mouseButton );
}

} // namespace
WaylandPointer::WaylandPointer( WaylandInput& input, wl_pointer* pointer )
    : m_input( &input )
    , m_pointer( pointer ) {
    static const struct wl_pointer_listener PointerListener = {
        onEnterSurface,
        onLeaveSurface,
        onMove,
        onButton,
        onAxis,
    };
    wl_pointer_add_listener( m_pointer, &PointerListener, this );
}

WaylandPointer::~WaylandPointer() {
    if( m_pointer != nullptr ) {
        wl_pointer_destroy( m_pointer );
    }
}

/*static*/ void WaylandPointer::onEnterSurface( [[maybe_unused]] void* instance,
                                                [[maybe_unused]] wl_pointer* pointer,
                                                [[maybe_unused]] uint32_t serial,
                                                [[maybe_unused]] wl_surface* surface,
                                                [[maybe_unused]] wl_fixed_t x,
                                                [[maybe_unused]] wl_fixed_t y ) {}

/*static*/ void WaylandPointer::onLeaveSurface( [[maybe_unused]] void* instance,
                                                [[maybe_unused]] wl_pointer* pointer,
                                                [[maybe_unused]] uint32_t serial,
                                                [[maybe_unused]] wl_surface* surface ) {}

/*static*/ void WaylandPointer::onMove( void* instance,
                                        wl_pointer* pointer,
                                        uint32_t time,
                                        wl_fixed_t x,
                                        wl_fixed_t y ) {
    WaylandPointer& pointerInstance = *reinterpret_cast< WaylandPointer* >( instance );
    ONYX_ASSERT( pointerInstance.m_input != nullptr );

    WaylandInput& waylandInput = *pointerInstance.m_input;
    WaylandPlatformContext& context = waylandInput.getContext();
    input::InputSystem& inputSystem = context.getInputSystem();

    input::MousePositionEvent event;
    event.Position.X = wl_fixed_to_int( x );
    event.Position.Y = wl_fixed_to_int( y );

    inputSystem.addEvent( event );
}

/*static*/ void WaylandPointer::onButton( void* instance,
                                          wl_pointer* /*pointer*/,
                                          uint32_t /*serial*/,
                                          uint32_t /*time*/,
                                          uint32_t button,
                                          uint32_t state ) {
    WaylandPointer& pointerInstance = *reinterpret_cast< WaylandPointer* >( instance );
    ONYX_ASSERT( pointerInstance.m_input != nullptr );

    WaylandInput& waylandInput = *pointerInstance.m_input;
    WaylandPlatformContext& context = waylandInput.getContext();
    input::InputSystem& inputSystem = context.getInputSystem();

    input::MouseButtonEvent event{};
    event.State = state == 0 ? input::ButtonState::Up : input::ButtonState::Down;
    event.Button = waylandToMouseButton( button );

    inputSystem.addEvent( event );
}

/*static*/ void WaylandPointer::onAxis( void* instance,
                                        wl_pointer* pointer,
                                        uint32_t time,
                                        uint32_t axis,
                                        wl_fixed_t value ) {
    WaylandPointer& pointerInstance = *reinterpret_cast< WaylandPointer* >( instance );
    ONYX_ASSERT( pointerInstance.m_input != nullptr );

    WaylandInput& waylandInput = *pointerInstance.m_input;
    WaylandPlatformContext& context = waylandInput.getContext();
    input::InputSystem& inputSystem = context.getInputSystem();

    input::MouseAxisEvent event{};
    // NOTE: 10 units of motion per mouse wheel step seems to be a common ratio
    event.Value = numericCast< int16_t >( -wl_fixed_to_double( value ) / 10.0f );
    inputSystem.addEvent( event );
}
} // namespace onyx::platform::wayland

#endif
