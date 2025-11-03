#pragma once

namespace Onyx::Volume
{

inline constexpr StringView BUILD_OCTREE_SHADER = R"(

#version 460 core 

// VolumeShader version @VERSION@

#include "includes/common.h"
#include "includes/volume/world_octree.h"
#include "includes/volume/world_chunk.h"
#include "includes/volume/volumesources.h"
#include "includes/volume/isosurface_extraction.h"
#include "includes/volume/octree_split_policy.h"
#include "includes/indirect_dispatch.h"
#include "includes/morton.h"

#include "@BASE_TERRAIN_SDF_SHADER@"

compute
{
    #define LOCAL_SIZE 64
    layout(local_size_x = LOCAL_SIZE, local_size_y = 1, local_size_z = 1) in;
    
    struct SplitNodeRequest
    {
        uint64_t Morton;
        uint Offset;
        uint ChunkIndex;
    }; 

    layout(std430, buffer_reference, buffer_reference_align = 8) buffer SplitOctreeRequests
    {
        uint Count;
        SplitNodeRequest Nodes[];
    };
    
    layout(push_constant) uniform Constants
    {
        WorldOctree Octree;
        uint64_t OctreeAllocator;

        WorldOctreeChunks Chunks;
        uint64_t ChunkAllocator;
        
        WorldVolumeSourcesList VolumeSourcesList;
        WorldVolumeSources VolumeSourcesData;

        IsoSurfaceRequests SurfaceRequests;
        IndirectDispatch IndirectDispatch;

        SplitOctreeRequests SplitRequestsReadBuffer;
        SplitOctreeRequests SplitRequestsWriteBuffer;

        vec3 CameraPosition;
        float RootHalfExtents;
        
        float NodeExtents;
        uint Offset;
        float MaxGeometricError;
        float ComplexSurfaceThreshold;

        uint Depth;
        uint MaxDepth;
        float Padding0;
        float Padding1;
    } u_Constants;

    void main()
    {
        if (gl_GlobalInvocationID == uvec3(0,0,0))
        {
            u_Constants.IndirectDispatch.X = 0;
            u_Constants.IndirectDispatch.Y = 1;
            u_Constants.IndirectDispatch.Z = 1;
            
            u_Constants.SplitRequestsWriteBuffer.Count = 0;
        }

        uint currentLevelRequestedNodeCount = u_Constants.SplitRequestsReadBuffer.Count;

        uint globalID = gl_GlobalInvocationID.x;
        bool isRoot = u_Constants.Depth == 0;

        if (isRoot && (globalID != 0))
            return;

        if ((isRoot == false) && (globalID >= currentLevelRequestedNodeCount))
            return;

        if (isRoot)
        {
            u_Constants.Octree.Count = 1;
            u_Constants.Chunks.Count = 0;
        }

        vec3 centerPosition = vec3(0);
        uint64_t mortonIndex = uint64_t(0);
        OctreeNode octreeNode;
        uint octreeFlatIndex = 0;
        uint childCount = 0;
    
        bool isMaxDepth = u_Constants.Depth == u_Constants.MaxDepth;
        if (isRoot == false)
        {
            uint flatIndex = globalID;
            SplitNodeRequest splitRequest = u_Constants.SplitRequestsReadBuffer.Nodes[flatIndex];
            mortonIndex = splitRequest.Morton;

            uint64_t depthMask = ((1ul << (3ul * uint64_t(u_Constants.Depth))) - 1ul);
            uint64_t mortonIndexAtDepth = mortonIndex & depthMask;
            octreeFlatIndex = uint(splitRequest.Offset);
            
            vec3 coordinate = vec3(DecodeMorton(mortonIndexAtDepth)) + vec3(0.5f);
            centerPosition = coordinate * u_Constants.NodeExtents - vec3(u_Constants.RootHalfExtents);
        }

        octreeNode = EvaluateOctreeNode(u_Constants.CameraPosition,
            centerPosition,
            u_Constants.NodeExtents,
            u_Constants.MaxGeometricError,
            u_Constants.ComplexSurfaceThreshold,
            u_Constants.VolumeSourcesList,
            u_Constants.VolumeSourcesData,
            childCount);

        if (octreeNode.ValidMask != 0)
        {
            octreeNode.ChildrenOffset = atomicAdd(u_Constants.Octree.Count, 8);
            uint createStartIndex = atomicAdd(u_Constants.SplitRequestsWriteBuffer.Count, childCount);

            uint chunkStartIndex = 0; 
            if (isMaxDepth)
            {
                chunkStartIndex = atomicAdd(u_Constants.Chunks.Count, 8);
            }

            uint validChildIndex = 0;
            uint64_t shiftedMorton = mortonIndex << 3ul;
            for (uint i = 0; i < 8; ++i)
            {
                uint childMask = (1 << i);
                if ((octreeNode.ValidMask & childMask) != 0)
                {
                    if (isMaxDepth)
                    {
                        u_Constants.Chunks.Chunks[chunkStartIndex] = WorldOctreeChunk(mortonIndex, uint(u_Constants.Depth), 0);
                    }

                    u_Constants.SplitRequestsWriteBuffer.Nodes[createStartIndex++] = SplitNodeRequest(shiftedMorton + i, octreeNode.ChildrenOffset + validChildIndex, chunkStartIndex);
                    ++validChildIndex;
                }

                ++chunkStartIndex;
            }
        }
        else if (octreeNode.LeafMask == 255)
        {
            uint chunkIndex = atomicAdd(u_Constants.Chunks.Count, 1);
            u_Constants.Chunks.Chunks[chunkIndex] = WorldOctreeChunk(mortonIndex, uint(u_Constants.Depth), 0);

            uint surfaceRequestIndex = atomicAdd(u_Constants.SurfaceRequests.Count, 1);
            u_Constants.SurfaceRequests.Requests[surfaceRequestIndex] = IsoSurfaceRequest(mortonIndex, uint(u_Constants.Depth), chunkIndex);
        }

        u_Constants.Octree.Nodes[octreeFlatIndex] = PackOctreeNode(octreeNode);
        
        uint dispatchX = u_Constants.SplitRequestsWriteBuffer.Count == 0 ? 0 : uint(ceil(u_Constants.SplitRequestsWriteBuffer.Count / float(LOCAL_SIZE)));
        atomicMax(u_Constants.IndirectDispatch.X, max(1, dispatchX));
    }

}

)";

