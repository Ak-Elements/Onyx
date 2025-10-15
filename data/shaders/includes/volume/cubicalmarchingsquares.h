#include "includes/volume/marchingsquares.h"
#include "includes/math/sat.h"
#include "includes/debug/print.h"

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
    uint VertexIndices[32];
    uint VertexCount;
};

uint AddVertex(Vertex vertex, inout Vertex[32] vertices, inout uint vertexCount)
{
    for (uint i = 0; i < vertexCount; ++i)
    {
        if ( (vertices[i].Position == vertex.Position) && (vertices[i].Normal == vertex.Normal) )
        {
            return i;
        }
    }

    uint index = vertexCount++;
    vertices[index] = vertex;
    return index;
}

vec2 ComputeIntersection(vec2 edgeCrossingPoint0, vec2 edgeCrossingNormal0, vec2 edgeCrossingPoint1, vec2 edgeCrossingNormal1)
{
    const vec2 direction = edgeCrossingPoint1 - edgeCrossingPoint0;

    const vec2 d1 = vec2(edgeCrossingNormal0.y, -edgeCrossingNormal0.x); // perpendicular to edgeCrossingNormal1
    float projection = (dot(edgeCrossingNormal1, direction)) / (dot(edgeCrossingNormal1, d1));
    return edgeCrossingPoint0 + d1 * projection;
}

bool HasSharpFeature(vec2 edgeCrossingFromNormal, vec2 edgeCrossingToNormal)
{
    float dot = dot(edgeCrossingFromNormal, -edgeCrossingToNormal);
    float cross = edgeCrossingFromNormal.x * edgeCrossingToNormal.y - edgeCrossingFromNormal.y * edgeCrossingToNormal.x;

    const float SHARP_ANGLE_FEATURE = cos((135 * (3.1415926538 / 180.0))); /*Degrees*/
    return (abs(cross) > 0.0001f) && dot >= SHARP_ANGLE_FEATURE;
}

bool ResolveFaceSharpFeature(vec2 edgeCrossingFromPosition,
                             vec2 edgeCrossingFromNormal,
                             vec2 edgeCrossingToPosition,
                             vec2 edgeCrossingToNormal,
                             vec2 minBounds, vec2 maxBounds,
                             out vec2 outIntersectionPosition)
{
    const bool hasSharpFeature = HasSharpFeature(edgeCrossingFromNormal, edgeCrossingToNormal);
    if (hasSharpFeature)
    {
        outIntersectionPosition = ComputeIntersection(edgeCrossingFromPosition, edgeCrossingFromNormal, edgeCrossingToPosition, edgeCrossingToNormal);

        // clamp to min and max of cell boundaries TODO readd
        if (outIntersectionPosition.x < minBounds.x || outIntersectionPosition.x > maxBounds.x)
        {
            outIntersectionPosition.x = max(minBounds.x, min(outIntersectionPosition.x, maxBounds.x));
        }

        if (outIntersectionPosition.y < minBounds.y || outIntersectionPosition.y > maxBounds.y)
        {
            outIntersectionPosition.y = max(minBounds.y, min(outIntersectionPosition[1], maxBounds.y));
        }
    }

    return hasSharpFeature;
}

