#pragma once

#if ONYX_IS_WINDOWS && ONYX_USE_GAMEINPUT

// struct IGameInput;
// struct IGameInputDevice;
#include <GameInput.h>
#include <onyx/input/gamecontroller.h>

namespace onyx::input {
enum class InputEventId : uint8_t;

class GameInput {
    struct GameInputDevice {
        GameInputDevice( IGameInputDevice& device, const GameInputDeviceInfo* info, uint32_t deviceIndex )
            : m_DevicePtr( &device )
            , m_DeviceIndex( deviceIndex )
            , m_Info( info ) {}

        bool operator==( IGameInputDevice& device ) const { return m_DevicePtr == &device; }
        bool operator==( IGameInputDevice* device ) const { return m_DevicePtr == device; }

        bool m_Connected = false;

        IGameInputDevice* m_DevicePtr;
        const GameInputDeviceInfo* m_Info;
        uint32_t m_DeviceIndex;
        uint32_t m_DeviceId;
    };

  public:
    GameInput();
    ~GameInput();
    bool OnDeviceChange();

    void Update();

    void SetControllerConnectedHandler( const InplaceFunction< void( uint32_t ) >& handler ) {
        m_OnGameControllerConnected = handler;
    }
    void SetControllerDisconnectedHandler( const InplaceFunction< void( uint32_t ) >& handler ) {
        m_OnGameControllerDisconnected = handler;
    }

    void SetControllerButtonHandler( const InplaceFunction< void( uint32_t, GameControllerButton, bool ) >& handler ) {
        m_ControllerButtonHandler = handler;
    }
    void SetControllerStickHandler( const InplaceFunction< void( uint32_t, GameControllerAxis, int16_t ) >& handler ) {
        m_ControllerStickHandler = handler;
    }

  private:
    bool AddInputDevice( IGameInputDevice& device );
    void RemoveInputDevice( IGameInputDevice& device );

    bool TryRestoreInputDevice( uint32_t deviceId, uint32_t& outRestoredDeviceIndex );
    bool HasInputDevice( IGameInputDevice& device ) const;

    static void OnDeviceCallback( GameInputCallbackToken callbackToken,
                                  void* context,
                                  IGameInputDevice* device,
                                  uint64_t timestamp,
                                  GameInputDeviceStatus currentStatus,
                                  GameInputDeviceStatus previousStatus );

    InplaceFunction< void( uint32_t ) > m_OnGameControllerConnected;
    InplaceFunction< void( uint32_t ) > m_OnGameControllerDisconnected;

    InplaceFunction< void( uint32_t, GameControllerButton, bool ) > m_ControllerButtonHandler;
    InplaceFunction< void( uint32_t, GameControllerAxis, int16_t ) > m_ControllerStickHandler;

    IGameInput* m_GameInput = nullptr;
    GameInputCallbackToken m_CallbackToken = GAMEINPUT_INVALID_CALLBACK_TOKEN_VALUE;

    DynamicArray< GameInputDevice > m_ActiveInputDevices;
    DynamicArray< GameInputDevice > m_InactiveInputDevices;

    Atomic< uint32_t > m_NextDeviceIndex = 0;
};
} // namespace onyx::input

#endif
