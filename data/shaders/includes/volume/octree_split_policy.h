#include "includes/volume/sample_terrain.h"

float Interpolate(float f000, float f001, float f010, float f011,
    float f100, float f101, float f110, float f111, vec3 position)
{
    const float oneMinX = 1.0f - position.x;
    const float oneMinY = 1.0f - position.y;
    const float oneMinZ = 1.0f - position.z;
    const float oneMinXoneMinY = oneMinX * oneMinY;
    const float xOneMinY = position.x * oneMinY;
    return oneMinZ * (f000* oneMinXoneMinY
        + f100 * xOneMinY
        + f010 * oneMinX * position.y)
        + position.z * (f001* oneMinXoneMinY
            + f101 * xOneMinY
            + f011 * oneMinX * position.y)
        + position.x * position.y * (f110* oneMinZ
            + f111 * position.z);
}

bool HasComplexSurface(vec4 cornerSamples[8], float complexSurfaceThreshold)
{
    for (uint i = 0; i < 7; ++i)
    {
        for (uint j = i + 1; j < 8; ++j)
        {
            if (dot(cornerSamples[i].xyz, cornerSamples[j].xyz) < complexSurfaceThreshold)
            {
                return true;
            }
        }
    }

    return false;
}

bool HasGeometricError(vec3 center, vec3 corner[8], vec4 cornerSamples[8], float maxGeometricError, in WorldVolumeSourcesList volumeSourcesList, in WorldVolumeSources volumeSourcesData)
{
    // Error metric of http://www.andrew.cmu.edu/user/jessicaz/publication/meshing/
    const float f000 = cornerSamples[0].w;
    const float f001 = cornerSamples[1].w;
    const float f010 = cornerSamples[2].w;
    const float f011 = cornerSamples[3].w;
    const float f100 = cornerSamples[4].w;
    const float f101 = cornerSamples[5].w;
    const float f110 = cornerSamples[6].w;
    const float f111 = cornerSamples[7].w;

    const vec3 from = corner[0];
    const vec3 to = corner[7];

    const vec3 samplePositions[19][2] =
    {
        { vec3(center[0], from[1], from[2]), vec3(0.5f, 0.0f, 0.0f) }, // center back bottom
        { vec3(from[0], from[1], center[2]), vec3(0.0f, 0.0f, 0.5f) }, // center left bottom
        { vec3(center[0], from[1], center[2]), vec3(0.5f, 0.0f, 0.5f) }, // center bottom
        { vec3(to[0], from[1], center[2]), vec3(1.0f, 0.0f, 0.5f) }, // center right bottom
        { vec3(center[0], from[1], to[2]), vec3(0.5f, 0.0f, 1.0f) }, // center front bottom

        { vec3(from[0], center[1], from[2]), vec3(0.0f, 0.5f, 0.0f) }, // center back left
        { vec3(center[0], center[1], from[2]), vec3(0.5f, 0.5f, 0.0f) }, // center back
        { vec3(to[0], center[1], from[2]), vec3(1.0f, 0.5f, 0.0f) }, // center back right
        { vec3(from[0], center[1], center[2]), vec3(0.0f, 0.5f, 0.5f) }, // center left

        { center, vec3(0.5f, 0.5f, 0.5f) }, // center
        { vec3(to[0], center[1], center[2]), vec3(1.0f, 0.5f, 0.5f) }, // center right
        { vec3(from[0], center[1], to[2]), vec3(0.0f, 0.5f, 1.0f) }, // center front left
        { vec3(center[0], center[1], to[2]), vec3(0.5f, 0.5f, 1.0f) }, // center front
        { vec3(to[0], center[1], to[2]), vec3(1.0f, 0.5f, 1.0f) }, // center front right

        { vec3(center[0], to[1], from[2]), vec3(0.5f, 1.0f, 0.0f) }, // center back top
        { vec3(from[0], to[1], center[2]), vec3(0.0f, 1.0f, 0.5f) }, // center left top
        { vec3(center[0], to[1], center[2]), vec3(0.5f, 1.0f, 0.5f) }, // center top
        { vec3(to[0], to[1], center[2]), vec3(1.0f, 1.0f, 0.5f) }, // center right top
        { vec3(center[0], to[1], to[2]), vec3(0.5f, 1.0f, 1.0f) } // center front top
    };

    float error = 0.0f;
    vec4 value;
    vec3 gradient;

    for (uint i = 0; i < 19; ++i)
    {
        value = SampleTerrain(samplePositions[i][0], volumeSourcesList, volumeSourcesData);
        gradient[0] = value[0];
        gradient[1] = value[1];
        gradient[2] = value[2];
        float interpolated = Interpolate(f000, f001, f010, f011, f100, f101, f110, f111, samplePositions[i][1]);
        float gradientMagnitude = length(gradient);
        if (gradientMagnitude < 0.0001)
        {
            gradientMagnitude = 1.0f;
        }
        error += abs(value.w - interpolated) / gradientMagnitude;
        if (error >= maxGeometricError)
        {
            return true;
        }
    }

    return false;
}

