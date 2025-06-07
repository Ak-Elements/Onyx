#include "includes/volume/marchingsquares.h"

const float ISO_LEVEL = 0.0f;

struct Vertex
{
    vec4 Position;
    vec4 Normal;
};

struct LineSegment
{
    Vertex LinePoints[4];
    uint LinePointsCount;

    uint FromEdgeIndex;
    uint ToEdgeIndex;
};

struct Component
{
    Vertex Vertices[12];
    uint VertexCount;
};

bool ResolveFaceSharpFeature(uint faceIndex, vec3 fromPosition, vec3 fromNormal, vec3 toPosition, vec3 toNormal, out vec3 intersectionPosition, out vec3 intersectionNormal)
{
    return false;
}

void CalculateZeroCrossing(uint edgeIndex, vec3 corners[4], vec4 hermiteData[4], out Vertex outVertex)
{
    const uint v0 = VERTEX_MAP[edgeIndex].x;
    const uint v1 = VERTEX_MAP[edgeIndex].y;

    const float isoVal0 = hermiteData[v0].w;
    const float isoVal1 = hermiteData[v1].w;

    // linear interpolation to get the point on the surface and the normal
    const float interpolated = (ISO_LEVEL - isoVal0) / (isoVal1 - isoVal0);

    outVertex.Position = vec4(corners[v0] + interpolated * (corners[v1] - corners[v0]), 1.0f);
    outVertex.Normal = vec4(normalize(vec3(hermiteData[v0] + (hermiteData[v1] - hermiteData[v0]) * interpolated)), 0.0f);
    //if (outVertex.Normal.IsZero() == false)
    //    outVertex.Normal.Normalize();
}

uint GenerateLines(uint faceIndex, vec3 corners[4], vec4 hermiteData[4], inout uint nextSegmentIndex, inout LineSegment outLineSegments[24])
{
    uint xIndex = FACE_COORDS[faceIndex].x;
    uint yIndex = FACE_COORDS[faceIndex].y;

    vec2 corner0_2D = vec2(corners[0][xIndex], corners[0][yIndex]);
    vec2 corner1_2D = vec2(corners[1][xIndex], corners[1][yIndex]);
    vec2 corner2_2D = vec2(corners[2][xIndex], corners[2][yIndex]);
    vec2 corner3_2D = vec2(corners[3][xIndex], corners[3][yIndex]);
    
    vec2 min = min(corner0_2D, min(corner1_2D , min(corner2_2D, corner3_2D)));
    vec2 max = max(corner0_2D, max(corner1_2D , max(corner2_2D, corner3_2D)));

    uint marchingSquaresCase = ((hermiteData[0].w <= ISO_LEVEL) ? 1 : 0) |
                               ((hermiteData[1].w <= ISO_LEVEL) ? 2 : 0) |
                               ((hermiteData[2].w <= ISO_LEVEL) ? 4 : 0) |
                               ((hermiteData[3].w <= ISO_LEVEL) ? 8 : 0);

    if ((marchingSquaresCase == 0) || (marchingSquaresCase == 15))
    {
        return marchingSquaresCase;
    }

    // handle ambigous
    if (MS_TABLE[marchingSquaresCase].z >= 0)
    {
        // ambigous case
    }
    else
    {
        uint edge0_From = MS_TABLE[marchingSquaresCase].x;
        uint edge0_To = MS_TABLE[marchingSquaresCase].y;
        
        uint cubeEdge0_From = FACE_EDGE_TO_CUBE_EDGE[faceIndex][edge0_From];
        uint cubeEdge0_To = FACE_EDGE_TO_CUBE_EDGE[faceIndex][edge0_To];
        
        Vertex edge0FromVertex;
        CalculateZeroCrossing(edge0_From, corners, hermiteData, edge0FromVertex);

        Vertex edge0ToVertex;
        CalculateZeroCrossing(edge0_To, corners, hermiteData, edge0ToVertex);

        vec3 intesectionPosition;
        vec3 intersectionNormal;
//        if (ResolveFaceSharpFeature(faceIndex, edge0_FromPosition, edge0_FromNormal, edge0_ToPosition, edge0_ToNormal))
 //       {
          // handle
//        }
//        else
        {
            LineSegment segment;
            segment.LinePoints[0] = edge0FromVertex;
            segment.LinePoints[1] = edge0ToVertex;
            segment.FromEdgeIndex = cubeEdge0_From;
            segment.ToEdgeIndex = cubeEdge0_To;

            segment.LinePointsCount = 2;
            outLineSegments[nextSegmentIndex++] = segment;
        }
    }

    return marchingSquaresCase;
}

