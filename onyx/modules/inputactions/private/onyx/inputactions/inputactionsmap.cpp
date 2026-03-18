#include <onyx/inputactions/inputactionsmap.h>

#include <onyx/inputactions/inputaction.h>
#include <onyx/filesystem/onyxfile.h>

namespace onyx::input_actions
{
    Optional<InputAction*> InputActionsMap::GetAction(StringId64 actionId)
    {
        for (InputAction& action : m_Actions)
        {
            if (action.GetId() == actionId)
                return &action;
        }

        return std::nullopt;
    }

    void InputActionsMap::RemoveAction(StringId64 actionId)
    {
        std::erase_if(m_Actions, [&](const InputAction& action)
        {
            return action.GetId() == actionId;
        });
    }

    bool InputActionsMap::HasInputAction(StringId64 actionId) const
    {
        for (const InputAction& action : m_Actions)
        {
            if (action.GetId() == actionId)
                return true;
        }

        return false;
    }
}
