#include <onyx/inputactions/bindings/inputbindingbool.h>

#include <onyx/input/inputsystem.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx::InputActions
{
    bool InputBindingBool::DoUpdate(const Input::InputSystem& inputSystem, Vector3f32& outInputValue)
    {
        bool newValue = inputSystem.IsButtonDown(m_Input);
        outInputValue.X = newValue ? 1.0f : 0.0f;
        return newValue;
    }
}

namespace Onyx
{
    bool Serialization<InputActions::InputBindingBool>::Serialize(Serializer& serializer, const InputActions::InputBindingBool& binding)
    {
        return serializer.Write<"input">(binding.m_Input);
    }

    bool Serialization<InputActions::InputBindingBool>::Deserialize(const Deserializer& deserializer, InputActions::InputBindingBool& outBinding)
    {
        return deserializer.Read<"input">(outBinding.m_Input);
    }
}
