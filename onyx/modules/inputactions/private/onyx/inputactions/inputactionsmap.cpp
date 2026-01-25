#include <onyx/inputactions/inputactionsmap.h>

#include <onyx/inputactions/inputaction.h>
#include <onyx/filesystem/onyxfile.h>

namespace Onyx::InputActions
{
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
