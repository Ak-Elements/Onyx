#include <onyx/input/inputbinding.h>

#include <onyx/input/keycodes.h>
#include <onyx/input/mouse.h>
#include <onyx/input/gamepad.h>
#include <onyx/input/inputsystem.h>

namespace Onyx::Input
{
    namespace
    {
        template <typename EnumT>
        onyxF32 GetInput1D(const InputBindingAxis1DComposite& binding, const InputSystem& inputSystem)
        {
            bool postiveDown = inputSystem.IsButtonDown(static_cast<EnumT>(binding.GetBoundInputForSlot(0)));
            bool negativeDown = inputSystem.IsButtonDown(static_cast<EnumT>(binding.GetBoundInputForSlot(1)));

            if (postiveDown && negativeDown)
                return 0.0f;
            if (postiveDown && !negativeDown)
                return 1.0f;

            return -1.0f;
        }

        template <typename EnumT>
        Vector2f GetInput2D(const InputBindingAxis2DComposite& binding, const InputSystem& inputSystem)
        {
            // do something
            bool upInputDown = inputSystem.IsButtonDown(static_cast<EnumT>(binding.GetBoundInputForSlot(0)));
            bool downInputDown = inputSystem.IsButtonDown(static_cast<EnumT>(binding.GetBoundInputForSlot(1)));
            bool leftInputDown = inputSystem.IsButtonDown(static_cast<EnumT>(binding.GetBoundInputForSlot(2)));
            bool rightInputDown = inputSystem.IsButtonDown(static_cast<EnumT>(binding.GetBoundInputForSlot(3)));

            Vector2f result;
            if (leftInputDown && rightInputDown)
                result[0] = 0.0f;
            else if (rightInputDown && !leftInputDown)
                result[0] = 1.0f;
            else if (leftInputDown)
                result[0] = -1.0f;

            if (upInputDown && downInputDown)
                result[1] = 0.0f;
            else if (upInputDown && !downInputDown)
                result[1] = 1.0f;
            else if (downInputDown)
                result[1] = -1.0f;

            return result;
        }

        template <typename EnumT>
        Vector3f GetInput3D(const InputBindingAxis3DComposite& binding, const InputSystem& inputSystem)
        {
            // do something
            bool upInputDown = inputSystem.IsButtonDown(static_cast<EnumT>(binding.GetBoundInputForSlot(0)));
            bool downInputDown = inputSystem.IsButtonDown(static_cast<EnumT>(binding.GetBoundInputForSlot(1)));
            bool leftInputDown = inputSystem.IsButtonDown(static_cast<EnumT>(binding.GetBoundInputForSlot(2)));
            bool rightInputDown = inputSystem.IsButtonDown(static_cast<EnumT>(binding.GetBoundInputForSlot(3)));
            bool forwardInputDown = inputSystem.IsButtonDown(static_cast<EnumT>(binding.GetBoundInputForSlot(4)));
            bool backwardInputDown = inputSystem.IsButtonDown(static_cast<EnumT>(binding.GetBoundInputForSlot(5)));

            Vector3f result;
            if (leftInputDown && rightInputDown)
                result[0] = 0.0f;
            else if (rightInputDown && !leftInputDown)
                result[0] = 1.0f;
            else if (leftInputDown)
                result[0] = -1.0f;

            if (upInputDown && downInputDown)
                result[1] = 0.0f;
            else if (upInputDown && !downInputDown)
                result[1] = 1.0f;
            else if (downInputDown)
                result[1] = -1.0f;

            if (forwardInputDown && backwardInputDown)
                result[2] = 0.0f;
            else if (forwardInputDown && !backwardInputDown)
                result[2] = 1.0f;
            else if (backwardInputDown)
                result[2] = -1.0f;

            return result;
        }
    }

    bool InputBindingBool::UpdateBinding(const InputSystem& inputSystem, UniquePtr<InputBindingContext>& context)
    {
        bool newValue = false;
        switch (m_Type)
        {
            case InputType::Mouse:
            {
                newValue = inputSystem.IsButtonDown(static_cast<MouseButton>(m_Input));
                break;
            }
            case InputType::Keyboard:
            {
                newValue = inputSystem.IsButtonDown(static_cast<Key>(m_Input));
                break;
            }
            case InputType::Gamepad:
            {
                newValue = inputSystem.IsButtonDown(static_cast<GameControllerButton>(m_Input));
                break;
            }
        }

        InputBindingBoolContext& boolContext = static_cast<InputBindingBoolContext&>(*context);
        if (boolContext.Value != newValue)
        {
            boolContext.Value = newValue;
            return true;
        }

        return false;
    }