inline constexpr const char* BUILD_CHUNK_OCTREE_SHADER = R"(

#version 460 core 

// VolumeShader version @VERSION@

#include "includes/common.h"
#include "includes/volume/world_octree.h"
#include "includes/volume/world_chunk.h"
#include "includes/volume/volumesources.h"
#include "includes/volume/isosurface_extraction.h"
#include "includes/volume/octree_split_policy.h"
#include "includes/indirect_dispatch.h"
#include "includes/morton.h"

#include "@BASE_TERRAIN_SDF_SHADER@"

compute
{
    #define LOCAL_SIZE 64
    layout(local_size_x = LOCAL_SIZE, local_size_y = 1, local_size_z = 1) in;
    
    struct SplitNodeRequest
    {
        uint64_t Morton;
        uint Offset;
        uint ChunkIndex;
    };

    layout(std430, buffer_reference, buffer_reference_align = 8) buffer SplitOctreeRequests
    {
        uint Count;
        SplitNodeRequest Nodes[];
    };
    
    layout(push_constant) uniform Constants
    {
        WorldOctree Octree;
        uint64_t OctreeAllocator;

        WorldOctreeChunks Chunks;
        uint64_t ChunkAllocator;

        WorldVolumeSourcesList VolumeSourcesList;
        WorldVolumeSources VolumeSourcesData;

        IsoSurfaceRequests SurfaceRequests;
        IndirectDispatch IndirectDispatch;

        SplitOctreeRequests SplitRequestsReadBuffer;
        SplitOctreeRequests SplitRequestsWriteBuffer;

        vec3 CameraPosition;
        float RootHalfExtents;
        
        float NodeExtents;
        uint Offset;
        float MaxGeometricError;
        float ComplexSurfaceThreshold;

        uint Depth;
        uint MaxDepth;
        float Padding0;
        float Padding1;

    } u_Constants;

    void main()
    {
        if (gl_GlobalInvocationID == uvec3(0,0,0))
        {
            u_Constants.IndirectDispatch.X = 0;
            u_Constants.IndirectDispatch.Y = 1;
            u_Constants.IndirectDispatch.Z = 1;
            
            u_Constants.SplitRequestsWriteBuffer.Count = 0;
        }

        uint currentLevelRequestedNodeCount = u_Constants.SplitRequestsReadBuffer.Count;

        if (currentLevelRequestedNodeCount == 0)
            return;

        uint globalID = gl_GlobalInvocationID.x;
        if (globalID >= currentLevelRequestedNodeCount)
        {
            return;
        }

        bool isMaxDepth = u_Constants.Depth == u_Constants.MaxDepth;
  
        uint flatIndex = globalID;
        SplitNodeRequest splitRequest = u_Constants.SplitRequestsReadBuffer.Nodes[flatIndex];
        uint64_t mortonIndex = splitRequest.Morton;

        uint chunkIndex = splitRequest.ChunkIndex;
        uint64_t depthMask = ((1ul << (3ul * uint64_t(u_Constants.Depth))) - 1ul);
        uint64_t mortonIndexAtDepth = mortonIndex & depthMask;
        uint octreeFlatIndex = uint(splitRequest.Offset);
        
        vec3 coordinate = vec3(DecodeMorton(mortonIndexAtDepth)) + vec3(0.5f);
        vec3 centerPosition = coordinate * u_Constants.NodeExtents - vec3(u_Constants.RootHalfExtents);

        uint childCount = 0;

        OctreeNode octreeNode;
        if (isMaxDepth)
        {
            octreeNode.LeafMask = 255;
            octreeNode.ChildrenOffset = 0;
            octreeNode.ValidMask = 0;
            octreeNode.IsFar = false;
        }
        else
        {
            octreeNode = EvaluateOctreeNode(u_Constants.CameraPosition,
                centerPosition,
                u_Constants.NodeExtents,
                u_Constants.MaxGeometricError,
                u_Constants.ComplexSurfaceThreshold,
                u_Constants.VolumeSourcesList,
                u_Constants.VolumeSourcesData,
                childCount);
        }

        if (octreeNode.ValidMask != 0)
        {
#if ONYX_IS_DEBUG
            if (u_Constants.SplitRequestsWriteBuffer.Count + childCount >= (1 << 20) )
            {
                debugPrintfEXT("Exceeding split requests");
            }
#endif
            octreeNode.ChildrenOffset = atomicAdd(u_Constants.Octree.Count, 8);
            uint createStartIndex = atomicAdd(u_Constants.SplitRequestsWriteBuffer.Count, childCount);

            uint validChildIndex = 0;
            uint64_t shiftedMorton = mortonIndex << 3ul;
            for (uint i = 0; i < 8; ++i)
            {
                uint childMask = (1 << i);
                if ((octreeNode.ValidMask & childMask) != 0)
                {
                    u_Constants.SplitRequestsWriteBuffer.Nodes[createStartIndex++] = SplitNodeRequest(shiftedMorton + i, octreeNode.ChildrenOffset + validChildIndex, chunkIndex);
                    ++validChildIndex;
                }
            }
        }
        else if (octreeNode.LeafMask == 255)
        {
            uint surfaceRequestIndex = atomicAdd(u_Constants.SurfaceRequests.Count, 1);
            u_Constants.SurfaceRequests.Requests[surfaceRequestIndex] = IsoSurfaceRequest(mortonIndex, uint(u_Constants.Depth), chunkIndex);
        }

        u_Constants.Octree.Nodes[octreeFlatIndex] = PackOctreeNode(octreeNode);

        if (octreeFlatIndex == 0)
        {
            debugPrintfEXT("ERROR, wrong flat index %u", u_Constants.Depth);
        }

        uint dispatchX = u_Constants.SplitRequestsWriteBuffer.Count == 0 ? 0 : uint(ceil(u_Constants.SplitRequestsWriteBuffer.Count / float(LOCAL_SIZE)));
        atomicMax(u_Constants.IndirectDispatch.X, max(1, dispatchX));
    }
}

)";

