#include <onyx/volume/components/csg/spherecomponent.h>

#include <onyx/volume/source/csg/csgsphere.h>
#include <onyx/ui/propertygrid.h>

namespace Onyx::Volume
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    bool SphereComponent::DrawImGuiEditor()
    {
        return Ui::PropertyGrid::DrawScalarProperty("Radius", Radius);
    }
#endif
}
