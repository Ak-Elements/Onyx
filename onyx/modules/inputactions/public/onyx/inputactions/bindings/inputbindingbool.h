#pragma once

#include <onyx/input/inputid.h>
#include <onyx/inputactions/bindings/inputbinding.h>

namespace onyx::input_actions {
class InputBindingBool : public InputBinding {
    friend struct Serialization< InputBindingBool >;

  public:
    static constexpr StringId32 TypeId{ "onyx::input_bindings::Bool" };
    StringId32 GetTypeId() const override { return TypeId; }

    void Reset() override { m_Input = input::InputID::Invalid; }

    int32_t GetInputBindingSlotsCount() const override { return 1; }
    input::InputID GetBoundInputForSlot( uint32_t ) const override { return m_Input; }
    void SetInputBindingSlot( uint32_t, input::InputID inputId ) override { m_Input = inputId; }

    StringView GetName() const override { return "Bool"; }
    StringView GetInputBindingSlotName( uint32_t ) const override { return ""; }

  private:
    bool DoUpdate( const input::InputSystem& inputSystem, Vector3f32& outInputValue ) override;

  private:
    input::InputID m_Input;
};
} // namespace onyx::input_actions

namespace onyx {
template <>
struct Serialization< input_actions::InputBindingBool > {
    static bool serialize( Serializer& serializer, const input_actions::InputBindingBool& binding );
    static bool deserialize( const Deserializer& deserializer, input_actions::InputBindingBool& outBinding );
};
} // namespace onyx
