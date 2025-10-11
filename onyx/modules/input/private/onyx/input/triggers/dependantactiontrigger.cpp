#include <onyx/input/triggers/dependantactiontrigger.h>
#include <onyx/input/inputactionsystem.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx::Input
{
    bool DependantActionInputTrigger::IsTriggered(const InputSystem& /*inputSystem*/, const InputActionSystem& inputActionSystem) const
    {
        return inputActionSystem.IsActionTriggered(m_ActionId);
    }
}

namespace Onyx
{
    bool Serialization<Input::DependantActionInputTrigger>::Serialize(Serializer& serializer, const Input::DependantActionInputTrigger& trigger)
    {
        return serializer.Write<"actionId">(trigger.m_ActionId);
    }

    bool Serialization<Input::DependantActionInputTrigger>::Deserialize(const Deserializer& deserializer, Input::DependantActionInputTrigger& outTrigger)
    {
        return deserializer.Read<"actionId">(outTrigger.m_ActionId);
    }
}