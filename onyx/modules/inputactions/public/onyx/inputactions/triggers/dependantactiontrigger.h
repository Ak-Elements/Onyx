#pragma once

#include <onyx/inputactions/triggers/inputtrigger.h>

namespace Onyx::Input
{
    class InputSystem;
}

namespace Onyx::InputActions
{
    class InputActionSystem;

    class DependantActionInputTrigger : public InputTrigger
    {
        friend struct Serialization<DependantActionInputTrigger>;
    public:
        static constexpr StringId32 TypeId{ "Onyx::Input::DependantActionInputTrigger" };
        StringId32 GetTypeId() const override { return TypeId; }

        bool IsTriggered(const Input::InputSystem& inputSystem, const InputActionSystem& inputActionSystem) const override;

    private:
        StringId64 m_ActionId;
    };
}


namespace Onyx
{
    template <>
    struct Serialization<InputActions::DependantActionInputTrigger>
    {
        static bool Serialize(Serializer& serializer, const InputActions::DependantActionInputTrigger& trigger);
        static bool Deserialize(const Deserializer& deserializer, InputActions::DependantActionInputTrigger& outTrigger);
    };
}
