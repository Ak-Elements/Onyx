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
    const String& getName() const { return m_name; }

    DynamicArray< InputAction >& getActions() { return m_actions; }
    const DynamicArray< InputAction >& getActions() const { return m_actions; }

    // StringId32 GetId() const { return m_Id; }
    Optional< InputAction* > getAction( StringId64 actionId );
    void removeAction( StringId64 actionId );

  private:
    bool hasInputAction( StringId64 actionId ) const;

  private:
    // StringId32 m=_Id;
    String m_name = "test";
    DynamicArray< InputAction > m_actions;
};
} // namespace onyx::input_actions
