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

vec4 RaiseLowerBrush(vec3 worldPosition, vec3 brushCenter, float radius, float strength)
{
    vec3 direction = normalize(worldPosition - brushCenter);
    float distance = length(worldPosition - brushCenter);
    float falloff = 1 - smoothstep(0.0, radius, distance);
    float delta = -strength * falloff;
    return vec4(direction * delta, delta);
}

vec4 GetSampleValue(vec3 worldPosition)
{
    // move to sample function
    vec3 planeNormal = vec3(0,1,0);
    CsgPlane plane = CsgPlane(vec3(0,1,0), 10);
    CsgSphere sphere = CsgSphere(vec3(0,0,-1000), 500);

    vec4 planeGradient = GetValueAndGradient(worldPosition, plane);
    vec4 sphereGradient = GetValueAndGradient(worldPosition, sphere);
    
    vec4 finalGradient = GetUnion(planeGradient, sphereGradient);

    for (uint i = 0; i < u_Constants.VolumeSourcesList.Count; ++i)
    {
        VolumeSource source = u_Constants.VolumeSourcesList.Sources[i];
        vec4 sourceGradient;
        uint dataStartIndex = source.Index * VolumeSources_ItemSize;
        
        float strength = -1.0f;
        float smoothness = -1.0f;

        switch (uint(source.Type))
        {
            case VolumeSource_Sphere:
            {
                CsgSphere sphere = UnpackCsgSphere(u_Constants.VolumeSourcesData, dataStartIndex);
                sourceGradient = GetValueAndGradient(worldPosition, sphere);
                break;
            }
            case VolumeSource_Cube:
            {
                CsgCube cube = UnpackCsgCube(u_Constants.VolumeSourcesData, dataStartIndex);
                sourceGradient = GetValueAndGradient(worldPosition, cube);
                break;
            }
            case VolumeSource_Ellipsoid:
            {
                CsgEllipsoid ellipsoid;
                UnpackCsgSource(u_Constants.VolumeSourcesData, dataStartIndex, ellipsoid);
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
                CsgSphere sphere = UnpackCsgSphere(u_Constants.VolumeSourcesData, dataStartIndex);
                strength = u_Constants.VolumeSourcesData.SourcesData[dataStartIndex + 4];
                smoothness = u_Constants.VolumeSourcesData.SourcesData[dataStartIndex + 5];
                
                sourceGradient = GetValueAndGradient(worldPosition, sphere);
                break;
            }
            case VolumeSource_CubeBrush:
            {
                CsgCube cube = UnpackCsgCube(u_Constants.VolumeSourcesData, dataStartIndex);
                strength = u_Constants.VolumeSourcesData.SourcesData[dataStartIndex + 6];
                smoothness = u_Constants.VolumeSourcesData.SourcesData[dataStartIndex + 7];
                sourceGradient = GetValueAndGradient(worldPosition, cube);
                break;
            }
        }

        switch (uint(source.Operation))
        {
            case VolumeOperation_Union:
            {
                if (smoothness < 0.0f)
                    finalGradient = GetUnion(sourceGradient, finalGradient);
                else
                    finalGradient = GetUnionSmooth(sourceGradient, finalGradient, smoothness);
                break;
            }
            case VolumeOperation_Difference:
            {
                if (smoothness < 0.0f)
                    finalGradient = GetDifference(sourceGradient, finalGradient);
                else
                    finalGradient = -GetUnionSmooth(sourceGradient, -finalGradient, smoothness); 
                break;
            }
            case VolumeOperation_Intersect:
            {
                finalGradient = GetIntersection(sourceGradient, finalGradient);
                break;
            }
        }
    }

    return finalGradient;
}