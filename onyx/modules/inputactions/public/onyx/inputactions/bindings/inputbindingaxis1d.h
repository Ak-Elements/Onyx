#pragma once

#include <onyx/input/inputid.h>
#include <onyx/inputactions/bindings/inputbinding.h>

namespace onyx::input_actions {
class InputBindingAxis1D : public InputBinding {
    friend struct Serialization< InputBindingAxis1D >;

  public:
    static constexpr StringId32 TypeId{ "onyx::input_bindings::Axis1D" };
    StringId32 GetTypeId() const override { return TypeId; }

    void Reset() override { m_Axis = 0; }

    int32_t GetInputBindingSlotsCount() const override { return 1; }
    input::InputID GetBoundInputForSlot( uint32_t /*index*/ ) const override { return m_Axis; }
    void SetInputBindingSlot( uint32_t index, input::InputID inputID ) override;

    StringView GetName() const override { return "Axis 1D"; }
    StringView GetInputBindingSlotName( uint32_t /*index*/ ) const override { return ""; }

  private:
    bool DoUpdate( const input::InputSystem& inputSystem, Vector3f32& outInputValue ) override;

  private:
    // just for mouse / gamepad / joysticks
    input::InputID m_Axis;
};
} // namespace onyx::input_actions

namespace onyx {
template <>
struct Serialization< input_actions::InputBindingAxis1D > {
    static bool serialize( Serializer& serializer, const input_actions::InputBindingAxis1D& binding );
    static bool deserialize( const Deserializer& deserializer, input_actions::InputBindingAxis1D& outBinding );
};
} // namespace onyx
