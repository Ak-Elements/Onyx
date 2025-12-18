#include <onyx/volume/components/csg/spherecomponent.h>

#include <onyx/volume/source/csg/csgsphere.h>
#include <onyx/ui/propertygrid.h>

namespace Onyx::Volume
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    bool SphereComponent::DrawProperties(bool /*showHidden*/)
    {
        return Ui::PropertyGrid::DrawProperty("Radius", Radius);
    }
#endif
}
