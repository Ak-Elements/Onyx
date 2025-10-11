#include <onyx/input/bindings/inputbindingaxis2d.h>
#include <onyx/input/inputsystem.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx::Input
{
    bool InputBindingAxis2D::DoUpdate(const InputSystem& inputSystem, Vector3f32& outInputValue)
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

    InputID InputBindingAxis2D::GetBoundInputForSlot(onyxU32 index) const
    {
        return index == 0 ? m_AxisX : m_AxisY;
    }

    void InputBindingAxis2D::SetInputBindingSlot(onyxU32 index, InputID inputID)
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
    bool Serialization<Input::InputBindingAxis2D>::Serialize(Serializer& serializer, const Input::InputBindingAxis2D& binding)
    {
        return serializer.Write<"axisX">(binding.m_AxisX) &&
            serializer.Write<"axisY">(binding.m_AxisY);
    }

    bool Serialization<Input::InputBindingAxis2D>::Deserialize(const Deserializer& deserializer, Input::InputBindingAxis2D& outBinding)
    {
        return deserializer.Read<"axisX">(outBinding.m_AxisX) &&
            deserializer.Read<"axisY">(outBinding.m_AxisY);
    }
}
