#pragma once

#include <onyx/input/inputbinding.h>

#include <onyx/filesystem/onyxfile.h>

namespace Onyx::Input
{
    struct InputBinding;

    struct InputAction
    {
    public:
        InputAction();
        InputAction(StringId64 actionId);

        constexpr bool operator==(StringId64 actionId) const { return m_Id == actionId; }
        
        constexpr StringId64 GetId() const { return m_Id; }
        ActionType GetType() const { return m_Type; }
        
        DynamicArray<UniquePtr<InputBinding>>& GetBindings() { return m_Bindings; }
        const DynamicArray<UniquePtr<InputBinding>>& GetBindings() const { return m_Bindings; }

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        void SetName(StringView name);
        void SetType(ActionType type) { m_Type = type; }
#endif

        static bool FromJson(const FileSystem::JsonValue& json, InputAction& outAction);

    private:
        StringId64 m_Id = 0;
        ActionType m_Type = ActionType::Invalid;

        DynamicArray<UniquePtr<InputBinding>> m_Bindings;
    };
}
