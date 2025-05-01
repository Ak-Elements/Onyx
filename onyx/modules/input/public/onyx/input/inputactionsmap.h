#pragma once

#include <onyx/input/inputaction.h>

namespace Onyx
{
    enum class Key : onyxU16;
    enum class GameControllerStick : onyxU8;
    enum class GameControllerButton : onyxU16;
}

namespace Onyx::FileSystem
{
    struct JsonValue;
}

namespace Onyx::Input
{
    struct GameControllerButtonEvent;
    struct GameControllerStickEvent;
    struct KeyboardEvent;

    struct InputActionsMap
    {
    public:
        InputActionsMap(onyxU32 id, const StringView& name, const FileSystem::JsonValue& json);

        const String& GetName() const { return m_Name; }

        DynamicArray<InputAction>& GetActions() { return m_Actions; }
        const DynamicArray<InputAction>& GetActions() const { return m_Actions; }

        onyxU32 GetId() const { return m_Id; }

    private:
        bool HasInputAction(StringId64 actionId) const;

    private:
        onyxU32 m_Id;
        String m_Name;
        DynamicArray<InputAction> m_Actions;
    };
}
