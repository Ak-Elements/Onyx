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
    static constexpr uint8_t InputQueueCount = 2;

  public:
    using MouseAxisSignalT = Signal< void( const MouseAxisEvent& ) >;
    using MouseMoveSignalT = Signal< void( const MousePositionEvent& ) >;
    using MouseButtonSignalT = Signal< void( const MouseButtonEvent& ) >;

    using KeySignalT = Signal< void( const KeyboardEvent& ) >;

    using ControllerAxisSignalT = Signal< void( const GameControllerAxisEvent& ) >;
    using ControllerButtonSignalT = Signal< void( const GameControllerButtonEvent& ) >;

    static constexpr StringId32 TypeId{ "onyx::input::InputSystem" };
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

    Sink< MouseAxisSignalT > onMouseAxisChange() { return Sink< MouseAxisSignalT >( m_mouseAxisSignal ); }
    Sink< MouseButtonSignalT > onMouseButton() { return Sink< MouseButtonSignalT >( m_mouseButtonSignal ); }
    Sink< MouseMoveSignalT > onMousePositionChange() { return Sink< MouseMoveSignalT >( m_mousePositionSignal ); }

    Sink< KeySignalT > onKey() { return Sink< KeySignalT >( m_keySignal ); }

    Sink< ControllerAxisSignalT > onControllerAxisChange() {
        return Sink< ControllerAxisSignalT >( m_controllerAxisSignal );
    }
    Sink< ControllerButtonSignalT > onControllerButton() {
        return Sink< ControllerButtonSignalT >( m_controllerButtonSignal );
    }

    void update();

    void addEvent( MouseAxisEvent event ) { m_mouseAxisInputQueue[ m_currentQueueIndex ] = event; }
    void addEvent( MouseButtonEvent event ) { m_mouseButtonInputQueue[ m_currentQueueIndex ].emplace_back( event ); }
    void addEvent( MousePositionEvent event ) { m_mousePositionInputQueue[ m_currentQueueIndex ] = event; }
    void addEvent( KeyboardEvent event ) { m_keyboardInputQueue[ m_currentQueueIndex ].emplace_back( event ); }
    void addEvent( GameControllerAxisEvent event ) {
        m_controllerAxisInputQueue[ m_currentQueueIndex ].emplace_back( event );
    }
    void addEvent( GameControllerButtonEvent event ) {
        m_controllerButtonInputQueue[ m_currentQueueIndex ].emplace_back( event );
    }

    [[nodiscard]] int32_t getAxisValue1D( uint32_t deviceIndex, InputID id ) const;
    [[nodiscard]] Vector2s32 getAxisValue2D( uint32_t deviceIndex, InputID id ) const;
    [[nodiscard]] bool isButtonDown( InputID id ) const;
    [[nodiscard]] bool isButtonDown( MouseButton button ) const;
    [[nodiscard]] bool isButtonDown( Key key ) const;
    [[nodiscard]] bool isButtonDown( GameControllerButton button, uint8_t deviceIndex ) const;

    [[nodiscard]] const Vector2s32& getMousePosition() const { return m_mousePosition; }
    void setMousePosition( const Vector2s32& mousePos );

    [[nodiscard]] Vector2s32 getMouseDelta() const { return m_mouseDelta; }
    [[nodiscard]] int32_t getMouseWheelDelta() const { return m_mouseWheelDelta; }

    [[nodiscard]] int32_t getControllerAxisValue( uint32_t controllerIndex, GameControllerAxis axis ) const;

    void enableSystemMouseCapture( bool enable );

  private:
    void updateMouse( uint8_t queueIndex );
    void updateKeyboard( uint8_t queueIndex );
    void updateGameControllers( uint8_t queueIndex );

  private:
    MouseAxisSignalT m_mouseAxisSignal;
    MouseButtonSignalT m_mouseButtonSignal;
    MouseMoveSignalT m_mousePositionSignal;

    KeySignalT m_keySignal;

    ControllerAxisSignalT m_controllerAxisSignal;
    ControllerButtonSignalT m_controllerButtonSignal;

    // #if ONYX_IS_PC
    bool m_mouseButtonStates[ MouseButton_Count ] = { false };
    bool m_keyState[ Key_Count ] = { false };

    Vector2s32 m_mousePosition = { 0, 0 };
    Vector2s32 m_mouseDelta = { 0, 0 };
    Vector2s32 m_lastMousePosition = { 0, 0 };

    int16_t m_mouseScroll;
    int16_t m_mouseWheelDelta = 0;
    // #endif
    DynamicArray< GameController > m_gamepads;

    // Or maybe one queue with unqiue ptr?
    InplaceArray< Optional< MouseAxisEvent >, InputQueueCount > m_mouseAxisInputQueue;
    InplaceArray< DynamicArray< MouseButtonEvent >, InputQueueCount > m_mouseButtonInputQueue;
    InplaceArray< Optional< MousePositionEvent >, InputQueueCount > m_mousePositionInputQueue;
    InplaceArray< DynamicArray< KeyboardEvent >, InputQueueCount > m_keyboardInputQueue;
    InplaceArray< DynamicArray< GameControllerButtonEvent >, InputQueueCount > m_controllerButtonInputQueue;
    InplaceArray< DynamicArray< GameControllerAxisEvent >, InputQueueCount > m_controllerAxisInputQueue;

    uint8_t m_currentQueueIndex = 0;
};
} // namespace onyx::input