void GenerateMesh(uvec3 id, vec3 corners[8], vec4 hermiteData[8], out Component outComponents[12], out uint outComponentCount)
{
    vec3 faceCorners[4];
    vec4 faceHermiteData[4];

    uint nextLineSegmentIndex = 0;
    LineSegment outLineSegments[24];

    for (uint faceIndex = 0; faceIndex < 6; ++faceIndex)
    {
        faceCorners[0] = corners[FACE_VERTICES[faceIndex].x];
        faceCorners[1] = corners[FACE_VERTICES[faceIndex].y];
        faceCorners[2] = corners[FACE_VERTICES[faceIndex].z];
        faceCorners[3] = corners[FACE_VERTICES[faceIndex].w];

        faceHermiteData[0] = hermiteData[FACE_VERTICES[faceIndex].x];
        faceHermiteData[1] = hermiteData[FACE_VERTICES[faceIndex].y];
        faceHermiteData[2] = hermiteData[FACE_VERTICES[faceIndex].z];
        faceHermiteData[3] = hermiteData[FACE_VERTICES[faceIndex].w];

        GenerateLines(faceIndex, faceCorners, faceHermiteData, nextLineSegmentIndex, outLineSegments);
    }

    // trace segments
    bool tracedSegment[24] = {
    false, false, false, false, false, false, false,
    false, false, false, false, false, false, false,
    false, false, false, false, false, false, false,
    false, false, false };

    uint vertexIndex = 0;
    uint componentIndex = 0;
    uint edgeIndex = 0;
    for (uint segmentIndex = 0; segmentIndex < nextLineSegmentIndex; ++segmentIndex)
    {
        if (tracedSegment[segmentIndex])
            continue;
        
        outComponents[componentIndex].VertexCount = 0;
        
        edgeIndex = outLineSegments[segmentIndex].ToEdgeIndex;

        for (uint linePointIndex = 0; linePointIndex < outLineSegments[segmentIndex].LinePointsCount; ++linePointIndex)
        {
            outComponents[componentIndex].Vertices[vertexIndex++] = outLineSegments[segmentIndex].LinePoints[linePointIndex];
        }
        
        tracedSegment[segmentIndex] = true;

        bool hasAdded = true;
        while (hasAdded)
        {
            hasAdded = false;
            for (uint traceSegmentIndex = 0; traceSegmentIndex < nextLineSegmentIndex; ++traceSegmentIndex)
            {
                if (tracedSegment[traceSegmentIndex])
                    continue;

                if (outLineSegments[traceSegmentIndex].FromEdgeIndex == edgeIndex)
                {
                    for (uint linePointIndex = 0; linePointIndex < outLineSegments[traceSegmentIndex].LinePointsCount; ++linePointIndex)
                    {
                        outComponents[componentIndex].Vertices[vertexIndex++] = outLineSegments[traceSegmentIndex].LinePoints[linePointIndex];
                    }
                    hasAdded = true;
                    edgeIndex = outLineSegments[traceSegmentIndex].ToEdgeIndex;
                    tracedSegment[traceSegmentIndex] = true;
                }
                else if (outLineSegments[traceSegmentIndex].ToEdgeIndex == edgeIndex)
                {
                    for (int linePointIndex = int(outLineSegments[traceSegmentIndex].LinePointsCount) - 1; linePointIndex >= 0 ; --linePointIndex)
                    {
                        outComponents[componentIndex].Vertices[vertexIndex++] = outLineSegments[traceSegmentIndex].LinePoints[linePointIndex];
                    }

                    hasAdded = true;
                    edgeIndex = outLineSegments[traceSegmentIndex].FromEdgeIndex;
                    tracedSegment[traceSegmentIndex] = true;
                }
            }
        }

        // remove duplicate
        if (outComponents[componentIndex].Vertices[0].Position == outComponents[componentIndex].Vertices[vertexIndex - 1].Position)
        {
            --vertexIndex;
        }

        outComponents[componentIndex].VertexCount = vertexIndex;
        ++componentIndex;
    }
    // trace segement end

    outComponentCount = componentIndex;
} 