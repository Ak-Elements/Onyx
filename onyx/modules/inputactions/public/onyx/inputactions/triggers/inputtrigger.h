#pragma once

namespace onyx::input
{
    class InputSystem;
}

namespace onyx::input_actions
{
    class InputActionSystem;

    class InputTrigger
    {
    public:
        virtual ~InputTrigger() = default;
        virtual StringId32 GetTypeId() const = 0;
        virtual bool IsTriggered(const input::InputSystem& inputSystem, const InputActionSystem& inputActionSystem) const = 0;
    };
}