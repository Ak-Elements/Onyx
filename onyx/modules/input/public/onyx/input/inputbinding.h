#pragma once

#include <onyx/input/inputtypes.h>

namespace Onyx
{
    enum class GameControllerAxis : onyxU8;
}

namespace Onyx::Input
{
    class InputSystem;

    enum class ActionType
    {
        Invalid = 0,
        Bool,
        Axis1D,
        Axis2D,
        Axis3D,
        Count
    };

    struct InputBindingContext
    {
        virtual ~InputBindingContext() = default;
        virtual void* GetData() = 0;
    };

    struct InputBindingBoolContext : public InputBindingContext
    {
        void* GetData() override { return &Value; }

        bool Value = false;
    };

    struct InputBindingAxis1DContext : public InputBindingContext
    {
        void* GetData() override { return &Value; }

        onyxF32 Value = 0.0f;
    };

    struct InputBindingAxis2DContext : public InputBindingContext
    {
        void* GetData() override { return &Value; }

        Vector2f Value;
    };

    struct InputBindingAxis3DContext : public InputBindingContext
    {
        void* GetData() override { return &Value; }

        Vector3f Value;
    };

    struct InputBinding
    {
        virtual ~InputBinding() = default;

        virtual void Reset() = 0;
        virtual bool UpdateBinding(const InputSystem& /*inputSystem*/, UniquePtr<InputBindingContext>& /*context*/) { return false; }
        virtual UniquePtr<InputBindingContext> CreateContext() const = 0;

        virtual onyxU32 GetId() const { return 0; }

        InputType GetType() const { return m_Type; }
        void SetType(InputType type) { m_Type = type; }

        virtual onyxS32 GetInputBindingSlotsCount() const { return 0; }
        virtual onyxU32 GetBoundInputForSlot(onyxU32 /*index*/) const { return onyxMax_U32; }
        virtual void SetInputBindingSlot(onyxU32 /*index*/, onyxU32 /*inputValue*/) {}

        virtual StringView GetName() const { return ""; }
        virtual StringView GetInputBindingSlotName(onyxU32 /*index*/) const = 0;

    protected:
        InputType m_Type = InputType::Keyboard;
    };

    struct InputBindingBool : public InputBinding
    {
        static constexpr onyxU32 ID = TypeHash<InputBindingBool>();

        void Reset() override { m_Input = 0; }

        onyxU32 GetId() const override { return ID; }
        bool UpdateBinding(const InputSystem& inputSystem, UniquePtr<InputBindingContext>& context) override;

        UniquePtr<InputBindingContext> CreateContext() const override { return MakeUnique<InputBindingBoolContext>(); }

        onyxS32 GetInputBindingSlotsCount() const override { return 1; }
        onyxU32 GetBoundInputForSlot(onyxU32) const override { return m_Input; }
        void SetInputBindingSlot(onyxU32, onyxU32 inputValue) override { m_Input = inputValue; }

        StringView GetName() const override { return "Bool"; }
        StringView GetInputBindingSlotName(onyxU32) const override { return ""; }

    private:
        onyxU32 m_Input = 0;
    };

    struct InputBindingAxis1DComposite : public InputBinding
    {
        static constexpr onyxU32 ID = TypeHash<InputBindingAxis1DComposite>();

        void Reset() override;

        onyxU32 GetId() const override { return ID; }
        bool UpdateBinding(const InputSystem& inputSystem, UniquePtr<InputBindingContext>& context) override;

        UniquePtr<InputBindingContext> CreateContext() const override { return MakeUnique<InputBindingAxis1DContext>(); }

        onyxS32 GetInputBindingSlotsCount() const override { return 2; }
        onyxU32 GetBoundInputForSlot(onyxU32 index) const override;
        void SetInputBindingSlot(onyxU32 index, onyxU32 inputValue) override;

        StringView GetName() const override { return "Axis 1D Composite"; }
        StringView GetInputBindingSlotName(onyxU32 index) const override;

    private:
        onyxU32 m_InputPostive = 0;
        onyxU32 m_InputNegative = 0;
    };

    struct InputBindingAxis2DComposite : public InputBinding
    {
        static constexpr onyxU32 ID = TypeHash<InputBindingAxis2DComposite>();

        void Reset() override;

        onyxU32 GetId() const override { return ID; }
        bool UpdateBinding(const InputSystem& inputSystem, UniquePtr<InputBindingContext>& context) override;

        UniquePtr<InputBindingContext> CreateContext() const override { return MakeUnique<InputBindingAxis2DContext>(); }

