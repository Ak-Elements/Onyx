#pragma once

#include <onyx/engine/enginesystem.h>

#include <onyx/input/gamecontroller.h>
#include <onyx/input/inputid.h>
#include <onyx/input/keycodes.h>
#include <onyx/input/mouse.h>

#include <onyx/eventhandler.h>
#include <onyx/input/inputevent.h>

namespace onyx {
class Engine;
}

namespace onyx::platform {
class PlatformSystem;
}

namespace onyx::input {
struct InputEvent;

class InputSystem : public IEngineSystem {
    static constexpr uint8_t INPUT_QUEUE_COUNT = 2;

  public:
    using MouseAxisSignalT = Signal< void( const MouseAxisEvent& ) >;
    using MouseMoveSignalT = Signal< void( const MousePositionEvent& ) >;
    using MouseButtonSignalT = Signal< void( const MouseButtonEvent& ) >;

    using KeySignalT = Signal< void( const KeyboardEvent& ) >;

    using ControllerAxisSignalT = Signal< void( const GameControllerAxisEvent& ) >;
    using ControllerButtonSignalT = Signal< void( const GameControllerButtonEvent& ) >;

    static constexpr StringId32 TypeId{ "onyx::input::InputSystem" };
    StringId32 getTypeId() const override { return TypeId; }

    Sink< MouseAxisSignalT > OnMouseAxisChange() { return Sink< MouseAxisSignalT >( m_MouseAxisSignal ); }
    Sink< MouseButtonSignalT > OnMouseButton() { return Sink< MouseButtonSignalT >( m_MouseButtonSignal ); }
    Sink< MouseMoveSignalT > OnMousePositionChange() { return Sink< MouseMoveSignalT >( m_MousePositionSignal ); }

    Sink< KeySignalT > OnKey() { return Sink< KeySignalT >( m_KeySignal ); }

    Sink< ControllerAxisSignalT > OnControllerAxisChange() {
        return Sink< ControllerAxisSignalT >( m_ControllerAxisSignal );
    }
    Sink< ControllerButtonSignalT > OnControllerButton() {
        return Sink< ControllerButtonSignalT >( m_ControllerButtonSignal );
    }

    void update();

    void AddEvent( MouseAxisEvent event ) { m_MouseAxisInputQueue[ m_CurrentQueueIndex ] = event; }
    void AddEvent( MouseButtonEvent event ) { m_MouseButtonInputQueue[ m_CurrentQueueIndex ].emplace_back( event ); }
    void AddEvent( MousePositionEvent event ) { m_MousePositionInputQueue[ m_CurrentQueueIndex ] = event; }
    void AddEvent( KeyboardEvent event ) { m_KeyboardInputQueue[ m_CurrentQueueIndex ].emplace_back( event ); }
    void AddEvent( GameControllerAxisEvent event ) {
        m_ControllerAxisInputQueue[ m_CurrentQueueIndex ].emplace_back( event );
    }
    void AddEvent( GameControllerButtonEvent event ) {
        m_ControllerButtonInputQueue[ m_CurrentQueueIndex ].emplace_back( event );
    }

    int32_t GetAxisValue1D( uint32_t deviceIndex, InputID id ) const;
    Vector2s32 GetAxisValue2D( uint32_t deviceIndex, InputID id ) const;
    bool IsButtonDown( InputID id ) const;
    bool IsButtonDown( MouseButton button ) const;
    bool IsButtonDown( Key key ) const;
    bool IsButtonDown( GameControllerButton button, uint8_t deviceIndex ) const;

    const Vector2s32& GetMousePosition() const { return m_MousePosition; }
    void SetMousePosition( const Vector2s32& mousePos );

    Vector2s32 GetMouseDelta() const { return m_MouseDelta; }
    int32_t GetMouseWheelDelta() const { return m_MouseWheelDelta; }

    int32_t GetControllerAxisValue( uint32_t controllerIndex, GameControllerAxis axis ) const;

    void EnableSystemMouseCapture( bool enable );

  private:
    void UpdateMouse( uint8_t queueIndex );
    void UpdateKeyboard( uint8_t queueIndex );
    void UpdateGameControllers( uint8_t queueIndex );

  private:
    MouseAxisSignalT m_MouseAxisSignal;
    MouseButtonSignalT m_MouseButtonSignal;
    MouseMoveSignalT m_MousePositionSignal;

    KeySignalT m_KeySignal;

    ControllerAxisSignalT m_ControllerAxisSignal;
    ControllerButtonSignalT m_ControllerButtonSignal;

    // #if ONYX_IS_PC
    bool m_MouseButtonStates[ MouseButton_Count ] = { false };
    bool m_KeyState[ Key_Count ] = { false };

    Vector2s32 m_MousePosition = { 0, 0 };
    Vector2s32 m_MouseDelta = { 0, 0 };
    Vector2s32 m_LastMousePosition = { 0, 0 };

    int16_t m_MouseScroll;
    int16_t m_MouseWheelDelta = 0;
    // #endif
    DynamicArray< GameController > m_Gamepads;

    // Or maybe one queue with unqiue ptr?
    InplaceArray< Optional< MouseAxisEvent >, INPUT_QUEUE_COUNT > m_MouseAxisInputQueue;
    InplaceArray< DynamicArray< MouseButtonEvent >, INPUT_QUEUE_COUNT > m_MouseButtonInputQueue;
    InplaceArray< Optional< MousePositionEvent >, INPUT_QUEUE_COUNT > m_MousePositionInputQueue;
    InplaceArray< DynamicArray< KeyboardEvent >, INPUT_QUEUE_COUNT > m_KeyboardInputQueue;
    InplaceArray< DynamicArray< GameControllerButtonEvent >, INPUT_QUEUE_COUNT > m_ControllerButtonInputQueue;
    InplaceArray< DynamicArray< GameControllerAxisEvent >, INPUT_QUEUE_COUNT > m_ControllerAxisInputQueue;

    uint8_t m_CurrentQueueIndex = 0;
};
} // namespace onyx::input