bool ResolveFaceSharpFeature(uint faceIndex, Vertex edgeCrossing_From, Vertex edgeCrossing_To, vec2 minBounds, vec2 maxBounds, out Vertex outIntersection)
{
    uint xIndex = FACE_COORDS[faceIndex].x;
    uint yIndex = FACE_COORDS[faceIndex].y;

    vec2 edgeCrossingFromPosition = vec2(edgeCrossing_From.Position[xIndex], edgeCrossing_From.Position[yIndex]);
    vec2 edgeCrossingFromNormal = vec2(edgeCrossing_From.Normal[xIndex], edgeCrossing_From.Normal[yIndex]);
    edgeCrossingFromNormal = normalize(edgeCrossingFromNormal);

    vec2 edgeCrossingToPosition = vec2(edgeCrossing_To.Position[xIndex], edgeCrossing_To.Position[yIndex]);
    vec2 edgeCrossingToNormal = vec2(edgeCrossing_To.Normal[xIndex], edgeCrossing_To.Normal[yIndex]);
    edgeCrossingToNormal = normalize(edgeCrossingToNormal);

    vec2 intersectionPoint;
    const bool hasSharpFeature = ResolveFaceSharpFeature(edgeCrossingFromPosition, edgeCrossingFromNormal, edgeCrossingToPosition, edgeCrossingToNormal, minBounds, maxBounds, intersectionPoint);
    if (hasSharpFeature)
    {
        outIntersection.Position = edgeCrossing_From.Position;
        outIntersection.Position[xIndex] = intersectionPoint.x;
        outIntersection.Position[yIndex] = intersectionPoint.y;
        outIntersection.Normal = vec4(normalize(edgeCrossing_From.Normal.xyz + edgeCrossing_To.Normal.xyz), 1.0f); // TODO: This normal is wrong
    }

    return hasSharpFeature;
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
    outVertex.Normal = vec4(normalize(vec3(hermiteData[v0] + (hermiteData[v1] - hermiteData[v0]) * interpolated)), 1.0f);
}

bool HasFaceAmbiguity(uint faceIndex, Vertex edgeCrossing0_From, Vertex edgeCrossing0_To, Vertex edgeCrossing1_From, Vertex edgeCrossing1_To, vec2 minBounds, vec2 maxBounds)
{
    uint xIndex = FACE_COORDS[faceIndex].x;
    uint yIndex = FACE_COORDS[faceIndex].y;

    vec3 intersection0P0 = edgeCrossing0_From.Position.xyz;
    vec3 intersection0P1 = edgeCrossing0_From.Position.xyz;
    vec3 intersection0P2 = edgeCrossing0_To.Position.xyz;

    vec3 intersection1P0 = edgeCrossing1_From.Position.xyz;
    vec3 intersection1P1 = edgeCrossing1_From.Position.xyz;
    vec3 intersection1P2 = edgeCrossing1_To.Position.xyz;

    vec2 edgeCrossing0FromPosition = vec2(edgeCrossing0_From.Position[xIndex], edgeCrossing0_From.Position[yIndex]);
    vec2 edgeCrossing0FromNormal = vec2(edgeCrossing0_From.Normal[xIndex], edgeCrossing0_From.Normal[yIndex]);
    edgeCrossing0FromNormal = normalize(edgeCrossing0FromNormal);

    vec2 edgeCrossing0ToPosition = vec2(edgeCrossing0_To.Position[xIndex], edgeCrossing0_To.Position[yIndex]);
    vec2 edgeCrossing0ToNormal = vec2(edgeCrossing0_To.Normal[xIndex], edgeCrossing0_To.Normal[yIndex]);
    edgeCrossing0ToNormal = normalize(edgeCrossing0ToNormal);

    vec2 edgeCrossing1FromPosition = vec2(edgeCrossing1_From.Position[xIndex], edgeCrossing1_From.Position[yIndex]);
    vec2 edgeCrossing1FromNormal = vec2(edgeCrossing1_From.Normal[xIndex], edgeCrossing1_From.Normal[yIndex]);
    edgeCrossing1FromNormal = normalize(edgeCrossing1FromNormal);

    vec2 edgeCrossing1ToPosition = vec2(edgeCrossing1_To.Position[xIndex], edgeCrossing1_To.Position[yIndex]);
    vec2 edgeCrossing1ToNormal = vec2(edgeCrossing1_To.Normal[xIndex], edgeCrossing1_To.Normal[yIndex]);
    edgeCrossing1ToNormal = normalize(edgeCrossing1ToNormal);

    vec2 intersectionEdge0;
    vec2 intersectionEdge1;

    const bool hasSharpFeatureEdge0 = ResolveFaceSharpFeature(edgeCrossing0FromPosition, edgeCrossing0FromNormal, edgeCrossing0ToPosition, edgeCrossing0ToNormal, minBounds, maxBounds, intersectionEdge0);
    const bool hasSharpFeatureEdge1 = ResolveFaceSharpFeature(edgeCrossing1FromPosition, edgeCrossing1FromNormal, edgeCrossing1ToPosition, edgeCrossing1ToNormal, minBounds, maxBounds, intersectionEdge1);

    if (hasSharpFeatureEdge0 && hasSharpFeatureEdge1)
    {
        if (SAT_2D_IsOverlapping(edgeCrossing0FromPosition, edgeCrossing0ToPosition, intersectionEdge0, edgeCrossing1FromPosition, edgeCrossing1ToPosition, intersectionEdge1))
        {
            // overlap choose other case
            return true;
        }
    }
    else if (hasSharpFeatureEdge0)
    {
        if (SAT_2D_IsOverlapping(edgeCrossing0FromPosition, intersectionEdge0, edgeCrossing0ToPosition, edgeCrossing1FromPosition, edgeCrossing1ToPosition))
        {
            return true;
        }
    }
    else if (hasSharpFeatureEdge1)
    {
        if (SAT_2D_IsOverlapping(edgeCrossing1FromPosition, intersectionEdge1, edgeCrossing1ToPosition, edgeCrossing0FromPosition, edgeCrossing0ToPosition))
        {
            return true;
        }
    }

    return false;
}