    void InputBindingAxis1D::SetInputBindingSlot(onyxU32, onyxU32 inputValue)
    {
        m_Axis = inputValue;
    }

    void InputBindingAxis1DComposite::Reset()
    {
        m_InputPostive = 0;
        m_InputNegative = 0;
    }

    bool InputBindingAxis1DComposite::UpdateBinding(const InputSystem& inputSystem, UniquePtr<InputBindingContext>& context)
    {
        onyxF32 newValue = 0.0f;
        switch (m_Type)
        {
            case InputType::Mouse:
            {
                newValue = GetInput1D<MouseButton>(*this, inputSystem);
                break;
            }
            case InputType::Keyboard:
            {
                newValue = GetInput1D<Key>(*this, inputSystem);
                break;
            }
            case InputType::Gamepad:
            {
                // TODO: Add support for sticks
                newValue = GetInput1D<GameControllerButton>(*this, inputSystem);
                break;
            }
        }

        InputBindingAxis1DContext& axisContext = static_cast<InputBindingAxis1DContext&>(*context);
        if (IsEqual(axisContext.Value, newValue) == false)
        {
            axisContext.Value = newValue;
            return true;
        }

        return false;
    }

    onyxU32 InputBindingAxis1DComposite::GetBoundInputForSlot(onyxU32 index) const
    {
        return index == 0 ? m_InputPostive : m_InputNegative;
    }

    void InputBindingAxis1DComposite::SetInputBindingSlot(onyxU32 index, onyxU32 inputValue)
    {
        if (index == 0)
            m_InputPostive = inputValue;
        else
            m_InputNegative = inputValue;
    }

    void InputBindingAxis2DComposite::Reset()
    {
        m_InputUp = 0;
        m_InputDown = 0;
        m_InputLeft = 0;
        m_InputRight = 0;
    }

    bool InputBindingAxis2DComposite::UpdateBinding(const InputSystem& inputSystem, UniquePtr<InputBindingContext>& context)
    {
        Vector2f newValue;
        switch (m_Type)
        {
            case InputType::Mouse:
            {
                newValue = GetInput2D<MouseButton>(*this, inputSystem);
                break;
            }
            case InputType::Keyboard:
            {
                newValue = GetInput2D<Key>(*this, inputSystem);
                break;
            }
            case InputType::Gamepad:
            {
                // TODO: Add support for sticks
                newValue = GetInput2D<GameControllerButton>(*this, inputSystem);
                break;
            }
        }
        
        InputBindingAxis2DContext& axisContext = static_cast<InputBindingAxis2DContext&>(*context);
        if (IsEqual(axisContext.Value, newValue) == false)
        {
            axisContext.Value = newValue;
            return true;
        }

        return false;
    }

    onyxU32 InputBindingAxis2DComposite::GetBoundInputForSlot(onyxU32 index) const
    {
        switch (index)
        {
        case 0: return m_InputUp;
        case 1: return m_InputDown;
        case 2: return m_InputLeft;
        case 3: return m_InputRight;
        default: ONYX_ASSERT(false); return onyxMax_U32;
        }
    }

    void InputBindingAxis2DComposite::SetInputBindingSlot(onyxU32 index, onyxU32 inputValue)
    {
        switch (index)
        {
        case 0:
            m_InputUp = inputValue;
            break;
        case 1:
            m_InputDown = inputValue;
            break;
        case 2:
            m_InputLeft = inputValue;
            break;
        case 3:
            m_InputRight = inputValue;
            break;
        default: ONYX_ASSERT(false); break;
        }
    }

    void InputBindingAxis3DComposite::Reset()
    {
        m_InputUp = 0;
        m_InputDown = 0;
        m_InputLeft = 0;
        m_InputRight = 0;
        m_InputForward = 0;
        m_InputBackward = 0;
    }