inline constexpr const char* FIND_OCTREE_NODE_SHADER = R"(

#version 460 core 

// VolumeShader version @VERSION@

#include "includes/common.h"
#include "includes/volume/world_octree.h"
#include "includes/volume/world_chunk.h"
#include "includes/volume/volumesources.h"
#include "includes/volume/isosurface_extraction.h"
#include "includes/volume/octree_split_policy.h"
#include "includes/indirect_dispatch.h"
#include "includes/morton.h"

#include "@BASE_TERRAIN_SDF_SHADER@"

compute
{
    #define LOCAL_SIZE 1
    layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
    
    struct UpdateOctreeNodeRequest
    {
        uint64_t Morton;
        uint Depth;
        uint Offset;
    };

    layout(std430, buffer_reference, buffer_reference_align = 8) buffer UpdateOctreeNodesBuffer
    {
        uint Count;
        UpdateOctreeNodeRequest Requests[];
    };

    layout(std430, buffer_reference, buffer_reference_align = 8) readonly buffer HitPositionBuffer
    {
        vec3 HitPosition;
        uint HasHit;
    };

    layout(push_constant) uniform Constants
    {
        WorldOctree Octree;
        float RootHalfExtents;
        uint MaxDepth;

        WorldVolumeSourcesList VolumeSourcesList;
        WorldVolumeSources VolumeSourcesData;

        UpdateOctreeNodesBuffer QueuedCollapseBuffer;
        UpdateOctreeNodesBuffer QueuedSplitBuffer;

        HitPositionBuffer HitPositionBuffer;
        float BrushSizeSquared;
        float MaxGeometricError;
        
        float ComplexSurfaceThreshold;
        float Padding[3];

    } u_Constants;


    void main()
    {
        vec3 hitPosition = u_Constants.HitPositionBuffer.HitPosition;

        debugPrintfEXT("%u volume sources count", u_Constants.VolumeSourcesList.Count);

        uint depth = 0;
        float nodeExtents = u_Constants.RootHalfExtents * 2.0f;
        uint64_t currentMortonIndex = 0;
        uint currentFlatNodeIndex = 0;
        OctreeNode currentNode = UnpackOctreeNode(u_Constants.Octree.Nodes[currentFlatNodeIndex]);

        vec3 centerPosition;
        bool hasFoundNode = false;
        while (depth < u_Constants.MaxDepth)
        {
            uint64_t depthMask = ((1ul << (3ul * uint64_t(depth))) - 1ul);
            uint64_t mortonIndexAtDepth = currentMortonIndex & depthMask;

            vec3 coordinate = vec3(DecodeMorton(mortonIndexAtDepth)) + vec3(0.5f);
            centerPosition = coordinate * nodeExtents - vec3(u_Constants.RootHalfExtents);

            float childNodeExtents = nodeExtents * 0.5f;
            float childNodeHalfExtents = childNodeExtents * 0.5f;

            uint childX = hitPosition.x < centerPosition.x ? 0 : 1;
            uint childY = hitPosition.y < centerPosition.y ? 0 : 1;
            uint childZ = hitPosition.z < centerPosition.z ? 0 : 1;

            uint childIndex = childX | (childY << 1) | (childZ << 2);
            if ((currentNode.ValidMask & childIndex) == 0)
            {
                // fill node info
                hasFoundNode = true;
                break;
            }

            uint64_t childMorton = (currentMortonIndex << 3ul) + childIndex;
            coordinate = vec3(DecodeMorton(childMorton)) + vec3(0.5f);
            vec3 childCenter = coordinate * childNodeExtents - vec3(u_Constants.RootHalfExtents);
            
            vec3 deltaToChild = childCenter - hitPosition;
            float lengthSquared = dot(deltaToChild, deltaToChild);

            if (u_Constants.BrushSizeSquared >= lengthSquared)
            {
                // fill node info
                hasFoundNode = true;
                break;
            }

            currentMortonIndex = childMorton;
            currentFlatNodeIndex = currentNode.ChildrenOffset + childIndex;
            currentNode = UnpackOctreeNode(u_Constants.Octree.Nodes[currentFlatNodeIndex]);
            nodeExtents = childNodeExtents;
            ++depth;
        }

        if (hasFoundNode == false)
        {
            return;
        }

        // put nodes in corresponding update requests
        bool isMaxDepth = depth == u_Constants.MaxDepth;
        if (isMaxDepth)
        {
            // queue that node for remesh
            if (isMaxDepth)
                debugPrintfEXT("Is at max depth");
            else
                debugPrintfEXT("Fully subdivided");
        }
        else
        {
            vec3 cameraPos;
            uint childCount = 0;
            uint64_t childBaseMorton = (currentMortonIndex << 3ul);
            float childNodeExtents = nodeExtents * 0.5f;
            for (uint childIndex = 0; childIndex < 8; ++childIndex)
            {
                uint childMask = (1 << childIndex);

                uint64_t childMorton = childBaseMorton + childIndex;
                vec3 coordinate = vec3(DecodeMorton(childMorton)) + vec3(0.5f);
                vec3 childCenter = coordinate * childNodeExtents - vec3(u_Constants.RootHalfExtents);

                OctreeNode evaluatedChild = EvaluateOctreeNode(cameraPos,
                    childCenter,
                    childNodeExtents,
                    u_Constants.MaxGeometricError,
                    u_Constants.ComplexSurfaceThreshold,
                    u_Constants.VolumeSourcesList, 
                    u_Constants.VolumeSourcesData,
                    childCount);

                if ((currentNode.ValidMask & childMask) != 0)
                {
                    uint childFlatIndex = currentNode.ChildrenOffset + childIndex;
                    OctreeNode currentChildNode = UnpackOctreeNode(u_Constants.Octree.Nodes[childFlatIndex]);
                    
                    bool hasSplitChanged = currentChildNode.ValidMask != evaluatedChild.ValidMask;
                    if ((hasSplitChanged) && (evaluatedChild.ValidMask == 0))
                    {
                        // collapse
                        debugPrintfEXT("Found node to collapse");
                    }
                    else
                    {
                        // split & traverse down
                        debugPrintfEXT("Found node to split / update");
                    }
                }
                else if ((currentNode.LeafMask & childMask) != 0)
                {
                    if (evaluatedChild.ValidMask == 0)
                        debugPrintfEXT("Nothing changed");
                    else
                        debugPrintfEXT("Found node to split / update");
                }
            }
        }
    }
}

)";

