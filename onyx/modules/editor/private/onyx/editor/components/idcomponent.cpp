#include <onyx/gamecore/components/idcomponent.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/ui/scopeddisable.h>

namespace Onyx::GameCore
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    bool IdComponent::DrawProperties(bool /*showHidden*/)
    {
        Ui::ScopedImGuiDisabled disable;
        Ui::PropertyGrid::DrawProperty("Runtime Id", Id);
        return false;
    }
#endif
}
