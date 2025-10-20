#include "includes/volume/csg/plane.h"
#include "includes/volume/csg/sphere.h"
#include "includes/volume/csg/cube.h"
#include "includes/volume/csg/ellipsoid.h"
#include "includes/volume/csg/simplexnoise2d.h"

#include "includes/volume/csg/operations/difference.h"
#include "includes/volume/csg/operations/intersect.h"
#include "includes/volume/csg/operations/union.h"

#include "includes/volume/volumesources.h"
#include "includes/debug/print.h"

// forward declare - defined in default_terrain.h or a custom volumegraph
vec4 SampleBaseTerrainValue(vec3 worldPosition);

vec4 RaiseLowerBrush(vec3 worldPosition, vec3 brushCenter, float radius, float strength)
{
    vec3 direction = normalize(worldPosition - brushCenter);
    float distance = length(worldPosition - brushCenter);
    float falloff = 1 - smoothstep(0.0, radius, distance);
    float delta = -strength * falloff;
    return vec4(direction * delta, delta);
}

vec4 SampleVolumeSources(vec3 worldPosition, in vec4 terrainSample, in WorldVolumeSourcesList volumeSourcesList, in WorldVolumeSources volumeSourcesData)
{
    for (uint i = 0; i < volumeSourcesList.Count; ++i)
    {
        VolumeSource source = volumeSourcesList.Sources[i];
        vec4 sourceGradient;
        uint dataStartIndex = source.Index * VolumeSources_ItemSize;
        
        float strength = -1.0f;
        float smoothness = -1.0f;

        switch (uint(source.Type))
        {
            case VolumeSource_Sphere:
            {
                CsgSphere sphere = UnpackCsgSphere(volumeSourcesData, dataStartIndex);
                sourceGradient = GetValueAndGradient(worldPosition, sphere);
                break;
            }
            case VolumeSource_Cube:
            {
                CsgCube cube = UnpackCsgCube(volumeSourcesData, dataStartIndex);
                sourceGradient = GetValueAndGradient(worldPosition, cube);
                break;
            }
            case VolumeSource_Ellipsoid:
            {
                CsgEllipsoid ellipsoid;
                UnpackCsgSource(volumeSourcesData, dataStartIndex, ellipsoid);
                sourceGradient = GetValueAndGradient(worldPosition, ellipsoid);
                break;
            }
            case VolumeSource_Plane:
            {
                //union = GetUnion(union, )
                break;
            }
            case VolumeSource_Grid:
            {
                //union = GetUnion(union, )
                break;
            }
            // TODO: Remove and merge into grid source
            case VolumeSource_SphereBrush:
            {
                CsgSphere sphere = UnpackCsgSphere(volumeSourcesData, dataStartIndex);
                strength = volumeSourcesData.SourcesData[dataStartIndex + 4];
                smoothness = volumeSourcesData.SourcesData[dataStartIndex + 5];
                
                sourceGradient = GetValueAndGradient(worldPosition, sphere);
                break;
            }
            case VolumeSource_CubeBrush:
            {
                CsgCube cube = UnpackCsgCube(volumeSourcesData, dataStartIndex);
                strength = volumeSourcesData.SourcesData[dataStartIndex + 6];
                smoothness = volumeSourcesData.SourcesData[dataStartIndex + 7];
                sourceGradient = GetValueAndGradient(worldPosition, cube);
                break;
            }
        }

        switch (uint(source.Operation))
        {
            case VolumeOperation_Union:
            {
                if (smoothness < 0.0f)
                    terrainSample = GetUnion(sourceGradient, terrainSample);
                else
                    terrainSample = GetUnionSmooth(sourceGradient, terrainSample, smoothness);
                break;
            }
            case VolumeOperation_Difference:
            {
                if (smoothness < 0.0f)
                    terrainSample = GetDifference(sourceGradient, terrainSample);
                else
                    terrainSample = -GetUnionSmooth(sourceGradient, -terrainSample, smoothness); 
                break;
            }
            case VolumeOperation_Intersect:
            {
                terrainSample = GetIntersection(sourceGradient, terrainSample);
                break;
            }
        }
    }
    
    return terrainSample;
}

vec4 SampleTerrain(vec3 worldPosition, in WorldVolumeSourcesList volumeSourcesList, in WorldVolumeSources volumeSourcesData)
{
    vec4 baseTerrainSample = SampleBaseTerrainValue(worldPosition);
    vec4 terrainSample = SampleVolumeSources(worldPosition, baseTerrainSample, volumeSourcesList, volumeSourcesData);
    return terrainSample;
}