inline constexpr const char* GENERATE_VOLUME_MESH_SHADER = R"(

#version 460 core 

// VolumeShader version @VERSION@

#include "includes/common.h"
#include "includes/indirect_draw.h"
#include "includes/morton.h"
#include "includes/volume/isosurface_extraction.h"
#include "includes/volume/cubicalmarchingsquares.h"
#include "includes/volume/volumesources.h"
#include "includes/volume/sample_terrain.h"

#include "@BASE_TERRAIN_SDF_SHADER@"

compute
{
    #define TERRAIN_SHADER_LOCAL_SIZE 64
    layout(local_size_x = TERRAIN_SHADER_LOCAL_SIZE, local_size_y = 1, local_size_z = 1) in;

    layout(std430, buffer_reference, buffer_reference_align = 8) writeonly buffer VertexBuffer
    {
        Vertex Vertices[];
    };
  
    layout(push_constant) uniform Constants
    {
        IsoSurfaceRequests Requests;
        VertexBuffer VertexBufferPointer;

        WorldVolumeSourcesList VolumeSourcesList;
        WorldVolumeSources VolumeSourcesData;

        IndirectDrawBuffer IndirectDraw;
        float RootSize;
        float RootHalfExtents;
    } u_Constants;

    void main()
    {
        uint flatIndex = gl_GlobalInvocationID.x;
        
        if (flatIndex >= u_Constants.Requests.Count)
        {
            return;
        }
        
        IsoSurfaceRequest surfaceRequest = u_Constants.Requests.Requests[flatIndex];
        uint64_t depthMask = ((1ul << (3ul * surfaceRequest.Depth)) - 1);
        uint64_t mortonIndexAtDepth = surfaceRequest.Morton & depthMask;
        
        float nodeExtents = u_Constants.RootSize / float(1 << surfaceRequest.Depth);
        vec3 leafCoordinate = vec3(DecodeMorton(mortonIndexAtDepth)) + vec3(0.5f);
        vec3 leafCenter = leafCoordinate * nodeExtents - vec3(u_Constants.RootHalfExtents);

        vec3 corners[8] = 
        {
            vec3(leafCenter + vec3(-0.5,-0.5,-0.5) * nodeExtents),
            vec3(leafCenter + vec3(-0.5,-0.5, 0.5) * nodeExtents),
            vec3(leafCenter + vec3(-0.5, 0.5,-0.5) * nodeExtents), 
            vec3(leafCenter + vec3(-0.5, 0.5, 0.5) * nodeExtents), 
            vec3(leafCenter + vec3( 0.5,-0.5,-0.5) * nodeExtents), 
            vec3(leafCenter + vec3( 0.5,-0.5, 0.5) * nodeExtents),
            vec3(leafCenter + vec3( 0.5, 0.5,-0.5) * nodeExtents),
            vec3(leafCenter + vec3( 0.5, 0.5, 0.5) * nodeExtents),
        };

        vec4 cornerSamples[8] =
        {
            SampleTerrain(corners[0], u_Constants.VolumeSourcesList, u_Constants.VolumeSourcesData),
            SampleTerrain(corners[1], u_Constants.VolumeSourcesList, u_Constants.VolumeSourcesData),
            SampleTerrain(corners[2], u_Constants.VolumeSourcesList, u_Constants.VolumeSourcesData),
            SampleTerrain(corners[3], u_Constants.VolumeSourcesList, u_Constants.VolumeSourcesData),
            SampleTerrain(corners[4], u_Constants.VolumeSourcesList, u_Constants.VolumeSourcesData),
            SampleTerrain(corners[5], u_Constants.VolumeSourcesList, u_Constants.VolumeSourcesData),
            SampleTerrain(corners[6], u_Constants.VolumeSourcesList, u_Constants.VolumeSourcesData),
            SampleTerrain(corners[7], u_Constants.VolumeSourcesList, u_Constants.VolumeSourcesData)
        };

        uint componentCount = 0;
        Component outComponents[12];
        Vertex outVertices[32];
        uint vertexCount = 0;
        uint marchingSquaresCase = GenerateMesh(gl_GlobalInvocationID, corners, cornerSamples, outComponents, componentCount, outVertices, vertexCount);

     
        for (uint i = 0; i < componentCount; ++i)
        {
            if (outComponents[i].VertexCount < 3)
            {
            #if ONYX_IS_DEBUG
                debugPrintfEXT("Invalid vertex count for component");
            #endif
                continue;
            }

            if (outComponents[i].VertexCount == 3)
            {
                uint vertexOffset = atomicAdd(u_Constants.IndirectDraw.Draw.VertexCount, 3);

                u_Constants.VertexBufferPointer.Vertices[vertexOffset++] = outVertices[outComponents[i].VertexIndices[0]];
                u_Constants.VertexBufferPointer.Vertices[vertexOffset++] = outVertices[outComponents[i].VertexIndices[2]];
                u_Constants.VertexBufferPointer.Vertices[vertexOffset++] = outVertices[outComponents[i].VertexIndices[1]];
            }
            else
            {  
                uint vertCount = outComponents[i].VertexCount;

                uint vertexOffset = atomicAdd(u_Constants.IndirectDraw.Draw.VertexCount, vertCount * 3);

                vec4 triangleFanCenter = vec4(0.0f);
                vec4 triangleFanCenterNormal = vec4(0.0f);

                // trace segments
                bool vistitedVertex[32] =
                {
                    false, false, false, false, false, false, false, false,
                    false, false, false, false, false, false, false, false,
                    false, false, false, false, false, false, false, false,
                    false, false, false, false, false, false, false, false
                };

                uint uniqueVerticesCount = 0;
                for (uint j = 0; j < vertCount; ++j)
                {
                    uint vertexIndex = outComponents[i].VertexIndices[j];

                    if (vistitedVertex[vertexIndex])
                        continue;

                    vistitedVertex[vertexIndex] = true;
                    Vertex vertex = outVertices[vertexIndex];

                    triangleFanCenter += vertex.Position;
                    triangleFanCenterNormal += vertex.Normal;
                    ++uniqueVerticesCount;
                }

                triangleFanCenter /= uniqueVerticesCount;
                triangleFanCenterNormal /= uniqueVerticesCount;
                Vertex triangleFanVertex;
                triangleFanVertex.Position = triangleFanCenter;
                triangleFanVertex.Normal = triangleFanCenterNormal;

                uint lastVertexIndex = (vertCount - 1);
                for (uint vertexIndex = 0; vertexIndex < vertCount; ++vertexIndex)
                {  
                    if (vertexIndex == lastVertexIndex)
                    { 
                        u_Constants.VertexBufferPointer.Vertices[vertexOffset++] = outVertices[outComponents[i].VertexIndices[0]];
                        u_Constants.VertexBufferPointer.Vertices[vertexOffset++] = outVertices[outComponents[i].VertexIndices[vertexIndex]];
                        u_Constants.VertexBufferPointer.Vertices[vertexOffset++] = triangleFanVertex;
                    }
                    else
                    {
                        u_Constants.VertexBufferPointer.Vertices[vertexOffset++] = outVertices[outComponents[i].VertexIndices[vertexIndex + 1]];
                        u_Constants.VertexBufferPointer.Vertices[vertexOffset++] = outVertices[outComponents[i].VertexIndices[vertexIndex]];
                        u_Constants.VertexBufferPointer.Vertices[vertexOffset++] = triangleFanVertex;
                    }
                }
            }
        }
    }
}

)";

