#include <onyx/input/inputsystem.h>

#include <onyx/input/inputevent.h>
#include <onyx/input/inputid.h>

namespace onyx::input {

void InputSystem::update() {
    uint8_t queueIndex = m_currentQueueIndex;
    m_currentQueueIndex = ( m_currentQueueIndex + 1 ) % InputQueueCount;

    // process input queues
    updateMouse( queueIndex );
    updateKeyboard( queueIndex );
    updateGameControllers( queueIndex );

    m_mouseDelta = m_mousePosition - m_lastMousePosition;
    m_lastMousePosition = m_mousePosition;

    m_mouseWheelDelta = m_mouseScroll;
    m_mouseScroll = 0;
}

int32_t InputSystem::getAxisValue1D( uint32_t deviceIndex, InputID id ) const {
    switch( id.ID ) {
    case enums::toIntegral( MouseAxis::X ):
        return m_mousePosition.X;
    case enums::toIntegral( MouseAxis::Y ):
        return m_mousePosition.Y;
    case enums::toIntegral( MouseAxis::DeltaX ):
        return m_mouseDelta.X;
    case enums::toIntegral( MouseAxis::DeltaY ):
        return m_mouseDelta.Y;
    case enums::toIntegral( MouseAxis::Wheel ):
        return m_mouseWheelDelta;
    case enums::toIntegral( GameControllerAxis::LeftStick_X ):
    case enums::toIntegral( GameControllerAxis::LeftStick_Y ):
    case enums::toIntegral( GameControllerAxis::RightStick_X ):
    case enums::toIntegral( GameControllerAxis::RightStick_Y ):
    case enums::toIntegral( GameControllerAxis::LeftTrigger ):
    case enums::toIntegral( GameControllerAxis::RightTrigger ):
        return getControllerAxisValue( deviceIndex, static_cast< GameControllerAxis >( id.ID ) );
    default:
        ONYX_ASSERT( false, "Invalid axis input id" );
        return 0;
    }
}

Vector2s32 InputSystem::getAxisValue2D( uint32_t deviceIndex, InputID id ) const {
    switch( id.ID ) {
    case enums::toIntegral( MouseAxis::XY ):
        return m_mousePosition;
    case enums::toIntegral( MouseAxis::DeltaXY ):
        return m_mouseDelta;
    case enums::toIntegral( GameControllerAxis::LeftStick_XY ):
        return { getControllerAxisValue( deviceIndex, GameControllerAxis::LeftStick_X ),
                 getControllerAxisValue( deviceIndex, GameControllerAxis::LeftStick_Y ) };
    case enums::toIntegral( GameControllerAxis::RightStick_XY ):
        return { getControllerAxisValue( deviceIndex, GameControllerAxis::RightStick_X ),
                 getControllerAxisValue( deviceIndex, GameControllerAxis::RightStick_Y ) };
    default:
        ONYX_ASSERT( false, "Invalid axis 2D input id" );
        return Vector2s32::zero();
    }
}

bool InputSystem::isButtonDown( InputID id ) const {
    if( IsMouseButton( id ) ) {
        return isButtonDown( static_cast< MouseButton >( id.ID ) );
    }
    if( IsKeyboardKey( id ) ) {
        return isButtonDown( static_cast< Key >( id.ID ) );
    }
    if( IsGameControllerButton( id ) ) {
        return isButtonDown( static_cast< GameControllerButton >( id.ID ), 0 );
    }

    return false;
}

bool InputSystem::isButtonDown( MouseButton button ) const {
    return m_mouseButtonStates[ ToIndex( button ) ];
}

bool InputSystem::isButtonDown( Key key ) const {
    return m_keyState[ ToIndex( key ) ];
}

bool InputSystem::isButtonDown( GameControllerButton button, uint8_t deviceIndex ) const {
    if( deviceIndex >= m_gamepads.size() )
        return false;

    constexpr uint16_t First = enums::toIntegral( GameControllerButton::First );
    ONYX_ASSERT( enums::toIntegral( button ) > First );
    ONYX_ASSERT( enums::toIntegral( button ) < enums::toIntegral( GameControllerButton::Last ) );

    uint16_t bitmaskIndex = enums::toIntegral( button ) - First;
    return m_gamepads[ deviceIndex ].ButtonStates & ( 1 << bitmaskIndex );
}

void InputSystem::setMousePosition( const Vector2s32& mousePos ) {
    if( m_mousePosition != mousePos ) {
        m_mousePosition = mousePos;

        // MouseEvent event;
        // event.m_Id = InputEventType::MousePositionChanged;
        // event.m_Position = mousePos;
        // m_OnInput(&event);
    }
}

int32_t InputSystem::getControllerAxisValue( uint32_t controllerIndex, GameControllerAxis axis ) const {
    // TODO: 0.5.0 fix this for controllers
    if( controllerIndex >= m_gamepads.size() )
        return 0;

    ONYX_ASSERT( controllerIndex < m_gamepads.size() );
    const GameController& gamepad = m_gamepads[ controllerIndex ];
    ONYX_ASSERT( gamepad.IsConnected );

    constexpr uint16_t Offset = enums::toIntegral( GameControllerAxis::First );
    uint8_t index = static_cast< uint8_t >( enums::toIntegral( axis ) - Offset );
    return gamepad.m_AxisValues[ index ];
}

void InputSystem::enableSystemMouseCapture( bool /*enable*/ ) {
    // TODO: Fix
    // m_MainWindow->EnableSystemMouseCapture(enable);
}

void InputSystem::updateMouse( uint8_t queueIndex ) {
    if( m_mouseAxisInputQueue[ queueIndex ].has_value() ) {
        MouseAxisEvent event = m_mouseAxisInputQueue[ queueIndex ].value();
        m_mouseScroll = event.Value;
        m_mouseAxisSignal.dispatch( event );
        m_mouseAxisInputQueue[ queueIndex ].reset();
    }

    for( const MouseButtonEvent& event : m_mouseButtonInputQueue[ queueIndex ] ) {
        m_mouseButtonStates[ ToIndex( event.Button ) ] = event.State != ButtonState::Up;
        m_mouseButtonSignal.dispatch( event );
    }
    m_mouseButtonInputQueue[ queueIndex ].clear();

    if( m_mousePositionInputQueue[ queueIndex ].has_value() ) {
        MousePositionEvent event = m_mousePositionInputQueue[ queueIndex ].value();
        m_mousePosition = event.Position;
        m_mousePositionSignal.dispatch( event );
        m_mousePositionInputQueue[ queueIndex ].reset();
    }
}

void InputSystem::updateKeyboard( uint8_t queueIndex ) {
    for( const KeyboardEvent& event : m_keyboardInputQueue[ queueIndex ] ) {
        m_keyState[ ToIndex( event.Key ) ] = event.State != ButtonState::Up;
        m_keySignal.dispatch( event );
    }
    m_keyboardInputQueue[ queueIndex ].clear();
}

void InputSystem::updateGameControllers( uint8_t queueIndex ) {
    for( const GameControllerAxisEvent& event : m_controllerAxisInputQueue[ queueIndex ] ) {
        GameController& controller = m_gamepads[ event.ControllerIndex ];
        controller.m_AxisValues[ ToIndex( ( event.Axis ) ) ] = event.Value;
        m_controllerAxisSignal.dispatch( event );
    }
    m_controllerAxisInputQueue[ queueIndex ].clear();

    for( const GameControllerButtonEvent& event : m_controllerButtonInputQueue[ queueIndex ] ) {
        GameController& controller = m_gamepads[ event.ControllerIndex ];

        uint32_t buttonMask = 1 << ToIndex( event.Button );
        if( event.State == ButtonState::Up ) {
            controller.ButtonStates &= ~buttonMask;
        } else {
            controller.ButtonStates |= buttonMask;
        }

        m_controllerButtonSignal.dispatch( event );
    }

    m_controllerButtonInputQueue[ queueIndex ].clear();
}

#if ONYX_IS_WINDOWS && !ONYX_USE_SDL2

//  void InputSystem::HandleGamepadConnected(uint32_t deviceIndex)
//  {
//      ONYX_ASSERT(deviceIndex <= m_Gamepads.size());
//
//      if (deviceIndex == m_Gamepads.size())
//          m_Gamepads.emplace_back();
//
//      m_Gamepads[deviceIndex].m_IsConnected = true;
//  }
//
//  void InputSystem::HandleGamepadDisconnected(uint32_t deviceIndex)
//  {
//      ONYX_ASSERT(deviceIndex < m_Gamepads.size());
//      m_Gamepads[deviceIndex].m_IsConnected = false;
//  }
//
//  void InputSystem::HandleGameControllerButtonMessage(uint32_t controllerIndex, GameControllerButton button, bool
//  isPressed)
//  {
//      ONYX_ASSERT(controllerIndex < m_Gamepads.size());
//      Gamepad& gamepad = m_Gamepads[controllerIndex];
//      ONYX_ASSERT(gamepad.m_IsConnected);
//
//      constexpr uint16_t offset = enums::ToIntegral(Key::Last) + 1;
//      uint8_t index = static_cast<uint8_t>(enums::ToIntegral(button) - offset);
//      uint32_t buttonMask = 1 << index;
//      bool wasPressed = (gamepad.m_ButtonStates & buttonMask) != 0;
//
//      if ((isPressed == false) && (wasPressed == false))
//          return;
//
//      if (isPressed)
//          gamepad.m_ButtonStates |= buttonMask;
//      else
//          gamepad.m_ButtonStates &= ~buttonMask;
//
//      InputEventType eventId = isPressed ? (wasPressed ? InputEventType::GameControllerButtonRepeat :
//      InputEventType::GameControllerButtonDown) : InputEventType::GameControllerButtonUp;
//
//      GameControllerButtonEvent event;
//      event.m_Id = eventId;
//      event.m_ControllerIndex = controllerIndex;
//      event.m_Button = button;
//      m_OnInput(&event);
//  }
//
//  void InputSystem::HandleGameControllerAxisMessage(uint32_t controllerIndex, GameControllerAxis axis, int16_t
//  axisValue)
//  {
//      ONYX_ASSERT(controllerIndex < m_Gamepads.size());
//      Gamepad& gamepad = m_Gamepads[controllerIndex];
//      ONYX_ASSERT(gamepad.m_IsConnected);
//
//      constexpr uint16_t offset = enums::ToIntegral(GameControllerButton::Last) + 1;
//      uint8_t index = static_cast<uint8_t>(enums::ToIntegral(axis) - offset);
//      if (axisValue == gamepad.m_AxisValues[index])
//          return;
//
//      gamepad.m_AxisValues[index] = axisValue;
//
//      GameControllerAxisEvent event;
//      event.m_Id = InputEventType::GameControllerAxis;
//      event.m_ControllerIndex = controllerIndex;
//      event.m_Axis = axis;
//      event.m_Value = axisValue;
//      m_OnInput(&event);
//  }

#endif
} // namespace onyx::input
