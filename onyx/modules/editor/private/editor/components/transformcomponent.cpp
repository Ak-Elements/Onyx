#include <onyx/gamecore/components/transformcomponent.h>

#include <onyx/ui/propertygrid.h>

namespace Onyx::GameCore
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    void TransformComponent::DrawImGuiEditor()
    {
        bool hasModified = false;
        hasModified = Ui::PropertyGrid::DrawVector3Property("Translation", m_Translation);

        Vector3f rotationInDegrees = ToDegrees(m_RotationEuler);
        if (Ui::PropertyGrid::DrawVector3Property("Rotation", rotationInDegrees))
        {
            hasModified = true;
            SetRotation(ToRadians(rotationInDegrees));
        }

        hasModified |= Ui::PropertyGrid::DrawVector3Property("Scale", m_Scale);

        if (hasModified)
        {
            // recalculate transform matrix
        }
    }
#endif
}
