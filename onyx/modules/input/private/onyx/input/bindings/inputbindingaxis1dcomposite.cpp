#include <onyx/input/bindings/inputbindingaxis1dcomposite.h>
#include <onyx/input/inputsystem.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace Onyx::Input
{
    bool InputBindingAxis1DComposite::DoUpdate(const InputSystem& inputSystem, Vector3f32& outInputValue)
    {
        bool isPostiveDown = inputSystem.IsButtonDown(m_InputPostive);
        bool isNegativeDown = inputSystem.IsButtonDown(m_InputNegative);

        if ((isPostiveDown == false) && (isNegativeDown == false))
            return false;

        outInputValue.X = (isPostiveDown ? 1.0f : 0.0f) - (isNegativeDown ? 1.0f : 0.0f);
        return true;
    }

    void InputBindingAxis1DComposite::Reset()
    {
        m_InputPostive = 0;
        m_InputNegative = 0;
    }

    InputID InputBindingAxis1DComposite::GetBoundInputForSlot(onyxU32 index) const
    {
        return index == 0 ? m_InputPostive : m_InputNegative;
    }

    void InputBindingAxis1DComposite::SetInputBindingSlot(onyxU32 index, InputID inputID)
    {
        if (index == 0)
            m_InputPostive = inputID;
        else
            m_InputNegative = inputID;
    }

    StringView InputBindingAxis1DComposite::GetInputBindingSlotName(onyxU32 index) const
    {
        return index == 0 ? "Postive" : "Negative";
    }
}

namespace Onyx
{
    bool Serialization<Input::InputBindingAxis1DComposite>::Serialize(Serializer& serializer, const Input::InputBindingAxis1DComposite& binding)
    {
        return serializer.Write<"positive">(binding.m_InputPostive) &&
            serializer.Write<"negative">(binding.m_InputNegative);
    }

    bool Serialization<Input::InputBindingAxis1DComposite>::Deserialize(const Deserializer& deserializer, Input::InputBindingAxis1DComposite& outBinding)
    {
        return deserializer.Read<"positive">(outBinding.m_InputPostive) &&
            deserializer.Read<"negative">(outBinding.m_InputNegative);
    }
}
