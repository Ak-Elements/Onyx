#include <onyx/input/inputaction.h>

// Included to please c++ unique ptr forward declaration
#include <onyx/input/bindings/inputbinding.h>
namespace Onyx::Input
{
    InputAction::InputAction() = default;

    InputAction::InputAction(StringId64 actionId)
        : m_Id(actionId)
    {
    }

    InputAction::~InputAction() = default;

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    void InputAction::SetName(StringView name)
    {
        m_Id = StringId64(name);
    }
#endif
}
