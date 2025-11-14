#include "includes/volume/sample_terrain.h"

const uint cornerIndices[8][8] =
{
    { 0, 2, 9, 12, 1, 4, 10, 13 },      // 0
    { 1, 4, 10, 13, 5, 6, 11, 14 },     // 1
    { 9, 12, 18, 20, 10, 13, 19, 23 },  // 2
    { 10, 13, 19, 23, 11, 14, 22, 24 }, // 3
    { 2, 3, 12, 15, 4, 8, 13, 16 },     // 4
    { 4, 8, 13, 16, 6, 7, 14, 17 },     // 5
    { 12, 15, 20, 21, 13, 16, 23, 25},  // 6
    { 13, 16, 23, 25, 14, 17, 24, 26 }, // 7
};

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

bool ShouldSubdivide(uint childIndex, vec3 childCornerPositions[27], vec4 childCornerSamples[27], float maxGeometricError, float complexSurfaceThreshold, in WorldVolumeSourcesList volumeSourcesList, in WorldVolumeSources volumeSourcesData)
{
    vec3 corner0 = childCornerPositions[cornerIndices[childIndex][0]];
    vec3 corner7 = childCornerPositions[cornerIndices[childIndex][7]];
    vec3 childCenter = corner0 + (corner7 - corner0) * 0.5f;

    float diagonal = length(corner7 - corner0);
    vec4 centerSample = SampleTerrain(childCenter, volumeSourcesList, volumeSourcesData);
    bool shouldSplit = abs(centerSample.w) <= diagonal;
    if (shouldSplit == false)
    {
        return false;
    }

    vec3 corners[8] = 
    {
        corner0,
        childCornerPositions[cornerIndices[childIndex][1]],
        childCornerPositions[cornerIndices[childIndex][2]],
        childCornerPositions[cornerIndices[childIndex][3]],
        childCornerPositions[cornerIndices[childIndex][4]],
        childCornerPositions[cornerIndices[childIndex][5]],
        childCornerPositions[cornerIndices[childIndex][6]],
        corner7
    };

    vec4 cornerSamples[8] =
    {
        childCornerSamples[cornerIndices[childIndex][0]],
        childCornerSamples[cornerIndices[childIndex][1]],
        childCornerSamples[cornerIndices[childIndex][2]],
        childCornerSamples[cornerIndices[childIndex][3]],
        childCornerSamples[cornerIndices[childIndex][4]],
        childCornerSamples[cornerIndices[childIndex][5]],
        childCornerSamples[cornerIndices[childIndex][6]],
        childCornerSamples[cornerIndices[childIndex][7]]
    };

    if (HasComplexSurface(cornerSamples, complexSurfaceThreshold))
    {
        return true;
    }
    
    if (HasGeometricError(childCenter, corners, cornerSamples, maxGeometricError, volumeSourcesList, volumeSourcesData))
    {
        return true;
    }
    return false;
}

uint RoundDownToPowerOfTwo(uint x)
{
    if (x == 0) return 0;
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return x - (x >> 1);
}
float CalculateLodChildSize(float distanceToCamera)
{
    if (distanceToCamera < 16.0f)
        return 1.0f;

    float base = 16.0f;
    float exponent = 1.1f;
    float scale = 0.4f; // ← increased from 0.6f

    float size = pow(distanceToCamera / base, exponent) * scale;

    uint rounded = RoundDownToPowerOfTwo(uint(size));
    return float(min(rounded, 1024u));
}