    bool InputBindingAxis3DComposite::UpdateBinding(const InputSystem& inputSystem, UniquePtr<InputBindingContext>& context)
    {
        Vector3f newValue;
        switch (m_Type)
        {
            case InputType::Mouse:
            {
                newValue = GetInput3D<MouseButton>(*this, inputSystem);
                break;
            }
            case InputType::Keyboard:
            {
                newValue = GetInput3D<Key>(*this, inputSystem);
                break;
            }
            case InputType::Gamepad:
            {
                // TODO: Add support for sticks
                newValue = GetInput3D<GameControllerButton>(*this, inputSystem);
                break;
            }
        }

        InputBindingAxis3DContext& axisContext = static_cast<InputBindingAxis3DContext&>(*context);
        if (IsEqual(axisContext.Value, newValue) == false)
        {
            axisContext.Value = newValue;
            return true;
        }

        return false;
    }

    bool InputBindingAxis1D::UpdateBinding(const InputSystem& inputSystem, UniquePtr<InputBindingContext>& context)
    {
        onyxF32 newValue = 0.0f;
        switch (m_Type)
        {
            case InputType::Mouse:
            {
                MouseAxis axis = static_cast<MouseAxis>(m_Axis);

                if (axis == MouseAxis::PositionX)
                    newValue = inputSystem.GetMousePosition()[0];
                else if (axis == MouseAxis::PositionY)
                    newValue = inputSystem.GetMousePosition()[1];
                else if (axis == MouseAxis::DeltaX)
                    newValue = inputSystem.GetMouseDelta()[0];
                else if (axis == MouseAxis::DeltaY)
                    newValue = inputSystem.GetMouseDelta()[1];
                else //if (axis == MouseAxis::Wheel)
                    newValue = inputSystem.GetMouseWheelDelta();
                break;
            }
            case InputType::Gamepad:
            {
                // TODO: Add controller index
                const GameControllerAxis axis = static_cast<GameControllerAxis>(m_Axis);
                newValue = inputSystem.GetControllerAxisValue(0, axis);
                break;
            }
            default:
                ONYX_ASSERT(false, "Unhandled input type for InputBindingAxis1D");
                break;
        }

        InputBindingAxis1DContext& axisContext = static_cast<InputBindingAxis1DContext&>(*context);
        if (IsEqual(axisContext.Value, newValue) == false)
        {
            axisContext.Value = newValue;
            return true;
        }

        return false;
    }

    void InputBindingAxis2D::Reset()
    {
        m_AxisX = 0;
        m_AxisY = 0;
    }

    bool InputBindingAxis2D::UpdateBinding(const InputSystem& inputSystem, UniquePtr<InputBindingContext>& context)
    {
        Vector2f newValue;
        switch (m_Type)
        {
            case InputType::Mouse:
            {
                const MouseAxis axisX = static_cast<MouseAxis>(m_AxisX);
                const MouseAxis axisY = static_cast<MouseAxis>(m_AxisY);

                switch (axisX)
                {
                    case MouseAxis::PositionX:
                        newValue[0] = inputSystem.GetMousePosition()[0];
                        break;
                    case MouseAxis::PositionY:
                        newValue[0] = inputSystem.GetMousePosition()[1];
                        break;
                    case MouseAxis::DeltaX:
                        newValue[0] = inputSystem.GetMouseDelta()[0];
                        break;
                    case MouseAxis::DeltaY:
                        newValue[0] = inputSystem.GetMouseDelta()[1];
                        break;
                    case MouseAxis::Wheel:
                    case MouseAxis::Invalid:
                    case MouseAxis::Count:
                        ONYX_ASSERT(false, "Invalid mouse axis");
                        break;
                }

                switch (axisY)
                {
                    case MouseAxis::PositionX:
                        newValue[1] = inputSystem.GetMousePosition()[0];
                        break;
                    case MouseAxis::PositionY:
                        newValue[1] = inputSystem.GetMousePosition()[1];
                        break;
                    case MouseAxis::DeltaX:
                        newValue[1] = inputSystem.GetMouseDelta()[0];
                        break;
                    case MouseAxis::DeltaY:
                        newValue[1] = inputSystem.GetMouseDelta()[1];
                        break;
                    case MouseAxis::Wheel:
                    case MouseAxis::Invalid:
                    case MouseAxis::Count:
                        ONYX_ASSERT(false, "Invalid mouse axis");
                        break;
                }

                break;
            }
            case InputType::Gamepad:
            {
                // TODO: Add controller index
                //const GameControllerAxis axisX = static_cast<GameControllerAxis>(m_AxisX);
                //const GameControllerAxis axisY = static_cast<GameControllerAxis>(m_AxisY);
                //newValue[0] = inputSystem.GetControllerAxisValue(0, axisX);
                //newValue[1] = inputSystem.GetControllerAxisValue(0, axisY);
                break;
            }
            default:
                ONYX_ASSERT(false, "Unhandled input type for InputBindingAxis1D");
                break;
        }

        InputBindingAxis2DContext& axisContext = static_cast<InputBindingAxis2DContext&>(*context);
        if (IsEqual(axisContext.Value, newValue) == false)
        {
            axisContext.Value = newValue;
            return true;
        }

        return false;
    }

