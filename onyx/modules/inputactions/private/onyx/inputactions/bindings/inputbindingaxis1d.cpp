#include <onyx/inputactions/bindings/inputbindingaxis1d.h>
#include <onyx/input/inputsystem.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx::InputActions
{
    bool InputBindingAxis1D::DoUpdate(const Input::InputSystem& inputSystem, Vector3f32& outInputValue)
    {
        outInputValue.X = inputSystem.GetAxisValue1D(0, m_Axis);
        return IsZero(outInputValue.X) == false;
    }

    void InputBindingAxis1D::SetInputBindingSlot(onyxU32, Input::InputID inputID)
    {
        m_Axis = inputID;
    }
}

namespace Onyx
{
    bool Serialization<InputActions::InputBindingAxis1D>::Serialize(Serializer& serializer, const InputActions::InputBindingAxis1D& binding)
    {
        return serializer.Write<"axis">(binding.m_Axis);
    }

    bool Serialization<InputActions::InputBindingAxis1D>::Deserialize(const Deserializer& deserializer, InputActions::InputBindingAxis1D& outBinding)
    {
        return deserializer.Read<"axis">(outBinding.m_Axis);
    }
}

