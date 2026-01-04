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
    Pointer::Pointer(Input& input, wl_pointer* pointer)
        : m_Input(&input)
        , m_Pointer(pointer)
    {
        static const struct wl_pointer_listener pointer_listener =
        { OnEnterSurface, OnLeaveSurface, OnMove, OnButton, OnAxis, };
        wl_pointer_add_listener(m_Pointer, &pointer_listener, this);
    }

    Pointer::~Pointer()
    {
        if (m_Pointer != nullptr)
        {
            wl_pointer_destroy(m_Pointer);
        }
    }

    /*static*/ void Pointer::OnEnterSurface(void* instance, wl_pointer* pointer, onyxU32 serial, wl_surface* surface, wl_fixed_t x, wl_fixed_t y)
    {
        ONYX_UNUSED(instance);
        ONYX_UNUSED(pointer);
        ONYX_UNUSED(serial);
        ONYX_UNUSED(x);
        ONYX_UNUSED(y);
    }

    /*static*/ void Pointer::OnLeaveSurface(void* instance, wl_pointer* pointer, onyxU32 serial, wl_surface* surface)
    {
        ONYX_UNUSED(instance);
        ONYX_UNUSED(pointer);
        ONYX_UNUSED(serial);
        ONYX_UNUSED(serial);
        ONYX_UNUSED(surface);
    }

    /*static*/ void WaylandPointer::OnMove(void* instance, wl_pointer* pointer, onyxU32 time, wl_fixed_t x, wl_fixed_t y)
    {
        Pointer& pointerInstance = *reinterpret_cast<Pointer*>(instance);
        ONYX_ASSERT(pointerInstance.m_Input != nullptr);

        Input& waylandInput = *pointerInstance.m_Input;
        PlatformContext& context = waylandInput.GetContext();
        Input::InputSystem& inputSystem = context.GetInputSystem();

        Input::MousePositionEvent event;
        event.Id = Input::InputEventType::MousePositionChanged;
        event.Position.X =  wl_fixed_to_int(x);
        event.Position.Y =  wl_fixed_to_int(y);

        inputSystem.AddEvent(event);
    }

    /*static*/ void Pointer::OnButton(void* instance, wl_pointer* /*pointer*/, onyxU32 /*serial*/, onyxU32 /*time*/, onyxU32 button, onyxU32 state)
    {
        Pointer& pointerInstance = *reinterpret_cast<Pointer*>(instance);
        ONYX_ASSERT(pointerInstance.m_Input != nullptr);

        Input& waylandInput = *pointerInstance.m_Input;
        PlatformContext& context = waylandInput.GetContext();
        Input::InputSystem& inputSystem = context.GetInputSystem();

        Input::MouseButtonEvent event;
        event.State = state == 0 ? Input::ButtonState::Up : Input::ButtonState::Down;
        event.Button = WaylandToMouseButton(button);

        inputSystem.AddEvent(event);

    }

    /*static*/ void Pointer::OnAxis(void* instance, wl_pointer* pointer, onyxU32 time, onyxU32 axis, wl_fixed_t value)
    {
        Pointer& pointerInstance = *reinterpret_cast<Pointer*>(instance);
        ONYX_ASSERT(pointerInstance.m_Input != nullptr);

        Input& waylandInput = *pointerInstance.m_Input;
        PlatformContext& context = waylandInput.GetContext();
        Input::InputSystem& inputSystem = context.GetInputSystem();

        Input::MouseAxisEvent event;
        event.Id = Input::InputEventType::MouseWheel;    
        // NOTE: 10 units of motion per mouse wheel step seems to be a common ratio
        event.Value = -wl_fixed_to_double(value) / 10.0f;
        inputSystem.AddEvent(event);
    }
}

#endif