void ResolveAmbigousCase(uint faceIndex, uint marchingSquaresCase, vec3 corners[4], vec4 hermiteData[4], vec2 minBounds, vec2 maxBounds, inout LineSegment outLineSegments[24], inout uint nextLineSegmentIndex)
{
    uint edge0_A = MS_TABLE[marchingSquaresCase][0];
    uint edge0_B = MS_TABLE[marchingSquaresCase][1];

    Vertex Edge0_FromVertex;
    Vertex Edge0_ToVertex;

    CalculateZeroCrossing(edge0_A, corners, hermiteData, Edge0_FromVertex);
    CalculateZeroCrossing(edge0_B, corners, hermiteData, Edge0_ToVertex);

    uint edge1_A = MS_TABLE[marchingSquaresCase][2];
    uint edge1_B = MS_TABLE[marchingSquaresCase][3];

    Vertex Edge1_FromVertex;
    Vertex Edge1_ToVertex;

    CalculateZeroCrossing(edge1_A, corners, hermiteData, Edge1_FromVertex);
    CalculateZeroCrossing(edge1_B, corners, hermiteData, Edge1_ToVertex);

    // is overlapping?
    if (HasFaceAmbiguity(faceIndex, Edge0_FromVertex, Edge0_ToVertex, Edge1_FromVertex, Edge1_ToVertex, minBounds, maxBounds))
    {
        edge0_A = MS_TABLE_AMBIGUOUS[marchingSquaresCase][0];
        edge0_B = MS_TABLE_AMBIGUOUS[marchingSquaresCase][1];

        CalculateZeroCrossing(edge0_A, corners, hermiteData, Edge0_FromVertex);
        CalculateZeroCrossing(edge0_B, corners, hermiteData, Edge0_ToVertex);

        edge1_A = MS_TABLE_AMBIGUOUS[marchingSquaresCase][2];
        edge1_B = MS_TABLE_AMBIGUOUS[marchingSquaresCase][3];

        CalculateZeroCrossing(edge1_A, corners, hermiteData, Edge1_FromVertex);
        CalculateZeroCrossing(edge1_B, corners, hermiteData, Edge1_ToVertex);
    }

    uint cubeEdge0_A = FACE_EDGE_TO_CUBE_EDGE[faceIndex][edge0_A];
    uint cubeEdge0_B = FACE_EDGE_TO_CUBE_EDGE[faceIndex][edge0_B];
    uint cubeEdge1_A = FACE_EDGE_TO_CUBE_EDGE[faceIndex][edge1_A];
    uint cubeEdge1_B = FACE_EDGE_TO_CUBE_EDGE[faceIndex][edge1_B];

    // we calculate the intersection now 2 times in case of no ambiguity
    Vertex intersection;
    if (ResolveFaceSharpFeature(faceIndex, Edge0_FromVertex, Edge0_ToVertex, minBounds, maxBounds, intersection))
    {
        LineSegment segment;
        segment.LinePoints[0] = Edge0_FromVertex;
        segment.LinePoints[1] = intersection;
        segment.LinePoints[2] = Edge0_ToVertex;
        segment.FromEdgeIndex = cubeEdge0_A;
        segment.ToEdgeIndex = cubeEdge0_B;

        segment.LinePointsCount = 3;
        outLineSegments[nextLineSegmentIndex++] = segment;
    }
    else
    {
        LineSegment segment;
        segment.LinePoints[0] = Edge0_FromVertex;
        segment.LinePoints[1] = Edge0_ToVertex;
        segment.FromEdgeIndex = cubeEdge0_A;
        segment.ToEdgeIndex = cubeEdge0_B;

        segment.LinePointsCount = 2;
        outLineSegments[nextLineSegmentIndex++] = segment;
    }

    Vertex intersection2;
    if (ResolveFaceSharpFeature(faceIndex, Edge1_FromVertex, Edge1_ToVertex, minBounds, maxBounds, intersection2))
    {
        LineSegment segment;
        segment.LinePoints[0] = Edge1_FromVertex;
        segment.LinePoints[1] = intersection2;
        segment.LinePoints[2] = Edge1_ToVertex;
        segment.FromEdgeIndex = cubeEdge1_A;
        segment.ToEdgeIndex = cubeEdge1_B;

        segment.LinePointsCount = 3;
        outLineSegments[nextLineSegmentIndex++] = segment;
    }
    else
    {
        LineSegment segment;
        segment.LinePoints[0] = Edge1_FromVertex;
        segment.LinePoints[1] = Edge1_ToVertex;
        segment.FromEdgeIndex = cubeEdge1_A;
        segment.ToEdgeIndex = cubeEdge1_B;

        segment.LinePointsCount = 2;
        outLineSegments[nextLineSegmentIndex++] = segment;
    }
}