    onyxU32 InputBindingAxis2D::GetBoundInputForSlot(onyxU32 index) const
    {
        return index == 0 ? m_AxisX : m_AxisY;
    }

    void InputBindingAxis2D::SetInputBindingSlot(onyxU32 index, onyxU32 inputValue)
    {
        if (index == 0)
            m_AxisX = inputValue;
        else
            m_AxisY = inputValue;
    }

    void InputBindingAxis3D::Reset()
    {
        m_AxisX = 0;
        m_AxisY = 0;
        m_AxisZ = 0;
    }

    bool InputBindingAxis3D::UpdateBinding(const InputSystem& inputSystem, UniquePtr<InputBindingContext>& context)
    {
        return InputBinding::UpdateBinding(inputSystem, context);
    }

    onyxU32 InputBindingAxis3D::GetBoundInputForSlot(onyxU32 index) const
    {
        switch (index)
        {
            case 0: return m_AxisX;
            case 1: return m_AxisY;
            case 2: return m_AxisZ;
            default: ONYX_ASSERT(false); return onyxMax_U32;
        }
    }

    void InputBindingAxis3D::SetInputBindingSlot(onyxU32 index, onyxU32 inputValue)
    {
        switch (index)
        {
            case 0:
                m_AxisX = inputValue;
                break;
            case 1:
                m_AxisY = inputValue;
                break;
            case 2:
                m_AxisZ = inputValue;
                break;
            default: ONYX_ASSERT(false); return;
        }
    }

    onyxU32 InputBindingAxis3DComposite::GetBoundInputForSlot(onyxU32 index) const
    {
        switch (index)
        {
            case 0: return m_InputUp;
            case 1: return m_InputDown;
            case 2: return m_InputLeft;
            case 3: return m_InputRight;
            case 4: return m_InputForward;
            case 5: return m_InputBackward;
            default: ONYX_ASSERT(false); return onyxMax_U32;
        }
    }

    void InputBindingAxis3DComposite::SetInputBindingSlot(onyxU32 index, onyxU32 inputValue)
    {
        switch (index)
        {
            case 0:
                m_InputUp = inputValue;
                break;
            case 1:
                m_InputDown = inputValue;
                break;
            case 2:
                m_InputLeft = inputValue;
                break;
            case 3:
                m_InputRight = inputValue;
                break;
            case 4:
                m_InputForward = inputValue;
                break;
            case 5:
                m_InputBackward = inputValue;
                break;
            default: ONYX_ASSERT(false); break;
        }
    }

    StringView InputBindingAxis1DComposite::GetInputBindingSlotName(onyxU32 index) const
    {
        return index == 0 ? "Postive" : "Negative";
    }

    StringView InputBindingAxis2D::GetInputBindingSlotName(onyxU32 index) const
    {
        return index == 0 ? "X" : "Y";
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

    StringView InputBindingAxis3DComposite::GetInputBindingSlotName(onyxU32 index) const
    {
        switch (index)
        {
            case 0: return "Up";
            case 1: return "Down";
            case 2: return "Left";
            case 3: return "Right";
            case 4: return "Forward";
            case 5: return "Backward";
            default: return "";
        }
    }

}
