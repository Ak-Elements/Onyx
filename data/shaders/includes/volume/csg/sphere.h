#include "includes/volume/volumesources.h"
#include "includes/debug/print.h"

struct CsgSphere
{
    vec3 Position;
    float Radius;
};

void PackCsgSphere(WorldVolumeSources volumeSourcesData, CsgSphere sphere, uint sourceIndex)
{
    volumeSourcesData.SourcesData[sourceIndex] = sphere.Position.x;
    volumeSourcesData.SourcesData[sourceIndex + 1] = sphere.Position.y;
    volumeSourcesData.SourcesData[sourceIndex + 2] = sphere.Position.z;
    volumeSourcesData.SourcesData[sourceIndex + 3] = sphere.Radius;
}

CsgSphere UnpackCsgSphere(WorldVolumeSources volumeSourcesData, uint sourceIndex)
{
    CsgSphere sphere;
    sphere.Position.x = volumeSourcesData.SourcesData[sourceIndex];
    sphere.Position.y = volumeSourcesData.SourcesData[sourceIndex + 1];
    sphere.Position.z = volumeSourcesData.SourcesData[sourceIndex + 2];
    sphere.Radius = volumeSourcesData.SourcesData[sourceIndex + 3];

    return sphere;
}

vec4 GetValueAndGradient(vec3 worldPosition, CsgSphere sphere)
{
    const float Epsilon = 0.00001;

    vec3 gradient = worldPosition - sphere.Position;
    
    float distance = length(gradient);
    gradient = gradient * ( 1.0 / (distance + Epsilon));

    return vec4(gradient, distance - sphere.Radius);
}