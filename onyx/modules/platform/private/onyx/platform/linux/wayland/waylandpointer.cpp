#include <onyx/platform/linux/wayland/waylandpointer.h>

#include <onyx/input/mouse.h>
#include <onyx/input/inputevent.h>
#include <onyx/input/inputsystem.h>
#include <onyx/platform/linux/wayland/waylandinput.h>
#include <onyx/platform/linux/wayland/waylandplatformcontext.h>


#include <wayland-client.h>
#include <linux/input-event-codes.h>

namespace Onyx::Platform
{
    namespace 
    {
        Input::MouseButton WaylandToMouseButton(onyxU32 button)
        {
            switch (button)
            {
                case BTN_LEFT: return Input::MouseButton::Button_1;
                case BTN_RIGHT: return Input::MouseButton::Button_2;
                case BTN_MIDDLE: return Input::MouseButton::Button_3;
                case BTN_SIDE: return Input::MouseButton::Button_4;
                case BTN_EXTRA: return Input::MouseButton::Button_5;
                case BTN_FORWARD: return Input::MouseButton::Button_6;
                case BTN_BACK: return Input::MouseButton::Button_7;
                case BTN_TASK: return Input::MouseButton::Button_8;
            }
            return Input::MouseButton::Invalid;
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
        event.Id = Input::InputEventType::MousePositionChanged;
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
        event.Id = state == 0 ? Input::InputEventType::MouseButtonUp : Input::InputEventType::MouseButtonDown;
        event.Button = WaylandToMouseButton(button);
        event.IsPressed = state != 0;

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
        event.Id = Input::InputEventType::MouseWheel;    
        // NOTE: 10 units of motion per mouse wheel step seems to be a common ratio
        event.Value = -wl_fixed_to_double(value) / 10.0f;
        inputSystem.AddEvent(event);
    }
}