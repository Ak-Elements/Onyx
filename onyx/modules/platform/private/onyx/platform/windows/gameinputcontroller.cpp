// #include <onyx/input/platform/windows/windows_gamecontroller.h>

#if ONYX_IS_WINDOWS && ONYX_USE_GAMEINPUT

#include <onyx/log/logger.h>

#include <onyx/input/inputevent.h>

#include <GameInput.h>

namespace onyx::input {
namespace {
constexpr uint64_t TIMEOUT_IN_MICROSECONDS = 10000;

GameControllerButton ConvertToGameControllerButton( const GameInputGamepadButtons& buttonMask ) {
    switch ( buttonMask ) {
    case GameInputGamepadMenu:
        return GameControllerButton::Button_Start;
    case GameInputGamepadView:
        return GameControllerButton::Button_Back;
    case GameInputGamepadA:
        return GameControllerButton::Button_South;
    case GameInputGamepadB:
        return GameControllerButton::Button_East;
    case GameInputGamepadX:
        return GameControllerButton::Button_West;
    case GameInputGamepadY:
        return GameControllerButton::Button_North;
    case GameInputGamepadDPadUp:
        return GameControllerButton::Button_DPad_Up;
    case GameInputGamepadDPadDown:
        return GameControllerButton::Button_DPad_Down;
    case GameInputGamepadDPadLeft:
        return GameControllerButton::Button_DPad_Left;
    case GameInputGamepadDPadRight:
        return GameControllerButton::Button_DPad_Right;
    case GameInputGamepadLeftShoulder:
        return GameControllerButton::Button_LeftShoulder;
    case GameInputGamepadRightShoulder:
        return GameControllerButton::Button_RightShoulder;
    case GameInputGamepadLeftThumbstick:
        return GameControllerButton::Button_LeftStick;
    case GameInputGamepadRightThumbstick:
        return GameControllerButton::Button_RightStick;
    case GameInputGamepadNone:
    default:
        ONYX_ASSERT( false, "Unhandled controller button." );
        return GameControllerButton::Invalid;
    }
}

int16_t ConvertStickAxisValue( float axisValue ) {
    static const float kAnalogStickDeadZone = 0.18f;
    if ( ( axisValue < kAnalogStickDeadZone ) && ( axisValue > -kAnalogStickDeadZone ) )
        return 0;

    return static_cast< int16_t >( ( axisValue < 0.0f ) ? ( axisValue * 32768.0f ) : ( axisValue * 32767.0f ) );
}
} // namespace

Gameinput::GameInput() {
    HRESULT result = GameInputCreate( &m_GameInput );
    if ( FAILED( result ) ) {
        ONYX_LOG_FATAL( "GameInput failed to be created. Result code: {}", result );
        return;
    }

    result = m_GameInput->RegisterDeviceCallback( nullptr,
                                                  GameInputKindController,
                                                  GameInputDeviceConnected,
                                                  GameInputBlockingEnumeration,
                                                  this,
                                                  &Gameinput::OnDeviceCallback,
                                                  &m_CallbackToken );

    if ( FAILED( result ) ) {
        ONYX_LOG_FATAL( "Failed to register device callback. Result code: {}", result );
        return;
    }
}

Gameinput::~GameInput() {
    m_GameInput->UnregisterCallback( m_CallbackToken, TIMEOUT_IN_MICROSECONDS );
}

bool Gameinput::OnDeviceChange() {
    // Detect new device
    return false;
}

void Gameinput::Update() {
    GameInputGamepadState gameControllerState;
    IGameInputReading* reading = nullptr;

    const bool hasButtonEventHandler = m_ControllerButtonHandler != nullptr;
    const bool hasStickEventHandler = m_ControllerStickHandler != nullptr;

    for ( GameInputDevice& inputDevice : m_ActiveInputDevices ) {
        HRESULT result = m_GameInput->GetCurrentReading( inputDevice.m_Info->supportedInput,
                                                         inputDevice.m_DevicePtr,
                                                         &reading );
        if ( FAILED( result ) )
            continue;

        if ( ( inputDevice.m_Info->supportedInput & GameInputKindController ) == 0 )
            continue;

        if ( ( inputDevice.m_Info->supportedInput & GameInputKindGamepad ) != 0 ) {
            if ( reading->GetGamepadState( &gameControllerState ) == false )
                continue;

            // Update button states
            for ( uint32_t buttonIndex = 0; buttonIndex < inputDevice.m_Info->controllerButtonCount; ++buttonIndex ) {
                const GameInputGamepadButtons buttonMask = static_cast< GameInputGamepadButtons >( 1 << buttonIndex );
                bool isPressed = ( gameControllerState.buttons & buttonMask ) != 0;

                if ( hasButtonEventHandler )
                    m_ControllerButtonHandler( inputDevice.m_DeviceIndex,
                                               ConvertToGameControllerButton( buttonMask ),
                                               isPressed );
            }

            if ( hasStickEventHandler ) {
                m_ControllerStickHandler( inputDevice.m_DeviceIndex,
                                          GameControllerAxis::LeftStick_AxisX,
                                          ConvertStickAxisValue( gameControllerState.leftThumbstickX ) );
                m_ControllerStickHandler( inputDevice.m_DeviceIndex,
                                          GameControllerAxis::LeftStick_AxisY,
                                          ConvertStickAxisValue( gameControllerState.leftThumbstickY ) );

                m_ControllerStickHandler( inputDevice.m_DeviceIndex,
                                          GameControllerAxis::RightStick_AxisX,
                                          ConvertStickAxisValue( gameControllerState.rightThumbstickX ) );
                m_ControllerStickHandler( inputDevice.m_DeviceIndex,
                                          GameControllerAxis::RightStick_AxisY,
                                          ConvertStickAxisValue( gameControllerState.rightThumbstickY ) );
            }
        } else {
            bool* buttonStates = new bool[ inputDevice.m_Info->controllerButtonCount ]{ false };
            float* axisStates = new float[ inputDevice.m_Info->controllerAxisCount ]{ 0.0f };

            reading->GetControllerButtonState( inputDevice.m_Info->controllerButtonCount, buttonStates );
            std::ignore/*const uint32_t axisCount*/ = reading->GetControllerAxisCount();

            // Update button states
            uint32_t buttonsPressedMask = 0;
            for ( uint32_t buttonIndex = 0; buttonIndex < inputDevice.m_Info->controllerButtonCount; ++buttonIndex ) {
                const GameInputGamepadButtons buttonMask = static_cast< GameInputGamepadButtons >( 1 << buttonIndex );
                bool isPressed = buttonStates[ buttonIndex ];

                if ( isPressed )
                    buttonsPressedMask |= ( 1 << buttonIndex );

                if ( hasButtonEventHandler ) {
                    m_ControllerButtonHandler( inputDevice.m_DeviceIndex,
                                               ConvertToGameControllerButton( buttonMask ),
                                               isPressed );
                }
            }

            delete[] buttonStates;
            delete[] axisStates;
        }

        reading->Release();
    }
}

bool Gameinput::AddInputDevice( IGameInputDevice& device ) {
    ConnectionType connection;

    const GameInputDeviceInfo* deviceInfo = device.GetDeviceInfo();
    if ( deviceInfo->capabilities & GameInputDeviceCapabilityWireless ) {
        connection = ConnectionType::Wireless;
    } else {
        connection = ConnectionType::USB;
    }

    std::ignore /*uint16_t vendorId*/ = deviceInfo->vendorId;
    std::ignore /*uint16_t productId*/ = deviceInfo->productId;
    std::ignore /*uint16_t version*/ = ( deviceInfo->firmwareVersion.major << 8 ) | deviceInfo->firmwareVersion.minor;

    std::ignore = connection;

    // Check if the device is handled by another input api

    if ( HasInputDevice( device ) )
        return true;

    uint32_t deviceId = 0;
    std::memcpy( &deviceId, &( deviceInfo->deviceId.value ), sizeof( uint32_t ) );

    uint32_t restoredDeviceIndex = 0;
    if ( TryRestoreInputDevice( deviceId, restoredDeviceIndex ) ) {
        if ( m_OnGameControllerConnected )
            m_OnGameControllerConnected( restoredDeviceIndex );

        return true;
    }

    // no controller can be hijacked - create new controller
    GameInputDevice& newDevice = m_ActiveInputDevices.emplace_back( device,
                                                                    deviceInfo,
                                                                    m_NextDeviceIndex.fetch_add( 1 ) );
    newDevice.m_Connected = true;
    newDevice.m_DeviceId = deviceId;

    if ( m_OnGameControllerConnected )
        m_OnGameControllerConnected( newDevice.m_DeviceIndex );

    return true;
}

void Gameinput::RemoveInputDevice( IGameInputDevice& device ) {
    m_ActiveInputDevices.erase( std::remove_if( m_ActiveInputDevices.begin(),
                                                m_ActiveInputDevices.end(),
                                                [ & ]( GameInputDevice& inputDevice ) {
                                                    if ( inputDevice == device ) {
                                                        inputDevice.m_Connected = false;
                                                        m_InactiveInputDevices.push_back( inputDevice );

                                                        if ( m_OnGameControllerDisconnected )
                                                            m_OnGameControllerDisconnected( inputDevice.m_DeviceIndex );

                                                        return true;
                                                    }

                                                    return false;
                                                } ),
                                m_ActiveInputDevices.end() );
}

bool Gameinput::TryRestoreInputDevice( uint32_t deviceId, uint32_t& outRestoredDeviceIndex ) {
    auto beginIt = m_InactiveInputDevices.begin();
    auto endIt = m_InactiveInputDevices.end();
    // try restore device with the same deviceId first
    auto it = std::remove_if( beginIt, endIt, [ & ]( GameInputDevice& inputDevice ) {
        if ( inputDevice.m_DeviceId == deviceId ) {
            inputDevice.m_Connected = true;
            outRestoredDeviceIndex = inputDevice.m_DeviceIndex;
            m_ActiveInputDevices.push_back( inputDevice );
            return true;
        }

        return false;
    } );

    if ( it != endIt ) {
        m_InactiveInputDevices.erase( it, endIt );
        return true;
    }

    // hijack free inactive device
    auto hijackIt = std::remove_if( beginIt, endIt, [ & ]( GameInputDevice& inputDevice ) {
        if ( inputDevice.m_DeviceId == deviceId ) {
            inputDevice.m_Connected = true;
            outRestoredDeviceIndex = inputDevice.m_DeviceIndex;
            m_ActiveInputDevices.push_back( inputDevice );
            return true;
        }

        return false;
    } );

    if ( hijackIt != endIt ) {
        m_InactiveInputDevices.erase( hijackIt, m_InactiveInputDevices.end() );
        return true;
    }

    return false;
}

bool Gameinput::HasInputDevice( IGameInputDevice& device ) const {
    for ( const GameInputDevice& gamepad : m_ActiveInputDevices ) {
        if ( gamepad == device )
            return true;
    }

    return false;
}

void Gameinput::OnDeviceCallback( GameInputCallbackToken,
                                  void* context,
                                  IGameInputDevice* device,
                                  uint64_t timestamp,
                                  GameInputDeviceStatus currentStatus,
                                  GameInputDeviceStatus previousStatus ) {
    std::ignore = timestamp;
    std::ignore = previousStatus;

    GameInput* gameInput = static_cast< GameInput* >( context );

    if ( device == nullptr )
        return;

    if ( ( currentStatus & GameInputDeviceConnected ) != 0 ) {
        gameInput->AddInputDevice( *device );
    } else {
        gameInput->RemoveInputDevice( *device );
    }
}
} // namespace onyx::input

#endif