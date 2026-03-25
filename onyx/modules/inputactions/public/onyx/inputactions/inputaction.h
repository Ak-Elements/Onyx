#pragma once

#include <onyx/inputactions/bindings/inputbinding.h>

namespace onyx::input_actions {
enum class ActionType : uint8_t { Invalid = 0, Bool, Axis1D, Axis2D, Axis3D, Count };

struct InputAction {
    friend struct Serialization< InputAction >;

  public:
    InputAction();
    InputAction( StringId64 actionId );

    InputAction( const InputAction& other );
    InputAction& operator=( const InputAction& other );

    constexpr bool operator==( StringId64 actionId ) const { return m_Id == actionId; }

    constexpr StringId64 GetId() const { return m_Id; }
    ActionType GetType() const { return m_Type; }

    DynamicArray< UniquePtr< InputBinding > >& GetBindings() { return m_Bindings; }
    const DynamicArray< UniquePtr< InputBinding > >& GetBindings() const { return m_Bindings; }

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    void AddBinding( UniquePtr< InputBinding > binding ) { m_Bindings.emplace_back( std::move( binding ) ); }
    void RemoveBinding( uint32_t index );

    void SetId( StringId64 id ) { m_Id = id; }
    void SetType( ActionType type );
#endif

  private:
    StringId64 m_Id = 0;
    ActionType m_Type = ActionType::Invalid;

    DynamicArray< UniquePtr< InputBinding > > m_Bindings;
};
} // namespace onyx::input_actions
