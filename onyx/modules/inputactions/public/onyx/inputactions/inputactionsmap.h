#pragma once

#include <onyx/inputactions/inputaction.h>

namespace onyx {
enum class Key : uint16_t;
enum class GameControllerStick : uint8_t;
enum class GameControllerButton : uint16_t;
} // namespace onyx

namespace onyx::file_system {
struct JsonValue;
}

namespace onyx::input {
struct GameControllerButtonEvent;
struct GameControllerStickEvent;
struct KeyboardEvent;
} // namespace onyx::input

namespace onyx::input_actions {
struct InputActionsMap {
    friend struct Serialization< InputActionsMap >;

  public:
    const String& GetName() const { return m_Name; }

    DynamicArray< InputAction >& GetActions() { return m_Actions; }
    const DynamicArray< InputAction >& GetActions() const { return m_Actions; }

    // StringId32 GetId() const { return m_Id; }
    Optional< InputAction* > GetAction( StringId64 actionId );
    void RemoveAction( StringId64 actionId );

  private:
    bool HasInputAction( StringId64 actionId ) const;

  private:
    // StringId32 m=_Id;
    String m_Name = "test";
    DynamicArray< InputAction > m_Actions;
};
} // namespace onyx::input_actions
