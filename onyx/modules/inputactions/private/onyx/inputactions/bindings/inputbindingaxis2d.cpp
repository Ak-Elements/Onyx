#include <onyx/inputactions/bindings/inputbindingaxis2d.h>
#include <onyx/input/inputsystem.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx::InputActions
{
    bool InputBindingAxis2D::DoUpdate(const Input::InputSystem& inputSystem, Vector3f32& outInputValue)
    {
        outInputValue.X = inputSystem.GetAxisValue1D(0, m_AxisX);
        outInputValue.Y = inputSystem.GetAxisValue1D(0, m_AxisY);
        return IsZero(outInputValue.X) == false || IsZero(outInputValue.Y) == false;
    }

    void InputBindingAxis2D::Reset()
    {
        m_AxisX = 0;
        m_AxisY = 0;
    }

    Input::InputID InputBindingAxis2D::GetBoundInputForSlot(onyxU32 index) const
    {
        return index == 0 ? m_AxisX : m_AxisY;
    }

    void InputBindingAxis2D::SetInputBindingSlot(onyxU32 index, Input::InputID inputID)
    {
        if (index == 0)
            m_AxisX = inputID;
        else
            m_AxisY = inputID;
    }

    StringView InputBindingAxis2D::GetInputBindingSlotName(onyxU32 index) const
    {
        return index == 0 ? "X" : "Y";
    }
}

namespace Onyx
{
    bool Serialization<InputActions::InputBindingAxis2D>::Serialize(Serializer& serializer, const InputActions::InputBindingAxis2D& binding)
    {
        return serializer.Write<"axisX">(binding.m_AxisX) &&
            serializer.Write<"axisY">(binding.m_AxisY);
    }

    bool Serialization<InputActions::InputBindingAxis2D>::Deserialize(const Deserializer& deserializer, InputActions::InputBindingAxis2D& outBinding)
    {
        return deserializer.Read<"axisX">(outBinding.m_AxisX) &&
            deserializer.Read<"axisY">(outBinding.m_AxisY);
    }
}
