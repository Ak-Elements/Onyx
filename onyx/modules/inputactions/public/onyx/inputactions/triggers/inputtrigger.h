#pragma once

namespace Onyx::Input
{
    class InputSystem;
}

namespace Onyx::InputActions
{
    class InputActionSystem;

    class InputTrigger
    {
    public:
        virtual ~InputTrigger() = default;
        virtual StringId32 GetTypeId() const = 0;
        virtual bool IsTriggered(const Input::InputSystem& inputSystem, const InputActionSystem& inputActionSystem) const = 0;
    };
}