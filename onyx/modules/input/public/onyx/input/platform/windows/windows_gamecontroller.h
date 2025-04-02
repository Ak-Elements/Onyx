#pragma once

#if ONYX_IS_WINDOWS && ONYX_USE_GAMEINPUT

//struct IGameInput;
//struct IGameInputDevice;
#include <onyx/input/gamepad.h>
#include <GameInput.h>

namespace Onyx::Input
{
    enum class InputEventId : onyxU8;

    class GameInput
    {
        struct GameInputDevice
        {
            GameInputDevice(IGameInputDevice& device, const GameInputDeviceInfo* info, onyxU32 deviceIndex)
                : m_DevicePtr(&device)
                , m_DeviceIndex(deviceIndex)
                , m_Info(info)
            {
            }

            bool operator==(IGameInputDevice& device) const { return m_DevicePtr == &device; }
            bool operator==(IGameInputDevice* device) const { return m_DevicePtr == device; }

            bool m_Connected = false;

            IGameInputDevice* m_DevicePtr;
            const GameInputDeviceInfo* m_Info;
            onyxU32 m_DeviceIndex;
            onyxU32 m_DeviceId;
        };

    public:
        GameInput();
        ~GameInput();
        bool OnDeviceChange();

        void Update();

        void SetControllerConnectedHandler(const InplaceFunction<void(onyxU32)>& handler) { m_OnGameControllerConnected = handler; }
        void SetControllerDisconnectedHandler(const InplaceFunction<void(onyxU32)>& handler) { m_OnGameControllerDisconnected = handler; }

        void SetControllerButtonHandler(const InplaceFunction<void(onyxU32, GameControllerButton, bool)>& handler) { m_ControllerButtonHandler = handler; }
        void SetControllerStickHandler(const InplaceFunction<void(onyxU32, GameControllerAxis, onyxS16)>& handler) { m_ControllerStickHandler = handler; }

    private:
        bool AddInputDevice(IGameInputDevice& device);
        void RemoveInputDevice(IGameInputDevice& device);

        bool TryRestoreInputDevice(onyxU32 deviceId, onyxU32& outRestoredDeviceIndex);
        bool HasInputDevice(IGameInputDevice& device) const;

        static void OnDeviceCallback(GameInputCallbackToken callbackToken,
                                     void* context,
                                     IGameInputDevice* device,
                                     onyxU64 timestamp,
                                     GameInputDeviceStatus currentStatus,
                                     GameInputDeviceStatus previousStatus);

        InplaceFunction<void(onyxU32)> m_OnGameControllerConnected;
        InplaceFunction<void(onyxU32)> m_OnGameControllerDisconnected;

        InplaceFunction<void(onyxU32, GameControllerButton, bool)> m_ControllerButtonHandler;
        InplaceFunction<void(onyxU32, GameControllerAxis, onyxS16)> m_ControllerStickHandler;

        IGameInput* m_GameInput = nullptr;
        GameInputCallbackToken m_CallbackToken = GAMEINPUT_INVALID_CALLBACK_TOKEN_VALUE;

        DynamicArray<GameInputDevice> m_ActiveInputDevices;
        DynamicArray<GameInputDevice> m_InactiveInputDevices;

        Atomic<onyxU32> m_NextDeviceIndex = 0;
    };
}

#endif
