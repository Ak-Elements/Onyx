#pragma once

#include <onyx/input/triggers/inputtrigger.h>

namespace Onyx::Input
{
    class InputSystem;
    class InputActionSystem;

    class DependantActionInputTrigger : public InputTrigger
    {
        friend struct Serialization<DependantActionInputTrigger>;
    public:
        static constexpr StringId32 TypeId{ "Onyx::Input::DependantActionInputTrigger" };
        StringId32 GetTypeId() const override { return TypeId; }

        bool IsTriggered(const InputSystem& inputSystem, const InputActionSystem& inputActionSystem) const override;

    private:
        StringId64 m_ActionId;
    };
}


namespace Onyx
{
    template <>
    struct Serialization<Input::DependantActionInputTrigger>
    {
        static bool Serialize(Serializer& serializer, const Input::DependantActionInputTrigger& trigger);
        static bool Deserialize(const Deserializer& deserializer, Input::DependantActionInputTrigger& outTrigger);
    };
}
