#include <onyx/input/inputsystem.h>

#include <onyx/input/inputevent.h>
#include <onyx/input/inputid.h>

namespace onyx::input {
void InputSystem::update() {
    uint8_t queueIndex = m_CurrentQueueIndex;
    m_CurrentQueueIndex = ( m_CurrentQueueIndex + 1 ) % INPUT_QUEUE_COUNT;

    // process input queues
    UpdateMouse( queueIndex );
    UpdateKeyboard( queueIndex );
    UpdateGameControllers( queueIndex );

    m_MouseDelta = m_MousePosition - m_LastMousePosition;
    m_LastMousePosition = m_MousePosition;

    m_MouseWheelDelta = m_MouseScroll;
    m_MouseScroll = 0;
}

int32_t InputSystem::GetAxisValue1D( uint32_t deviceIndex, InputID id ) const {
    switch( id.ID ) {
    case enums::toIntegral( MouseAxis::X ):
        return m_MousePosition.X;
    case enums::toIntegral( MouseAxis::Y ):
        return m_MousePosition.Y;
    case enums::toIntegral( MouseAxis::DeltaX ):
        return m_MouseDelta.X;
    case enums::toIntegral( MouseAxis::DeltaY ):
        return m_MouseDelta.Y;
    case enums::toIntegral( MouseAxis::Wheel ):
        return m_MouseWheelDelta;
    case enums::toIntegral( GameControllerAxis::LeftStick_X ):
    case enums::toIntegral( GameControllerAxis::LeftStick_Y ):
    case enums::toIntegral( GameControllerAxis::RightStick_X ):
    case enums::toIntegral( GameControllerAxis::RightStick_Y ):
    case enums::toIntegral( GameControllerAxis::LeftTrigger ):
    case enums::toIntegral( GameControllerAxis::RightTrigger ):
        return GetControllerAxisValue( deviceIndex, static_cast< GameControllerAxis >( id.ID ) );
    default:
        ONYX_ASSERT( false, "Invalid axis input id" );
        return 0;
    }
}

Vector2s32 InputSystem::GetAxisValue2D( uint32_t deviceIndex, InputID id ) const {
    switch( id.ID ) {
    case enums::toIntegral( MouseAxis::XY ):
        return m_MousePosition;
    case enums::toIntegral( MouseAxis::DeltaXY ):
        return m_MouseDelta;
    case enums::toIntegral( GameControllerAxis::LeftStick_XY ):
        return { GetControllerAxisValue( deviceIndex, GameControllerAxis::LeftStick_X ),
                 GetControllerAxisValue( deviceIndex, GameControllerAxis::LeftStick_Y ) };
    case enums::toIntegral( GameControllerAxis::RightStick_XY ):
        return { GetControllerAxisValue( deviceIndex, GameControllerAxis::RightStick_X ),
                 GetControllerAxisValue( deviceIndex, GameControllerAxis::RightStick_Y ) };
    default:
        ONYX_ASSERT( false, "Invalid axis 2D input id" );
        return Vector2s32::zero();
    }
}

bool InputSystem::IsButtonDown( InputID id ) const {
    if( IsMouseButton( id ) ) {
        return IsButtonDown( static_cast< MouseButton >( id.ID ) );
    }
    if( IsKeyboardKey( id ) ) {
        return IsButtonDown( static_cast< Key >( id.ID ) );
    }
    if( IsGameControllerButton( id ) ) {
        return IsButtonDown( static_cast< GameControllerButton >( id.ID ), 0 );
    }

    return false;
}

bool InputSystem::IsButtonDown( MouseButton button ) const {
    return m_MouseButtonStates[ ToIndex( button ) ];
}

bool InputSystem::IsButtonDown( Key key ) const {
    return m_KeyState[ ToIndex( key ) ];
}

bool InputSystem::IsButtonDown( GameControllerButton button, uint8_t deviceIndex ) const {
    if( deviceIndex >= m_Gamepads.size() )
        return false;

    constexpr uint16_t first = enums::toIntegral( GameControllerButton::First );
    ONYX_ASSERT( enums::toIntegral( button ) > first );
    ONYX_ASSERT( enums::toIntegral( button ) < enums::toIntegral( GameControllerButton::Last ) );

    uint16_t bitmaskIndex = enums::toIntegral( button ) - first;
    return m_Gamepads[ deviceIndex ].ButtonStates & ( 1 << bitmaskIndex );
}

void InputSystem::SetMousePosition( const Vector2s32& mousePos ) {
    if( m_MousePosition != mousePos ) {
        m_MousePosition = mousePos;

        // MouseEvent event;
        // event.m_Id = InputEventType::MousePositionChanged;
        // event.m_Position = mousePos;
        // m_OnInput(&event);
    }
}

int32_t InputSystem::GetControllerAxisValue( uint32_t controllerIndex, GameControllerAxis axis ) const {
    // TODO: 0.5.0 fix this for controllers
    if( controllerIndex >= m_Gamepads.size() )
        return 0;

    ONYX_ASSERT( controllerIndex < m_Gamepads.size() );
    const GameController& gamepad = m_Gamepads[ controllerIndex ];
    ONYX_ASSERT( gamepad.IsConnected );

    constexpr uint16_t offset = enums::toIntegral( GameControllerAxis::First );
    uint8_t index = static_cast< uint8_t >( enums::toIntegral( axis ) - offset );
    return gamepad.m_AxisValues[ index ];
}

void InputSystem::EnableSystemMouseCapture( bool /*enable*/ ) {
    // TODO: Fix
    // m_MainWindow->EnableSystemMouseCapture(enable);
}

void InputSystem::UpdateMouse( uint8_t queueIndex ) {
    if( m_MouseAxisInputQueue[ queueIndex ].has_value() ) {
        MouseAxisEvent event = m_MouseAxisInputQueue[ queueIndex ].value();
        m_MouseScroll = event.Value;
        m_MouseAxisSignal.Dispatch( event );
        m_MouseAxisInputQueue[ queueIndex ].reset();
    }

    for( const MouseButtonEvent& event : m_MouseButtonInputQueue[ queueIndex ] ) {
        m_MouseButtonStates[ ToIndex( event.Button ) ] = event.State != ButtonState::Up;
        m_MouseButtonSignal.Dispatch( event );
    }
    m_MouseButtonInputQueue[ queueIndex ].clear();

    if( m_MousePositionInputQueue[ queueIndex ].has_value() ) {
        MousePositionEvent event = m_MousePositionInputQueue[ queueIndex ].value();
        m_MousePosition = event.Position;
        m_MousePositionSignal.Dispatch( event );
        m_MousePositionInputQueue[ queueIndex ].reset();
    }
}

void InputSystem::UpdateKeyboard( uint8_t queueIndex ) {
    for( const KeyboardEvent& event : m_KeyboardInputQueue[ queueIndex ] ) {
        m_KeyState[ ToIndex( event.Key ) ] = event.State != ButtonState::Up;
        m_KeySignal.Dispatch( event );
    }
    m_KeyboardInputQueue[ queueIndex ].clear();
}

void InputSystem::UpdateGameControllers( uint8_t queueIndex ) {
    for( const GameControllerAxisEvent& event : m_ControllerAxisInputQueue[ queueIndex ] ) {
        GameController& controller = m_Gamepads[ event.ControllerIndex ];
        controller.m_AxisValues[ ToIndex( ( event.Axis ) ) ] = event.Value;
        m_ControllerAxisSignal.Dispatch( event );
    }
    m_ControllerAxisInputQueue[ queueIndex ].clear();

    for( const GameControllerButtonEvent& event : m_ControllerButtonInputQueue[ queueIndex ] ) {
        GameController& controller = m_Gamepads[ event.ControllerIndex ];

        uint32_t buttonMask = 1 << ToIndex( event.Button );
        if( event.State == ButtonState::Up ) {
            controller.ButtonStates &= ~buttonMask;
        } else {
            controller.ButtonStates |= buttonMask;
        }

        m_ControllerButtonSignal.Dispatch( event );
    }

    m_ControllerButtonInputQueue[ queueIndex ].clear();
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