inline constexpr const char* RAYTRACE_TERRAIN_SHADER = R"(

#version 460 core 

// VolumeShader version @VERSION@

#include "includes/common.h"
#include "includes/volume/world_octree.h"
#include "includes/volume/volumesources.h"
#include "includes/volume/sample_terrain.h"
#include "includes/morton.h"

#include "@BASE_TERRAIN_SDF_SHADER@"

compute
{
    const float ISO_LEVEL = 0.0f;

    #define TERRAIN_SHADER_LOCAL_SIZE 1
    layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

   layout(std430, buffer_reference, buffer_reference_align = 8) readonly buffer ViewConstants
    {
        mat4 ProjectionMatrix;
        mat4 InverseProjectionMatrix;
        mat4 ViewMatrix;
        mat4 InverseViewMatrix;
        mat4 ViewProjectionMatrix;
        mat4 InverseViewProjectionMatrix;

        vec2 Viewport;
        float Near;
        float Far;

        vec3 CameraPosition;
        float Padding;

        vec3 CameraDirection;
        float Padding2;
    };

    layout(std430, buffer_reference, buffer_reference_align = 8) writeonly buffer HitPositionBuffer
    {
        vec3 HitPosition;
        uint HasHit;
    };

    layout(push_constant) uniform Constants
    {
        vec2 MousePosition;
        ViewConstants ViewConstantsBuffer;

        WorldVolumeSourcesList VolumeSourcesList;
        WorldVolumeSources VolumeSourcesData;

        HitPositionBuffer HitPositionBuffer;
    } u_Constants;

    void main()
    { 
        vec4 mouseClipPos = vec4(u_Constants.MousePosition.xy, 0.0f, 1.0f);

        vec4 ray = u_Constants.ViewConstantsBuffer.InverseProjectionMatrix * mouseClipPos;
        ray /= ray.w;
        ray = u_Constants.ViewConstantsBuffer.InverseViewMatrix * ray;

        vec3 rayOrigin = u_Constants.ViewConstantsBuffer.CameraPosition;
        vec3 rayDirection = normalize(ray.xyz - rayOrigin);

        bool hasHit = false;
        vec4 sampleValue = SampleTerrain(rayOrigin, u_Constants.VolumeSourcesList, u_Constants.VolumeSourcesData);
        vec3 hitPosition;
        float distance = sampleValue.w;
        for(uint i = 0; i < 512; ++i)
        {
            vec3 position = rayOrigin + rayDirection * distance;
            sampleValue = SampleTerrain(position, u_Constants.VolumeSourcesList, u_Constants.VolumeSourcesData);
            if (sampleValue.w <= 0.0001f)
            {
                hasHit = true;
                hitPosition = position;
                break;
            }
        
            distance += sampleValue.w;
        }

        if (hasHit)
        {
            u_Constants.HitPositionBuffer.HitPosition = hitPosition;
            u_Constants.HitPositionBuffer.HasHit = 1;
        }
        else
        {
            u_Constants.HitPositionBuffer.HasHit = 0;
        }
    }
}

)";
}