OctreeNode EvaluateOctreeNode(vec3 cameraPosition, vec3 nodePosition, float nodeExtents, float maxGeometricError, float complexSurfaceThreshold, in WorldVolumeSourcesList volumeSourcesList, in WorldVolumeSources volumeSourcesData, out uint childCount)
{
    OctreeNode octreeNode; 
    octreeNode.LeafMask = 0;
    octreeNode.ValidMask = 0;
    octreeNode.ChildrenOffset = 0;
    octreeNode.IsFar = false;

    childCount = 0;

    // Scale geometric error based on distance and FOV
   

    float halfExtents = nodeExtents;
    float LodChildSize = 0.1f;
    float distanceToCamera = length(nodePosition - cameraPosition);
    
    float fov = radians(45.0f);
    float screenHeight = 900;
    float exponent = 2.0f;
    maxGeometricError = max((maxGeometricError * (pow(distanceToCamera, exponent)) * 2.0f * tan(fov* 0.5f)) / screenHeight, 0.2f);

    if (halfExtents > 8000)
    {

        childCount = 8;
        octreeNode.ValidMask = 255;
        return octreeNode;
    }

    LodChildSize = CalculateLodChildSize(distanceToCamera);
    
    if (halfExtents <= LodChildSize)
    {
        octreeNode.LeafMask = 255;
        return octreeNode;
    }
 
    // evaluate each child to get a valid and leaf mask
    // sample all child corner positions
    vec3 corner0 = nodePosition + -0.5f * nodeExtents;
    vec3 corner1 = vec3( nodePosition + vec3(-0.5,-0.5, 0.5) * nodeExtents );
    vec3 corner2 = vec3( nodePosition + vec3(-0.5, 0.5,-0.5) * nodeExtents );
    vec3 corner3 = vec3( nodePosition + vec3(-0.5, 0.5, 0.5) * nodeExtents );
    vec3 corner4 = vec3( nodePosition + vec3( 0.5,-0.5,-0.5) * nodeExtents );
    vec3 corner5 = vec3( nodePosition + vec3( 0.5,-0.5, 0.5) * nodeExtents );
    vec3 corner6 = vec3( nodePosition + vec3( 0.5, 0.5,-0.5) * nodeExtents );
    vec3 corner7 = nodePosition + 0.5f * nodeExtents;

    const vec3 childCornerPositions[27] =
    {
        // bottom face
        corner0,
        vec3(nodePosition.x, corner0.y, corner0.z), // center back bottom
        vec3(corner0.x, corner0.y, nodePosition.z), // center left bottom
        corner1,
        vec3(nodePosition.x, corner0.y, nodePosition.z), // center bottom
        corner4,
        vec3(corner7.x, corner0.y, nodePosition.z), // center right bottom
        corner5,
        vec3(nodePosition.x, corner0.y, corner7.z), // center front bottom

        vec3(corner0.x, nodePosition.y, corner0.z), // center back left
        vec3(nodePosition.x, nodePosition.y, corner0.z), // center back
        vec3(corner7.x, nodePosition.y, corner0.z), // center back right
        vec3(corner0.x, nodePosition.y, nodePosition.z), // center left

        nodePosition, // center
        vec3(corner7.x, nodePosition.y, nodePosition.z), // center right
        vec3(corner0.x, nodePosition.y, corner7.z), // center front left
        vec3(nodePosition.x, nodePosition.y, corner7.z), // center front
        vec3(corner7.x, nodePosition.y, corner7.z), // center front right

        // top face
        corner2,
        vec3(nodePosition.x, corner7.y, corner0.z), // center back top
        vec3(corner0.x, corner7.y, nodePosition.z), // center left top
        corner3,
        corner6,
        vec3(nodePosition.x, corner7.y, nodePosition.z), // center top
        vec3(corner7.x, corner7.y, nodePosition.z), // center right top
        vec3(nodePosition.x, corner7.y, corner7.z), // center front top
        corner7
    };

    vec4 childCornerSamples[27] =
    {
        SampleTerrain(childCornerPositions[ 0], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[ 1], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[ 2], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[ 3], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[ 4], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[ 5], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[ 6], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[ 7], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[ 8], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[ 9], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[10], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[11], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[12], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[13], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[14], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[15], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[16], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[17], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[18], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[19], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[20], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[21], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[22], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[23], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[24], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[25], volumeSourcesList, volumeSourcesData),
        SampleTerrain(childCornerPositions[26], volumeSourcesList, volumeSourcesData),
    };

    for (uint i = 0; i < 8; ++i)
    {
        if (ShouldSubdivide(i, childCornerPositions, childCornerSamples, maxGeometricError, complexSurfaceThreshold, volumeSourcesList, volumeSourcesData))
        {
            octreeNode.ValidMask |= 1 << i;
            ++childCount;
        }
        else
        {
            octreeNode.LeafMask |= 1 << i;
        }
    }

    //octreeNode.LeafMask = 255;
    return octreeNode;
}