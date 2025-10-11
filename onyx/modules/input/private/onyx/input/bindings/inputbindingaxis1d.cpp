#include <onyx/input/bindings/inputbindingaxis1d.h>
#include <onyx/input/inputsystem.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx::Input
{
    bool InputBindingAxis1D::DoUpdate(const InputSystem& inputSystem, Vector3f32& outInputValue)
    {
        outInputValue.X = inputSystem.GetAxisValue1D(0, m_Axis);
        return IsZero(outInputValue.X) == false;
    }

    void InputBindingAxis1D::SetInputBindingSlot(onyxU32, InputID inputID)
    {
        m_Axis = inputID;
    }
}

namespace Onyx
{
    bool Serialization<Input::InputBindingAxis1D>::Serialize(Serializer& serializer, const Input::InputBindingAxis1D& binding)
    {
        return serializer.Write<"axis">(binding.m_Axis);
    }

    bool Serialization<Input::InputBindingAxis1D>::Deserialize(const Deserializer& deserializer, Input::InputBindingAxis1D& outBinding)
    {
        return deserializer.Read<"axis">(outBinding.m_Axis);
    }
}

