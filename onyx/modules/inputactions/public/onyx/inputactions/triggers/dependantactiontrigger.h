#pragma once

#include <onyx/inputactions/triggers/inputtrigger.h>

namespace onyx::input
{
    class InputSystem;
}

namespace onyx::input_actions
{
    class InputActionSystem;

    class DependantActionInputTrigger : public InputTrigger
    {
        friend struct Serialization<DependantActionInputTrigger>;
    public:
        static constexpr StringId32 TypeId{ "onyx::input::DependantActionInputTrigger" };
        StringId32 GetTypeId() const override { return TypeId; }

        bool IsTriggered(const input::InputSystem& inputSystem, const InputActionSystem& inputActionSystem) const override;

        StringId64& GetActionId() { return m_ActionId; }
        void SetActionId(StringId64 id) { m_ActionId = id; }

    private:
        StringId64 m_ActionId;
    };
}


namespace onyx
{
    template <>
    struct Serialization<input_actions::DependantActionInputTrigger>
    {
        static bool Serialize(Serializer& serializer, const input_actions::DependantActionInputTrigger& trigger);
        static bool Deserialize(const Deserializer& deserializer, input_actions::DependantActionInputTrigger& outTrigger);
    };
}
