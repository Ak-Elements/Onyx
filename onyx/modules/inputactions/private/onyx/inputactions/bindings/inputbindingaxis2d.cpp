#include <onyx/inputactions/bindings/inputbindingaxis2d.h>
#include <onyx/input/inputsystem.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::input_actions
{
    bool InputBindingAxis2D::DoUpdate(const input::InputSystem& inputSystem, Vector3f32& outInputValue)
    {
        outInputValue.X = numeric_cast<onyxF32>(inputSystem.GetAxisValue1D(0, m_AxisX));
        outInputValue.Y = numeric_cast<onyxF32>(inputSystem.GetAxisValue1D(0, m_AxisY));
        return IsZero(outInputValue.X) == false || IsZero(outInputValue.Y) == false;
    }

    void InputBindingAxis2D::Reset()
    {
        m_AxisX = 0;
        m_AxisY = 0;
    }

    input::InputID InputBindingAxis2D::GetBoundInputForSlot(onyxU32 index) const
    {
        return index == 0 ? m_AxisX : m_AxisY;
    }

    void InputBindingAxis2D::SetInputBindingSlot(onyxU32 index, input::InputID inputID)
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

namespace onyx
{
    bool Serialization<input_actions::InputBindingAxis2D>::Serialize(Serializer& serializer, const input_actions::InputBindingAxis2D& binding)
    {
        return serializer.Write<"axisX">(binding.m_AxisX) &&
            serializer.Write<"axisY">(binding.m_AxisY);
    }

    bool Serialization<input_actions::InputBindingAxis2D>::Deserialize(const Deserializer& deserializer, input_actions::InputBindingAxis2D& outBinding)
    {
        return deserializer.Read<"axisX">(outBinding.m_AxisX) &&
            deserializer.Read<"axisY">(outBinding.m_AxisY);
    }
}
