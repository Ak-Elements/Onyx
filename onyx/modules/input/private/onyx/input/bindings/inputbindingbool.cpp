#include <onyx/input/bindings/inputbindingbool.h>

#include <onyx/input/inputsystem.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx::Input
{
    bool InputBindingBool::DoUpdate(const InputSystem& inputSystem, Vector3f32& outInputValue)
    {
        bool newValue = inputSystem.IsButtonDown(m_Input);
        outInputValue.X = newValue ? 1.0f : 0.0f;
        return newValue;
    }
}

namespace Onyx
{
    bool Serialization<Input::InputBindingBool>::Serialize(Serializer& serializer, const Input::InputBindingBool& binding)
    {
        return serializer.Write<"input">(binding.m_Input);
    }

    bool Serialization<Input::InputBindingBool>::Deserialize(const Deserializer& deserializer, Input::InputBindingBool& outBinding)
    {
        return deserializer.Read<"input">(outBinding.m_Input);
    }
}
