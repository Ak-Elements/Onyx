#include <onyx/gamecore/components/graphics/materialcomponent.h>

#include <onyx/ui/propertygrid.h>

#include <onyx/graphics/shadergraph/materialshadergraph.h>

namespace Onyx::GameCore
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR

    bool MaterialComponent::DrawProperties(bool /*showHidden*/)
    {
        if (Ui::PropertyGrid::DrawProperty("Material", Material))
        {
            Material.Reset();
            return true;
        }

        return false;
    }

#endif
}