uint GenerateLines(uint faceIndex, vec3 corners[4], vec4 hermiteData[4], inout uint nextSegmentIndex, inout LineSegment outLineSegments[24])
{
    uint xIndex = FACE_COORDS[faceIndex].x;
    uint yIndex = FACE_COORDS[faceIndex].y;

    vec2 corner0_2D = vec2(corners[0][xIndex], corners[0][yIndex]);
    vec2 corner1_2D = vec2(corners[1][xIndex], corners[1][yIndex]);
    vec2 corner2_2D = vec2(corners[2][xIndex], corners[2][yIndex]);
    vec2 corner3_2D = vec2(corners[3][xIndex], corners[3][yIndex]);

    vec2 minBounds = min(corner0_2D, min(corner1_2D , min(corner2_2D, corner3_2D)));
    vec2 maxBounds = max(corner0_2D, max(corner1_2D , max(corner2_2D, corner3_2D)));

    uint marchingSquaresCase = ((hermiteData[0].w > ISO_LEVEL) ? 1 : 0) |
                               ((hermiteData[1].w > ISO_LEVEL) ? 2 : 0) |
                               ((hermiteData[2].w > ISO_LEVEL) ? 4 : 0) |
                               ((hermiteData[3].w > ISO_LEVEL) ? 8 : 0);

    if ((marchingSquaresCase == 0) || (marchingSquaresCase == 15))
    {
        return 0;
    }

    // handle ambigous
    if (MS_TABLE[marchingSquaresCase].z >= 0)
    {
        ResolveAmbigousCase(faceIndex, marchingSquaresCase, corners, hermiteData, minBounds, maxBounds, outLineSegments, nextSegmentIndex);
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

        Vertex intersectionVertex;
        if (ResolveFaceSharpFeature(faceIndex, edge0FromVertex, edge0ToVertex, minBounds, maxBounds, intersectionVertex))
        {
            LineSegment segment;
            segment.LinePoints[0] = edge0FromVertex;
            segment.LinePoints[1] = intersectionVertex;
            segment.LinePoints[2] = edge0ToVertex;
            segment.FromEdgeIndex = cubeEdge0_From;
            segment.ToEdgeIndex = cubeEdge0_To;

            segment.LinePointsCount = 3;
            outLineSegments[nextSegmentIndex++] = segment;
        }
       else
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

uint GenerateMesh(uvec3 id, vec3 corners[8], vec4 hermiteData[8], out Component outComponents[12], out uint outComponentCount, out Vertex outVertices[32], out uint outVertexCount)
{
    vec3 faceCorners[4];
    vec4 faceHermiteData[4];

    outVertexCount = 0;

    uint nextLineSegmentIndex = 0;
    LineSegment outLineSegments[24];

    uint marchingSquaresCase = 0;
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

        uint msCase = GenerateLines(faceIndex, faceCorners, faceHermiteData, nextLineSegmentIndex, outLineSegments);
        if (msCase != 0)
        {
            marchingSquaresCase = msCase;
        }
    }

    // trace segments
    bool tracedSegment[24] = {
    false, false, false, false, false, false, false,
    false, false, false, false, false, false, false,
    false, false, false, false, false, false, false,
    false, false, false };

    uint vertexCount = 0;
    uint componentIndex = 0;
    uint edgeIndex = 0;
            
    for (uint segmentIndex = 0; segmentIndex < nextLineSegmentIndex; ++segmentIndex)
    {
        if (tracedSegment[segmentIndex])
            continue;
        
        vertexCount = 0;
        outComponents[componentIndex].VertexCount = 0;
        
        edgeIndex = outLineSegments[segmentIndex].ToEdgeIndex;

        for (uint linePointIndex = 0; linePointIndex < outLineSegments[segmentIndex].LinePointsCount; ++linePointIndex)
        {
            Vertex linePointVertex = outLineSegments[segmentIndex].LinePoints[linePointIndex];
            uint vertexIndex = AddVertex(linePointVertex, outVertices, outVertexCount);
            
            outComponents[componentIndex].VertexIndices[vertexCount++] = vertexIndex;
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
                        Vertex linePointVertex = outLineSegments[traceSegmentIndex].LinePoints[linePointIndex];
                        uint vertexIndex = AddVertex(linePointVertex, outVertices, outVertexCount);

                        outComponents[componentIndex].VertexIndices[vertexCount++] = vertexIndex;
                    }
                    hasAdded = true;
                    edgeIndex = outLineSegments[traceSegmentIndex].ToEdgeIndex;
                    tracedSegment[traceSegmentIndex] = true;
                }
                else if (outLineSegments[traceSegmentIndex].ToEdgeIndex == edgeIndex)
                {
                    for (int linePointIndex = int(outLineSegments[traceSegmentIndex].LinePointsCount) - 1; linePointIndex >= 0 ; --linePointIndex)
                    {
                        Vertex linePointVertex = outLineSegments[traceSegmentIndex].LinePoints[linePointIndex];
                        uint vertexIndex = AddVertex(linePointVertex, outVertices, outVertexCount);

                        outComponents[componentIndex].VertexIndices[vertexCount++] = vertexIndex;
                    }

                    hasAdded = true;
                    edgeIndex = outLineSegments[traceSegmentIndex].FromEdgeIndex;
                    tracedSegment[traceSegmentIndex] = true;
                }
            }
        }

        // remove duplicate
        while (vertexCount > 0 && outComponents[componentIndex].VertexIndices[0] == outComponents[componentIndex].VertexIndices[vertexCount - 1])
        {
            --vertexCount;
        }

        if (vertexCount != 0)
        {
            outComponents[componentIndex].VertexCount = vertexCount;
            ++componentIndex;
        }
    }
    // trace segement end

    outComponentCount = componentIndex;
    
    return marchingSquaresCase;
} 