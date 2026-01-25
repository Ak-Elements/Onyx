#include <onyx/platform/linux/wayland/waylandpointer.h>

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

#include <onyx/input/mouse.h>
#include <onyx/input/inputevent.h>
#include <onyx/input/inputsystem.h>
#include <onyx/platform/linux/wayland/waylandinput.h>
#include <onyx/platform/linux/wayland/waylandplatformcontext.h>


#include <wayland-client.h>
#include <linux/input-event-codes.h>

namespace Onyx::Platform::Wayland
{
    namespace 
    {
        Onyx::Input::MouseButton WaylandToMouseButton(onyxU32 button)
        {
            constexpr onyxU16 leftMouseButton = Enums::ToIntegral(Input::MouseButton::Button_1);
            constexpr onyxU16 first = Enums::ToIntegral(Onyx::Input::MouseButton::First);
            constexpr onyxU16 last = Enums::ToIntegral(Onyx::Input::MouseButton::Last);
            onyxU16 mouseButton = button - BTN_LEFT + leftMouseButton;
            ONYX_ASSERT(mouseButton > first);
            ONYX_ASSERT(mouseButton < last);

            // offset so we start from left mouse button
            return static_cast<Onyx::Input::MouseButton>(mouseButton);
        }
    
    }
    WaylandPointer::WaylandPointer(WaylandInput& input, wl_pointer* pointer)
        : m_Input(&input)
        , m_Pointer(pointer)
    {
        static const struct wl_pointer_listener pointer_listener =
        { OnEnterSurface, OnLeaveSurface, OnMove, OnButton, OnAxis, };
        wl_pointer_add_listener(m_Pointer, &pointer_listener, this);
    }

    WaylandPointer::~WaylandPointer()
    {
        if (m_Pointer != nullptr)
        {
            wl_pointer_destroy(m_Pointer);
        }
    }

    /*static*/ void WaylandPointer::OnEnterSurface(void* instance, wl_pointer* pointer, onyxU32 serial, wl_surface* surface, wl_fixed_t x, wl_fixed_t y)
    {
        ONYX_UNUSED(instance);
        ONYX_UNUSED(pointer);
        ONYX_UNUSED(serial);
        ONYX_UNUSED(x);
        ONYX_UNUSED(y);
    }

    /*static*/ void WaylandPointer::OnLeaveSurface(void* instance, wl_pointer* pointer, onyxU32 serial, wl_surface* surface)
    {
        ONYX_UNUSED(instance);
        ONYX_UNUSED(pointer);
        ONYX_UNUSED(serial);
        ONYX_UNUSED(serial);
        ONYX_UNUSED(surface);
    }

    /*static*/ void WaylandPointer::OnMove(void* instance, wl_pointer* pointer, onyxU32 time, wl_fixed_t x, wl_fixed_t y)
    {
        WaylandPointer& pointerInstance = *reinterpret_cast<WaylandPointer*>(instance);
        ONYX_ASSERT(pointerInstance.m_Input != nullptr);

        WaylandInput& waylandInput = *pointerInstance.m_Input;
        WaylandPlatformContext& context = waylandInput.GetContext();
        Input::InputSystem& inputSystem = context.GetInputSystem();

        Input::MousePositionEvent event;
        event.Position.X =  wl_fixed_to_int(x);
        event.Position.Y =  wl_fixed_to_int(y);

        inputSystem.AddEvent(event);
    }

    /*static*/ void WaylandPointer::OnButton(void* instance, wl_pointer* /*pointer*/, onyxU32 /*serial*/, onyxU32 /*time*/, onyxU32 button, onyxU32 state)
    {
        WaylandPointer& pointerInstance = *reinterpret_cast<WaylandPointer*>(instance);
        ONYX_ASSERT(pointerInstance.m_Input != nullptr);

        WaylandInput& waylandInput = *pointerInstance.m_Input;
        WaylandPlatformContext& context = waylandInput.GetContext();
        Input::InputSystem& inputSystem = context.GetInputSystem();

        Input::MouseButtonEvent event;
        event.State = state == 0 ? Input::ButtonState::Up : Input::ButtonState::Down;
        event.Button = WaylandToMouseButton(button);

        inputSystem.AddEvent(event);

    }

    /*static*/ void WaylandPointer::OnAxis(void* instance, wl_pointer* pointer, onyxU32 time, onyxU32 axis, wl_fixed_t value)
    {
        WaylandPointer& pointerInstance = *reinterpret_cast<WaylandPointer*>(instance);
        ONYX_ASSERT(pointerInstance.m_Input != nullptr);

        WaylandInput& waylandInput = *pointerInstance.m_Input;
        WaylandPlatformContext& context = waylandInput.GetContext();
        Input::InputSystem& inputSystem = context.GetInputSystem();

        Input::MouseAxisEvent event;
        // NOTE: 10 units of motion per mouse wheel step seems to be a common ratio
        event.Value = -wl_fixed_to_double(value) / 10.0f;
        inputSystem.AddEvent(event);
    }
}

#endif