        onyxS32 GetInputBindingSlotsCount() const override { return 4; }
        onyxU32 GetBoundInputForSlot(onyxU32 index) const override;
        void SetInputBindingSlot(onyxU32 index, onyxU32 inputValue) override;

        StringView GetName() const override { return "m_Axis 2D Composite"; }
        StringView GetInputBindingSlotName(onyxU32 index) const override;

    private:
        onyxU32 m_InputUp = 0;
        onyxU32 m_InputDown = 0;
        onyxU32 m_InputLeft = 0;
        onyxU32 m_InputRight = 0;
    };

    struct InputBindingAxis3DComposite : public InputBinding
    {
        static constexpr onyxU32 ID = TypeHash<InputBindingAxis3DComposite>();

        void Reset() override;

        onyxU32 GetId() const override { return ID; }
        bool UpdateBinding(const InputSystem& inputSystem, UniquePtr<InputBindingContext>& context) override;

        UniquePtr<InputBindingContext> CreateContext() const override { return MakeUnique<InputBindingAxis3DContext>(); }

        onyxS32 GetInputBindingSlotsCount() const override { return 6; }
        onyxU32 GetBoundInputForSlot(onyxU32 index) const override;
        void SetInputBindingSlot(onyxU32 index, onyxU32 inputValue) override;

        StringView GetName() const override { return "Axis 3D Composite"; }
        StringView GetInputBindingSlotName(onyxU32 index) const override;

    private:
        onyxU32 m_InputUp = 0;
        onyxU32 m_InputDown = 0;
        onyxU32 m_InputLeft = 0;
        onyxU32 m_InputRight = 0;
        onyxU32 m_InputForward = 0;
        onyxU32 m_InputBackward = 0;
    };

    struct InputBindingAxis1D : public InputBinding
    {
        static constexpr onyxU32 ID = TypeHash<InputBindingAxis1D>();

        void Reset() override { m_Axis = 0; }

        onyxU32 GetId() const override { return ID; }
        bool UpdateBinding(const InputSystem& inputSystem, UniquePtr<InputBindingContext>& context) override;

        UniquePtr<InputBindingContext> CreateContext() const override { return MakeUnique<InputBindingAxis1DContext>(); }

        onyxS32 GetInputBindingSlotsCount() const override { return 1; }
        onyxU32 GetBoundInputForSlot(onyxU32 /*index*/) const override { return m_Axis; }
        void SetInputBindingSlot(onyxU32 index, onyxU32 inputValue) override;

        StringView GetName() const override { return "Axis 1D"; }
        StringView GetInputBindingSlotName(onyxU32 /*index*/) const override { return ""; }

    private:
        // just for mouse / gamepad / joysticks
        onyxU32 m_Axis = 0;
    };

    struct InputBindingAxis2D : public InputBinding
    {
        static constexpr onyxU32 ID = TypeHash<InputBindingAxis2D>();

        void Reset() override;

        onyxU32 GetId() const override { return ID; }
        bool UpdateBinding(const InputSystem& inputSystem, UniquePtr<InputBindingContext>& context) override;

        UniquePtr<InputBindingContext> CreateContext() const override { return MakeUnique<InputBindingAxis2DContext>(); }

        onyxS32 GetInputBindingSlotsCount() const override { return 2; }
        onyxU32 GetBoundInputForSlot(onyxU32 index) const override;
        void SetInputBindingSlot(onyxU32 index, onyxU32 inputValue) override;

        StringView GetName() const override { return "Axis 2D"; }
        StringView GetInputBindingSlotName(onyxU32 index) const override;

    private:
        // just for mouse / gamepad / joysticks
        onyxU32 m_AxisX = 0;
        onyxU32 m_AxisY = 0;
    };

    struct InputBindingAxis3D : public InputBinding
    {
        static constexpr onyxU32 ID = TypeHash<InputBindingAxis3D>();

        void Reset() override;

        onyxU32 GetId() const override { return ID; }
        bool UpdateBinding(const InputSystem& inputSystem, UniquePtr<InputBindingContext>& context) override;

        UniquePtr<InputBindingContext> CreateContext() const override { return MakeUnique<InputBindingAxis3DContext>(); }

        onyxS32 GetInputBindingSlotsCount() const override { return 3; }
        onyxU32 GetBoundInputForSlot(onyxU32 index) const override;
        void SetInputBindingSlot(onyxU32 index, onyxU32 inputValue) override;

        StringView GetName() const override { return "Axis 3D"; }
        StringView GetInputBindingSlotName(onyxU32 index) const override;

    private:
        // just for mouse / gamepad / joysticks
        onyxU32 m_AxisX = 0;
        onyxU32 m_AxisY = 0;
        onyxU32 m_AxisZ = 0;
    };
}
