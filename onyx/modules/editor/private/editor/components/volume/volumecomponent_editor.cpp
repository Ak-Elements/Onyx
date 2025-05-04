#include <onyx/volume/components/volumecomponent.h>

#include <onyx/ui/propertygrid.h>

#include <onyx/volume/source/csg/csgplane.h>
#include <onyx/volume/source/csg/csgsphere.h>
#include <onyx/volume/source/csg/csgcube.h>
#include <onyx/volume/source/noise/simplexnoisesource.h>

#include <imgui.h>

namespace Onyx::Volume
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    void VolumeComponent::DrawImGuiEditor()
    {
    }

    void VolumeSourceComponent::DrawImGuiEditor()
    {
        constexpr StringId32 SPHERE_TYPE("CSGSphere");
        constexpr StringId32 CUBE_TYPE ("CSGCube");
        constexpr StringId32 PLANE_TYPE("CSGPlane");
        constexpr StringId32 NOISE_TYPE("SimplexNoiseSource");

        Ui::PropertyGrid::DrawPropertyName("Type");
        Ui::PropertyGrid::DrawPropertyValue([&]()
        {
            StringView selectedName;
            if (VolumeType == SPHERE_TYPE)
                selectedName = "Sphere";
            else if (VolumeType == NOISE_TYPE)
                selectedName = "Simplex Noise";
            else if (VolumeType == CUBE_TYPE)
                selectedName = "Cube";
            else
                selectedName = "Plane";

            if (ImGui::BeginCombo("##Type", selectedName.data()))
            {
                if (ImGui::Selectable("Sphere", VolumeType == SPHERE_TYPE))
                {
                    if (VolumeType != SPHERE_TYPE)
                    {
                        VolumeType = SPHERE_TYPE;
                        delete Volume;
                        Volume = new CSGSphere();
                        IsModified = true;
                    }
                }

                if (ImGui::Selectable("Cube", VolumeType == CUBE_TYPE))
                {
                    if (VolumeType != CUBE_TYPE)
                    {
                        VolumeType = CUBE_TYPE;
                        delete Volume;
                        Volume = new CSGCube();
                        IsModified = true;
                    }
                }

                if (ImGui::Selectable("Plane", VolumeType == PLANE_TYPE))
                {
                    if (VolumeType != PLANE_TYPE)
                    {
                        VolumeType = PLANE_TYPE;
                        delete Volume;
                        Volume = new CSGPlane();
                        IsModified = true;
                    }
                }

                if (ImGui::Selectable("Simplex Noise", VolumeType == NOISE_TYPE))
                {
                    if (VolumeType != NOISE_TYPE)
                    {
                        VolumeType = NOISE_TYPE;
                        delete Volume;
                        Volume = new SimplexNoiseSource();
                        IsModified = true;
                    }
                }

                ImGui::EndCombo();
            }
        });

        if ((VolumeType.IsValid() == false) || (Volume == nullptr))
            return;

        if (VolumeType == SPHERE_TYPE)
        {
            CSGSphere* sphere = static_cast<CSGSphere*>(Volume);
            Vector3f center = sphere->GetCenter();
            if (Ui::PropertyGrid::DrawVector3Property("Center", center))
            {
                sphere->SetCenter(center);
                IsModified = true;
            }

            onyxF32 radius = sphere->GetRadius();
            if (Ui::PropertyGrid::DrawScalarProperty("Radius", radius))
            {
                sphere->SetRadius(radius);
                IsModified = true;
            }
        }
        if (VolumeType == CUBE_TYPE)
        {
            CSGCube* cube = static_cast<CSGCube*>(Volume);

            bool useV2 = cube->GetUseV2();
            if (Ui::PropertyGrid::DrawBoolProperty("Use V2", useV2))
            {
                cube->SetUseV2(useV2);
                IsModified = true;
            }

            Vector3f center = cube->GetCenter();
            if (Ui::PropertyGrid::DrawVector3Property("Center", center))
            {
                cube->SetCenter(center);
                IsModified = true;
            }

            Vector3f halfExtents = cube->GetHalfExtents();
            if (Ui::PropertyGrid::DrawVector3Property("Half extents", halfExtents))
            {
                cube->SetHalfExtents(halfExtents);
                IsModified = true;
            }
        }
        else if (VolumeType == PLANE_TYPE)
        {
            CSGPlane* plane = static_cast<CSGPlane*>(Volume);
            Vector3f normal = plane->GetNormal();
            if (Ui::PropertyGrid::DrawVector3Property("Normal", normal))
            {
                plane->SetNormal(normal);
                IsModified = true;
            }

            float distance = plane->GetDistance();
            if (Ui::PropertyGrid::DrawScalarProperty("Distance", distance))
            {
                plane->SetDistance(distance);
                IsModified = true;
            }
        }
        else if (VolumeType == NOISE_TYPE)
        {
            SimplexNoiseSource* noiseSource = static_cast<SimplexNoiseSource*>(Volume);

            SimplexNoiseSource::Dimension dimension = noiseSource->GetDimension();
            if (Ui::PropertyGrid::DrawEnumProperty("Dimension", dimension))
            {
                noiseSource->SetDimension(dimension);
                IsModified = true;
            }

            onyxU32 octaves = noiseSource->GetOctaves();
            if (Ui::PropertyGrid::DrawScalarProperty("Octaves", octaves))
            {
                noiseSource->SetOctaves(octaves);
                IsModified = true;
            }

            onyxF32 amplitude = noiseSource->GetAmplitude();
            if (Ui::PropertyGrid::DrawScalarProperty("Amplitude", amplitude))
            {
                noiseSource->SetAmplitude(amplitude);
                IsModified = true;
            }

            onyxF32 frequency = noiseSource->GetFrequency();
            if (Ui::PropertyGrid::DrawScalarProperty("Frequency", frequency))
            {
                noiseSource->SetFrequency(frequency);
                IsModified = true;
            }

            onyxF32 lacunarity = noiseSource->GetLacunarity();
            if (Ui::PropertyGrid::DrawScalarProperty("Lacunarity", lacunarity))
            {
                noiseSource->SetLacunarity(lacunarity);
                IsModified = true;
            }

            onyxF32 gain = noiseSource->GetGain();
            if (Ui::PropertyGrid::DrawScalarProperty("Gain", gain))
            {
                noiseSource->SetGain(gain);
                IsModified = true;
            }

            onyxF32 scale = noiseSource->GetScale();
            if (Ui::PropertyGrid::DrawScalarProperty("Scale", scale))
            {
                noiseSource->SetScale(scale);
                IsModified = true;
            }
        }
    }
#endif
}
