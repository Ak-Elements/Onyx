#include <onyx/inputactions/bindings/inputbindingbool.h>

#include <onyx/input/inputsystem.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::input_actions
{
    bool InputBindingBool::DoUpdate(const input::InputSystem& inputSystem, Vector3f32& outInputValue)
    {
        bool newValue = inputSystem.IsButtonDown(m_Input);
        outInputValue.X = newValue ? 1.0f : 0.0f;
        return newValue;
    }
}

namespace onyx
{
    bool Serialization<input_actions::InputBindingBool>::serialize(Serializer& serializer, const input_actions::InputBindingBool& binding)
    {
        return serializer.write<"input">(binding.m_Input);
    }

    bool Serialization<input_actions::InputBindingBool>::deserialize(const Deserializer& deserializer, input_actions::InputBindingBool& outBinding)
    {
        return deserializer.read<"input">(outBinding.m_Input);
    }
}
