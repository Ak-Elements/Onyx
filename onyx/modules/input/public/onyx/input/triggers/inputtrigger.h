#pragma once

namespace Onyx::Input
{
    class InputSystem;
    class InputActionSystem;

    class InputTrigger
    {
    public:
        virtual ~InputTrigger() = default;
        virtual StringId32 GetTypeId() const = 0;
        virtual bool IsTriggered(const InputSystem& inputSystem, const InputActionSystem& inputActionSystem) const = 0;
    };
}