#include <onyx/input/bindings/inputbindingaxis2dcomposite.h>
#include <onyx/input/inputsystem.h>
#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx::Input
{
    void InputBindingAxis2DComposite::Reset()
    {
        m_InputUp = 0;
        m_InputDown = 0;
        m_InputLeft = 0;
        m_InputRight = 0;
    }

    bool InputBindingAxis2DComposite::DoUpdate(const InputSystem& inputSystem, Vector3f32& outInputValue)
    {
        bool isUpDown = inputSystem.IsButtonDown(m_InputUp);
        bool isDownDown = inputSystem.IsButtonDown(m_InputDown);
        bool isLeftDown = inputSystem.IsButtonDown(m_InputLeft);
        bool isRightDown = inputSystem.IsButtonDown(m_InputRight);

        if ((isUpDown == false) && (isDownDown == false) &&
            (isLeftDown == false) && (isRightDown == false))
            return false;

        outInputValue.X = (isRightDown ? 1.0f : 0.0f) - (isLeftDown ? 1.0f : 0.0f);
        outInputValue.Y = (isUpDown ? 1.0f : 0.0f) - (isDownDown ? 1.0f : 0.0f);
        return true;
    }

    InputID InputBindingAxis2DComposite::GetBoundInputForSlot(onyxU32 index) const
    {
        switch (index)
        {
            case 0: return m_InputUp;
            case 1: return m_InputDown;
            case 2: return m_InputLeft;
            case 3: return m_InputRight;
            default: ONYX_ASSERT(false); return InputID{};
        }
    }

    void InputBindingAxis2DComposite::SetInputBindingSlot(onyxU32 index, InputID inputID)
    {
        switch (index)
        {
            case 0:
                m_InputUp = inputID;
                break;
            case 1:
                m_InputDown = inputID;
                break;
            case 2:
                m_InputLeft = inputID;
                break;
            case 3:
                m_InputRight = inputID;
                break;
            default: ONYX_ASSERT(false); break;
        }
    }

    StringView InputBindingAxis2DComposite::GetInputBindingSlotName(onyxU32 index) const
    {
        switch (index)
        {
            case 0: return "Up";
            case 1: return "Down";
            case 2: return "Left";
            case 3: return "Right";
            default: return "";
        }
    }
}

namespace Onyx
{
    bool Serialization<Input::InputBindingAxis2DComposite>::Serialize(Serializer& serializer, const Input::InputBindingAxis2DComposite& binding)
    {
        return serializer.Write<"up">(binding.m_InputUp) &&
            serializer.Write<"down">(binding.m_InputDown) &&
            serializer.Write<"left">(binding.m_InputLeft) &&
            serializer.Write<"right">(binding.m_InputRight);
    }

    bool Serialization<Input::InputBindingAxis2DComposite>::Deserialize(const Deserializer& deserializer, Input::InputBindingAxis2DComposite& outBinding)
    {
        return deserializer.Read<"up">(outBinding.m_InputUp) &&
            deserializer.Read<"down">(outBinding.m_InputDown) &&
            deserializer.Read<"left">(outBinding.m_InputLeft) &&
            deserializer.Read<"right">(outBinding.m_InputRight);
    }
}
