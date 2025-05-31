#include <onyx/input/inputaction.h>
#include <onyx/input/inputbinding.h>
#include <onyx/input/inputbindingsregistry.h>

namespace Onyx::Input
{
    InputAction::InputAction() = default;

    InputAction::InputAction(StringId64 actionId)
        : m_Id(actionId)
    {
    }


#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    void InputAction::SetName(StringView name)
    {
        m_Id = StringId64(name);
    }
#endif
}
