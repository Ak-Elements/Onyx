#include <onyx/input/inputactionsmap.h>

#include <onyx/input/inputaction.h>
#include <onyx/filesystem/onyxfile.h>

namespace Onyx::Input
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
