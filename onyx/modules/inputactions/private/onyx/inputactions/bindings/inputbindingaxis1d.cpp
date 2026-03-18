#include <onyx/inputactions/bindings/inputbindingaxis1d.h>
#include <onyx/input/inputsystem.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::input_actions
{
    bool InputBindingAxis1D::DoUpdate(const input::InputSystem& inputSystem, Vector3f32& outInputValue)
    {
        outInputValue.X = numeric_cast<onyxF32>(inputSystem.GetAxisValue1D(0, m_Axis));
        return IsZero(outInputValue.X) == false;
    }

    void InputBindingAxis1D::SetInputBindingSlot(onyxU32, input::InputID inputID)
    {
        m_Axis = inputID;
    }
}

namespace onyx
{
    bool Serialization<input_actions::InputBindingAxis1D>::Serialize(Serializer& serializer, const input_actions::InputBindingAxis1D& binding)
    {
        return serializer.Write<"axis">(binding.m_Axis);
    }

    bool Serialization<input_actions::InputBindingAxis1D>::Deserialize(const Deserializer& deserializer, input_actions::InputBindingAxis1D& outBinding)
    {
        return deserializer.Read<"axis">(outBinding.m_Axis);
    }
}

