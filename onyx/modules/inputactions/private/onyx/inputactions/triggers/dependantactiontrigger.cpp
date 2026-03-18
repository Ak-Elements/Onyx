#include <onyx/inputactions/triggers/dependantactiontrigger.h>

#include <onyx/inputactions/inputactionsystem.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::input_actions
{
    bool DependantActionInputTrigger::IsTriggered(const input::InputSystem& /*inputSystem*/, const InputActionSystem& inputActionSystem) const
    {
        return inputActionSystem.IsActionTriggered(m_ActionId);
    }
}

namespace onyx
{
    bool Serialization<input_actions::DependantActionInputTrigger>::Serialize(Serializer& serializer, const input_actions::DependantActionInputTrigger& trigger)
    {
        return serializer.Write<"actionId">(trigger.m_ActionId);
    }

    bool Serialization<input_actions::DependantActionInputTrigger>::Deserialize(const Deserializer& deserializer, input_actions::DependantActionInputTrigger& outTrigger)
    {
        return deserializer.Read<"actionId">(outTrigger.m_ActionId);
    }
}