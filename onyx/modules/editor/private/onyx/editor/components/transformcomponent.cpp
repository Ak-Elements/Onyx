#include <onyx/gamecore/components/transformcomponent.h>

#include <onyx/ui/propertygrid.h>

namespace Onyx::GameCore
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    bool TransformComponent::DrawProperties(bool /*showHidden*/)
    {
        bool hasModified = false;
        hasModified = Ui::PropertyGrid::DrawProperty("Translation", m_Translation);

        Ui::PropertyGrid::SetNextPropertyTooltip("Rotation in degrees");
        Vector3f32 rotationInDegrees = ToDegrees(m_RotationEuler);
        if (Ui::PropertyGrid::DrawProperty("Rotation", rotationInDegrees))
        {
            hasModified = true;
            SetRotation(ToRadians(rotationInDegrees));
        }

        hasModified |= Ui::PropertyGrid::DrawProperty("Scale", m_Scale);

        return hasModified;
    }
#endif
}
