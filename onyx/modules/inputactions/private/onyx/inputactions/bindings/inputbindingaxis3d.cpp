#include <onyx/inputactions/bindings/inputbindingaxis3d.h>
#include <onyx/input/inputsystem.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx::InputActions
{
    bool InputBindingAxis3D::DoUpdate(const Input::InputSystem& inputSystem, Vector3f32& outInputValue)
    {
        outInputValue.X = numeric_cast<onyxF32>(inputSystem.GetAxisValue1D(0, m_AxisX));
        outInputValue.X = numeric_cast<onyxF32>(inputSystem.GetAxisValue1D(0, m_AxisY));
        outInputValue.Z = numeric_cast<onyxF32>(inputSystem.GetAxisValue1D(0, m_AxisZ));
        return IsZero(outInputValue.X) == false || IsZero(outInputValue.Y) || IsZero(outInputValue.Z);
    }

    void InputBindingAxis3D::Reset()
    {
        m_AxisX = 0;
        m_AxisY = 0;
        m_AxisZ = 0;
    }


    Input::InputID InputBindingAxis3D::GetBoundInputForSlot(onyxU32 index) const
    {
        switch (index)
        {
            case 0: return m_AxisX;
            case 1: return m_AxisY;
            case 2: return m_AxisZ;
            default: ONYX_ASSERT(false); return Input::InputID{};
        }
    }

    void InputBindingAxis3D::SetInputBindingSlot(onyxU32 index, Input::InputID inputID)
    {
        switch (index)
        {
            case 0:
                m_AxisX = inputID;
                break;
            case 1:
                m_AxisY = inputID;
                break;
            case 2:
                m_AxisZ = inputID;
                break;
            default: ONYX_ASSERT(false); return;
        }
    }

    StringView InputBindingAxis3D::GetInputBindingSlotName(onyxU32 index) const
    {
        switch (index)
        {
            case 0: return "X";
            case 1: return "Y";
            case 2: return "Z";
            default: return "";
        }
    }
}

namespace Onyx
{
    bool Serialization<InputActions::InputBindingAxis3D>::Serialize(Serializer& serializer, const InputActions::InputBindingAxis3D& binding)
    {
        return serializer.Write<"axisX">(binding.m_AxisX) &&
            serializer.Write<"axisY">(binding.m_AxisY) &&
            serializer.Write<"axisZ">(binding.m_AxisZ);
    }

    bool Serialization<InputActions::InputBindingAxis3D>::Deserialize(const Deserializer& deserializer, InputActions::InputBindingAxis3D& outBinding)
    {
        return deserializer.Read<"axisX">(outBinding.m_AxisX) &&
            deserializer.Read<"axisY">(outBinding.m_AxisY) &&
            deserializer.Read<"axisZ">(outBinding.m_AxisZ);
    }
}
