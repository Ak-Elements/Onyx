#include <onyx/gamecore/components/graphics/lightcomponents.h>

#include <onyx/ui/propertygrid.h>

namespace Onyx::GameCore
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    void DirectionalLightComponent::DrawImGuiEditor()
    {
        bool hasModified = Ui::PropertyGrid::DrawColorProperty("Color", Light.Color);

        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Intensity", Light.Intensity);
        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Shadow Amount", Light.ShadowAmount);
        hasModified |= Ui::PropertyGrid::DrawBoolProperty("Cast Shadows", Light.IsShadowCasting);
    }

    void PointLightComponent::DrawImGuiEditor()
    {
        bool hasModified = Ui::PropertyGrid::DrawColorProperty("Color", Light.Color);

        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Intensity", Light.Intensity);
        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Radius", Light.Radius);
        hasModified |= Ui::PropertyGrid::DrawBoolProperty("Cast Shadows", Light.IsShadowCasting);
    }

    void SpotLightComponent::DrawImGuiEditor()
    {
        bool hasModified = Ui::PropertyGrid::DrawColorProperty("Color", Light.Color);

        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Intensity", Light.Intensity);
        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Falloff", Light.Falloff);
        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Range", Light.Range);
        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Angle", Light.Angle);
        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Angle Attenuation", Light.AngleAttenuation);
        hasModified |= Ui::PropertyGrid::DrawBoolProperty("Cast Shadows", Light.IsShadowCasting);
    }
#endif
}
