#include <onyx/inputactions/triggers/dependantactiontrigger.h>

#include <onyx/inputactions/inputactionsystem.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx::InputActions
{
    bool DependantActionInputTrigger::IsTriggered(const Input::InputSystem& /*inputSystem*/, const InputActionSystem& inputActionSystem) const
    {
        return inputActionSystem.IsActionTriggered(m_ActionId);
    }
}

namespace Onyx
{
    bool Serialization<InputActions::DependantActionInputTrigger>::Serialize(Serializer& serializer, const InputActions::DependantActionInputTrigger& trigger)
    {
        return serializer.Write<"actionId">(trigger.m_ActionId);
    }

    bool Serialization<InputActions::DependantActionInputTrigger>::Deserialize(const Deserializer& deserializer, InputActions::DependantActionInputTrigger& outTrigger)
    {
        return deserializer.Read<"actionId">(outTrigger.m_ActionId);
    }
}