OctreeNode EvaluateOctreeNode(vec3 cameraPosition, vec3 nodePosition, float nodeExtents, float maxGeometricError, float complexSurfaceThreshold, in WorldVolumeSourcesList volumeSourcesList, in WorldVolumeSources volumeSourcesData, out uint childCount)
{
    OctreeNode octreeNode; 
    octreeNode.LeafMask = 0;
    octreeNode.ChildrenOffset = 0;
    octreeNode.ValidMask = 0;
    octreeNode.IsFar = false;

    childCount = 0;

    float halfExtents = nodeExtents * 0.5;
    float LodChildSize = 128.0f;
    float distanceToCamera = length(nodePosition - cameraPosition);
    
    if (distanceToCamera < 2.0f)
    {
        LodChildSize = 0.05f;
    }
    if (distanceToCamera < 20)
    {
        LodChildSize = 0.1f;
    }
    else if (distanceToCamera < 50)
    {
        LodChildSize = 1.0f;
    }
    else if (distanceToCamera < 512)
    {
        LodChildSize = 4.0f;
    }
    else if (distanceToCamera < 2048)
    {
        LodChildSize = 8.0f;
    }
    else if (distanceToCamera < 8096)
    {
        LodChildSize = 8.0f;
    }
    else if (distanceToCamera < 16000)  
    {
        LodChildSize = 16;
    }
    else
    {
        LodChildSize = 128;
    }
    
    if (halfExtents < LodChildSize)
    {
        octreeNode.LeafMask = 255;
        return octreeNode;
    }

    vec3 corner0 = nodePosition + vec3(-0.5,-0.5,-0.5) * nodeExtents;
    vec3 corner7 = nodePosition + vec3( 0.5, 0.5, 0.5) * nodeExtents;

    float diagonal = length(corner7 - corner0);
    vec4 centerSample = SampleTerrain(nodePosition, volumeSourcesList, volumeSourcesData);
    bool shouldSplit = abs(centerSample.w) <= diagonal;
    if (shouldSplit == false)
    {
        octreeNode.LeafMask = 255;
        return octreeNode;
    }

    vec3 corners[8] = 
    {
        corner0,
        vec3( nodePosition + vec3(-0.5,-0.5, 0.5) * nodeExtents ),
        vec3( nodePosition + vec3(-0.5, 0.5,-0.5) * nodeExtents ), 
        vec3( nodePosition + vec3(-0.5, 0.5, 0.5) * nodeExtents ), 
        vec3( nodePosition + vec3( 0.5,-0.5,-0.5) * nodeExtents ), 
        vec3( nodePosition + vec3( 0.5,-0.5, 0.5) * nodeExtents ),
        vec3( nodePosition + vec3( 0.5, 0.5,-0.5) * nodeExtents ),
        corner7
    };

    vec4 cornerSamples[8] =
    {
        SampleTerrain(corner0, volumeSourcesList, volumeSourcesData),
        SampleTerrain(corners[1], volumeSourcesList, volumeSourcesData),
        SampleTerrain(corners[2], volumeSourcesList, volumeSourcesData),
        SampleTerrain(corners[3], volumeSourcesList, volumeSourcesData),
        SampleTerrain(corners[4], volumeSourcesList, volumeSourcesData),
        SampleTerrain(corners[5], volumeSourcesList, volumeSourcesData),
        SampleTerrain(corners[6], volumeSourcesList, volumeSourcesData),
        SampleTerrain(corner7, volumeSourcesList, volumeSourcesData)
    };

    if (HasComplexSurface(cornerSamples, complexSurfaceThreshold))
    {
        octreeNode.ValidMask = 255;
        childCount = 8;
        return octreeNode;
    }
    
    if (HasGeometricError(nodePosition, corners, cornerSamples, maxGeometricError, volumeSourcesList, volumeSourcesData))
    {
        octreeNode.ValidMask = 255;
        childCount = 8;
        return octreeNode;
    }
    
    octreeNode.LeafMask = 255;
    return octreeNode;
}