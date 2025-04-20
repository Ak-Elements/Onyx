#pragma once

#include <onyx/volume/onyx_volume_pch.h>
#include <onyx/volume/isosurface/isosurface.h>
#include <onyx/volume/isosurface/marchingsquaressurface.h>
#include <onyx/volume/source/volumebase.h>

#ifndef USE_ANALYTICAL_NORMAL
#define USE_ANALYTICAL_NORMAL 0
#endif

namespace Onyx::Volume
{
    //TODO: change octreekey to z order again 
    //const uint8 Nodes[8] = {1, 5, 4, 0, 3, 7, 6, 2};
    const onyxU8 Nodes[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };

template <typename OctreeNodeT>
void Dualgrid<OctreeNodeT>::GenerateDualgrid(const OctreeNodeT& root, const Vector3f& rootWorldPosition, const VolumeBase& volumeSource)
{
    m_OctreeRoot = &root;
    m_OctreeRootWorldPosition = rootWorldPosition;
    m_Volume = &volumeSource;

    NodeProc(root);

    // Build up a minimal dualgrid for octrees without children.
    if (root.IsSubdivided() == false)
    {
        const Vector3f CORNER_0(-1.0f, -1.0f, -1.0f);
        const Vector3f CORNER_1(1.0f, -1.0f, -1.0f);
        const Vector3f CORNER_2(1.0f, -1.0f, 1.0f);
        const Vector3f CORNER_3(-1.0f, -1.0f, 1.0f);
        const Vector3f CORNER_4(-1.0f, 1.0f, -1.0f);
        const Vector3f CORNER_5(1.0f, 1.0f, -1.0f);
        const Vector3f CORNER_6(1.0f, 1.0f, 1.0f);
        const Vector3f CORNER_7(-1.0f, 1.0f, 1.0f);

        const Vector3f FACE_TOP(0.0f, 1.0f, 0.0f);
        const Vector3f FACE_BOTTOM(0.0f, -1.0f, 0.0f);
        const Vector3f FACE_RIGHT(1.0f, 0.0f, 0.0f);
        const Vector3f FACE_LEFT(-1.0f, 0.0f, 0.0f);
        const Vector3f FACE_FRONT(-1.0f, 0.0f, 1.0f);
        const Vector3f FACE_BACK(0.0f, 0.0f, -1.0f);

        const Vector3f CENTER_BACK_BOTTOM(0.0f, -1.0f, -1.0f);
        const Vector3f CENTER_FRONT_BOTTOM(0.0f, -1.0f, 1.0f);
        const Vector3f CENTER_LEFT_BOTTOM(-1.0f, -1.0f, 0.0f);
        const Vector3f CENTER_RIGHT_BOTTOM(1.0f, -1.0f, 0.0f);

        const Vector3f CENTER_BACK_LEFT(-1.0f, 0.0f, -1.0f);
        const Vector3f CENTER_BACK_RIGHT(1.0f, 0.0f, -1.0f);
        const Vector3f CENTER_FRONT_LEFT(-1.0f, 0.0f, 1.0f);
        const Vector3f CENTER_FRONT_RIGHT(1.0f, 0.0f, 1.0f);

        const Vector3f CENTER_BACK_TOP(0.0f, 1.0f, -1.0f);
        const Vector3f CENTER_LEFT_TOP(-1.0f, 1.0f, 0.0f);
        const Vector3f CENTER_RIGHT_TOP(1.0f, 1.0f, 0.0f);
        const Vector3f CENTER_FRONT_TOP(0.0f, 1.0f, 1.0f);


        const auto GetNodePosition = [](const OctreeNodeT& node, const Vector3f& halfExtentScale = Vector3f(0.0f))
        {
            Vector3f position = node.GetData()->Position;
            position += halfExtentScale * node.GetData()->HalfExtent;
            return position;
        };

        AddBorderDualCell(GetNodePosition(root, CORNER_0), GetNodePosition(root, CENTER_BACK_BOTTOM), GetNodePosition(root, FACE_BOTTOM), GetNodePosition(root, CENTER_LEFT_BOTTOM),
            GetNodePosition(root, CENTER_BACK_LEFT), GetNodePosition(root, FACE_BACK), GetNodePosition(root), GetNodePosition(root, FACE_LEFT));
        AddBorderDualCell(GetNodePosition(root, CENTER_BACK_BOTTOM), GetNodePosition(root, CORNER_1), GetNodePosition(root, CENTER_RIGHT_BOTTOM), GetNodePosition(root,FACE_BOTTOM),
            GetNodePosition(root, FACE_BACK), GetNodePosition(root, CENTER_BACK_RIGHT), GetNodePosition(root, FACE_RIGHT), GetNodePosition(root));
        AddBorderDualCell(GetNodePosition(root, FACE_BOTTOM), GetNodePosition(root, CENTER_RIGHT_BOTTOM), GetNodePosition(root, CORNER_2), GetNodePosition(root , CENTER_FRONT_BOTTOM),
            GetNodePosition(root), GetNodePosition(root, FACE_RIGHT), GetNodePosition(root, CENTER_FRONT_RIGHT), GetNodePosition(root, FACE_FRONT));
        AddBorderDualCell(GetNodePosition(root, CENTER_LEFT_BOTTOM), GetNodePosition(root, FACE_BOTTOM), GetNodePosition(root, CENTER_FRONT_BOTTOM), GetNodePosition(root, CORNER_3),
            GetNodePosition(root, FACE_LEFT), GetNodePosition(root), GetNodePosition(root, FACE_FRONT), GetNodePosition(root, CENTER_FRONT_LEFT));

        AddBorderDualCell(GetNodePosition(root, CENTER_BACK_LEFT), GetNodePosition(root, FACE_BACK), GetNodePosition(root), GetNodePosition(root, FACE_LEFT),
            GetNodePosition(root, CORNER_4), GetNodePosition(root, CENTER_BACK_TOP), GetNodePosition(root, FACE_TOP), GetNodePosition(root, CENTER_LEFT_TOP));
        AddBorderDualCell(GetNodePosition(root, FACE_BACK), GetNodePosition(root, CENTER_BACK_RIGHT), GetNodePosition(root, FACE_RIGHT), GetNodePosition(root),
            GetNodePosition(root, CENTER_BACK_TOP), GetNodePosition(root, CORNER_5), GetNodePosition(root, CENTER_RIGHT_TOP), GetNodePosition(root, FACE_TOP));
        AddBorderDualCell(GetNodePosition(root), GetNodePosition(root, FACE_RIGHT), GetNodePosition(root, CENTER_FRONT_RIGHT), GetNodePosition(root, FACE_FRONT),
            GetNodePosition(root, FACE_TOP), GetNodePosition(root, CENTER_RIGHT_TOP), GetNodePosition(root, CORNER_6), GetNodePosition(root, CENTER_FRONT_TOP));
        AddBorderDualCell(GetNodePosition(root, FACE_LEFT), GetNodePosition(root), GetNodePosition(root, FACE_FRONT), GetNodePosition(root, CENTER_FRONT_LEFT),
            GetNodePosition(root, CENTER_LEFT_TOP), GetNodePosition(root, FACE_TOP), GetNodePosition(root, CENTER_FRONT_TOP), GetNodePosition(root, CORNER_7));
    }
}

template <typename OctreeNodeT>
void Dualgrid<OctreeNodeT>::NodeProc(const OctreeNodeT& node)
{
    if (node.IsSubdivided())
    {
        const OctreeNodeT& c0 = node.GetChild(Nodes[0]);
        const OctreeNodeT& c1 = node.GetChild(Nodes[1]);
        const OctreeNodeT& c2 = node.GetChild(Nodes[2]);
        const OctreeNodeT& c3 = node.GetChild(Nodes[3]);
        const OctreeNodeT& c4 = node.GetChild(Nodes[4]);
        const OctreeNodeT& c5 = node.GetChild(Nodes[5]);
        const OctreeNodeT& c6 = node.GetChild(Nodes[6]);
        const OctreeNodeT& c7 = node.GetChild(Nodes[7]);

        NodeProc(c0);
        NodeProc(c1);
        NodeProc(c2);
        NodeProc(c3);
        NodeProc(c4);
        NodeProc(c5);
        NodeProc(c6);
        NodeProc(c7);

        FaceProcXY(c0, c3);
        FaceProcXY(c1, c2);
        FaceProcXY(c4, c7);
        FaceProcXY(c5, c6);

        FaceProcZY(c0, c1);
        FaceProcZY(c3, c2);
        FaceProcZY(c4, c5);
        FaceProcZY(c7, c6);

        FaceProcXZ(c4, c0);
        FaceProcXZ(c5, c1);
        FaceProcXZ(c7, c3);
        FaceProcXZ(c6, c2);

        EdgeProcX(c0, c3, c7, c4);
        EdgeProcX(c1, c2, c6, c5);

        EdgeProcY(c0, c1, c2, c3);
        EdgeProcY(c4, c5, c6, c7);

        EdgeProcZ(c7, c6, c2, c3);
        EdgeProcZ(c4, c5, c1, c0);

        VertexProc(c0, c1, c2, c3, c4, c5, c6, c7);
    }
}

template <typename OctreeNodeT>
void Dualgrid<OctreeNodeT>::EdgeProcX(const OctreeNodeT& node0, const OctreeNodeT& node1, const OctreeNodeT& node2,
    const OctreeNodeT& node3)
{
    const bool n0Subdivided = node0.IsSubdivided();
    const bool n1Subdivided = node1.IsSubdivided();
    const bool n2Subdivided = node2.IsSubdivided();
    const bool n3Subdivided = node3.IsSubdivided();

    if (n0Subdivided || n1Subdivided || n2Subdivided || n3Subdivided)
    {
        const OctreeNodeT& c0 = n0Subdivided ? node0.GetChild(Nodes[7]) : node0;
        const OctreeNodeT& c1 = n0Subdivided ? node0.GetChild(Nodes[6]) : node0;
        const OctreeNodeT& c2 = n1Subdivided ? node1.GetChild(Nodes[5]) : node1;
        const OctreeNodeT& c3 = n1Subdivided ? node1.GetChild(Nodes[4]) : node1;
        const OctreeNodeT& c4 = n3Subdivided ? node3.GetChild(Nodes[3]) : node3;
        const OctreeNodeT& c5 = n3Subdivided ? node3.GetChild(Nodes[2]) : node3;
        const OctreeNodeT& c6 = n2Subdivided ? node2.GetChild(Nodes[1]) : node2;
        const OctreeNodeT& c7 = n2Subdivided ? node2.GetChild(Nodes[0]) : node2;

        EdgeProcX(c0, c3, c7, c4);
        EdgeProcX(c1, c2, c6, c5);

        VertexProc(c0, c1, c2, c3, c4, c5, c6, c7);
    }
}

template <typename OctreeNodeT>
void Dualgrid<OctreeNodeT>::EdgeProcY(const OctreeNodeT& node0, const OctreeNodeT& node1, const OctreeNodeT& node2,
    const OctreeNodeT& node3)
{
    const bool n0Subdivided = node0.IsSubdivided();
    const bool n1Subdivided = node1.IsSubdivided();
    const bool n2Subdivided = node2.IsSubdivided();
    const bool n3Subdivided = node3.IsSubdivided();

    if (n0Subdivided || n1Subdivided || n2Subdivided || n3Subdivided)
    {
        const OctreeNodeT& c0 = n0Subdivided ? node0.GetChild(Nodes[2]) : node0;
        const OctreeNodeT& c1 = n1Subdivided ? node1.GetChild(Nodes[3]) : node1;
        const OctreeNodeT& c2 = n2Subdivided ? node2.GetChild(Nodes[0]) : node2;
        const OctreeNodeT& c3 = n3Subdivided ? node3.GetChild(Nodes[1]) : node3;
        const OctreeNodeT& c4 = n0Subdivided ? node0.GetChild(Nodes[6]) : node0;
        const OctreeNodeT& c5 = n1Subdivided ? node1.GetChild(Nodes[7]) : node1;
        const OctreeNodeT& c6 = n2Subdivided ? node2.GetChild(Nodes[4]) : node2;
        const OctreeNodeT& c7 = n3Subdivided ? node3.GetChild(Nodes[5]) : node3;

        EdgeProcY(c0, c1, c2, c3);
        EdgeProcY(c4, c5, c6, c7);

        VertexProc(c0, c1, c2, c3, c4, c5, c6, c7);
    }
}

template <typename OctreeNodeT>
void Dualgrid<OctreeNodeT>::EdgeProcZ(const OctreeNodeT& node0, const OctreeNodeT& node1, const OctreeNodeT& node2,
    const OctreeNodeT& node3)
{
    const bool n0Subdivided = node0.IsSubdivided();
    const bool n1Subdivided = node1.IsSubdivided();
    const bool n2Subdivided = node2.IsSubdivided();
    const bool n3Subdivided = node3.IsSubdivided();

    if (n0Subdivided || n1Subdivided || n2Subdivided || n3Subdivided)
    {
        const OctreeNodeT& c0 = n3Subdivided ? node3.GetChild(Nodes[5]) : node3;
        const OctreeNodeT& c1 = n2Subdivided ? node2.GetChild(Nodes[4]) : node2;
        const OctreeNodeT& c2 = n2Subdivided ? node2.GetChild(Nodes[7]) : node2;
        const OctreeNodeT& c3 = n3Subdivided ? node3.GetChild(Nodes[6]) : node3;
        const OctreeNodeT& c4 = n0Subdivided ? node0.GetChild(Nodes[1]) : node0;
        const OctreeNodeT& c5 = n1Subdivided ? node1.GetChild(Nodes[0]) : node1;
        const OctreeNodeT& c6 = n1Subdivided ? node1.GetChild(Nodes[3]) : node1;
        const OctreeNodeT& c7 = n0Subdivided ? node0.GetChild(Nodes[2]) : node0;

        EdgeProcZ(c7, c6, c2, c3);
        EdgeProcZ(c4, c5, c1, c0);

        VertexProc(c0, c1, c2, c3, c4, c5, c6, c7);
    }
}

template <typename OctreeNodeT>
void Dualgrid<OctreeNodeT>::FaceProcXY(const OctreeNodeT& node0, const OctreeNodeT& node1)
{
    const bool n0Subdivided = node0.IsSubdivided();
    const bool n1Subdivided = node1.IsSubdivided();
    if (n0Subdivided || n1Subdivided)
    {
        const OctreeNodeT& child0 = n0Subdivided ? node0.GetChild(Nodes[3]) : node0;
        const OctreeNodeT& child1 = n0Subdivided ? node0.GetChild(Nodes[2]) : node0;
        const OctreeNodeT& child2 = n1Subdivided ? node1.GetChild(Nodes[1]) : node1;
        const OctreeNodeT& child3 = n1Subdivided ? node1.GetChild(Nodes[0]) : node1;

        const OctreeNodeT& child4 = n0Subdivided ? node0.GetChild(Nodes[7]) : node0;
        const OctreeNodeT& child5 = n0Subdivided ? node0.GetChild(Nodes[6]) : node0;
        const OctreeNodeT& child6 = n1Subdivided ? node1.GetChild(Nodes[5]) : node1;
        const OctreeNodeT& child7 = n1Subdivided ? node1.GetChild(Nodes[4]) : node1;

        FaceProcXY(child0, child3);
        FaceProcXY(child1, child2);
        FaceProcXY(child4, child7);
        FaceProcXY(child5, child6);

        EdgeProcX(child0, child3, child7, child4);
        EdgeProcX(child1, child2, child6, child5);

        EdgeProcY(child0, child1, child2, child3);
        EdgeProcY(child4, child5, child6, child7);

        VertexProc(child0, child1, child2, child3, child4, child5, child6, child7);
    }
}

template <typename OctreeNodeT>
void Dualgrid<OctreeNodeT>::FaceProcZY(const OctreeNodeT& node0, const OctreeNodeT& node1)
{
const bool n0Subdivided = node0.IsSubdivided();
const bool n1Subdivided = node1.IsSubdivided();
if (n0Subdivided || n1Subdivided)
{
    const OctreeNodeT& child0 = n0Subdivided ? node0.GetChild(Nodes[1]) : node0;
    const OctreeNodeT& child1 = n1Subdivided ? node1.GetChild(Nodes[0]) : node1;
    const OctreeNodeT& child2 = n1Subdivided ? node1.GetChild(Nodes[3]) : node1;
    const OctreeNodeT& child3 = n0Subdivided ? node0.GetChild(Nodes[2]) : node0;

    const OctreeNodeT& child4 = n0Subdivided ? node0.GetChild(Nodes[5]) : node0;
    const OctreeNodeT& child5 = n1Subdivided ? node1.GetChild(Nodes[4]) : node1;
    const OctreeNodeT& child6 = n1Subdivided ? node1.GetChild(Nodes[7]) : node1;
    const OctreeNodeT& child7 = n0Subdivided ? node0.GetChild(Nodes[6]) : node0;

    FaceProcZY(child0, child1);
    FaceProcZY(child3, child2);
    FaceProcZY(child4, child5);
    FaceProcZY(child7, child6);

    EdgeProcY(child0, child1, child2, child3);
    EdgeProcY(child4, child5, child6, child7);

    EdgeProcZ(child7, child6, child2, child3);
    EdgeProcZ(child4, child5, child1, child0);

    VertexProc(child0, child1, child2, child3, child4, child5, child6, child7);
}
}

template <typename OctreeNodeT>
void Dualgrid<OctreeNodeT>::FaceProcXZ(const OctreeNodeT& node0, const OctreeNodeT& node1)
{
    const bool n0Subdivided = node0.IsSubdivided();
    const bool n1Subdivided = node1.IsSubdivided();
    if (n0Subdivided || n1Subdivided)
    {
        
        const OctreeNodeT& child0 = n1Subdivided ? node1.GetChild(Nodes[4]) : node1;
        const OctreeNodeT& child1 = n1Subdivided ? node1.GetChild(Nodes[5]) : node1;
        const OctreeNodeT& child2 = n1Subdivided ? node1.GetChild(Nodes[6]) : node1;
        const OctreeNodeT& child3 = n1Subdivided ? node1.GetChild(Nodes[7]) : node1;

        const OctreeNodeT& child4 = n0Subdivided ? node0.GetChild(Nodes[0]) : node0;
        const OctreeNodeT& child5 = n0Subdivided ? node0.GetChild(Nodes[1]) : node0;
        const OctreeNodeT& child6 = n0Subdivided ? node0.GetChild(Nodes[2]) : node0;
        const OctreeNodeT& child7 = n0Subdivided ? node0.GetChild(Nodes[3]) : node0;

        FaceProcXZ(child4, child0);
        FaceProcXZ(child5, child1);
        FaceProcXZ(child7, child3);
        FaceProcXZ(child6, child2);

        EdgeProcX(child0, child3, child7, child4);
        EdgeProcX(child1, child2, child6, child5);

        EdgeProcZ(child7, child6, child2, child3);
        EdgeProcZ(child4, child5, child1, child0);

        VertexProc(child0, child1, child2, child3, child4, child5, child6, child7);
    }
}

template <typename OctreeNodeT>
void Dualgrid<OctreeNodeT>::VertexProc(const OctreeNodeT& node0, const OctreeNodeT& node1, const OctreeNodeT& node2,
    const OctreeNodeT& node3, const OctreeNodeT& node4, const OctreeNodeT& node5, const OctreeNodeT& node6,
    const OctreeNodeT& node7)
{
    const bool n0Subdivided = node0.IsSubdivided();
    const bool n1Subdivided = node1.IsSubdivided();
    const bool n2Subdivided = node2.IsSubdivided();
    const bool n3Subdivided = node3.IsSubdivided();
    const bool n4Subdivided = node4.IsSubdivided();
    const bool n5Subdivided = node5.IsSubdivided();
    const bool n6Subdivided = node6.IsSubdivided();
    const bool n7Subdivided = node7.IsSubdivided();

    if (n0Subdivided || n1Subdivided || n2Subdivided || n3Subdivided ||
        n4Subdivided || n5Subdivided || n6Subdivided || n7Subdivided)
    {
        const OctreeNodeT& c0 = n0Subdivided ? node0.GetChild(Nodes[6]) : node0;
        const OctreeNodeT& c1 = n1Subdivided ? node1.GetChild(Nodes[7]) : node1;
        const OctreeNodeT& c2 = n2Subdivided ? node2.GetChild(Nodes[4]) : node2;
        const OctreeNodeT& c3 = n3Subdivided ? node3.GetChild(Nodes[5]) : node3;
        const OctreeNodeT& c4 = n4Subdivided ? node4.GetChild(Nodes[2]) : node4;
        const OctreeNodeT& c5 = n5Subdivided ? node5.GetChild(Nodes[3]) : node5;
        const OctreeNodeT& c6 = n6Subdivided ? node6.GetChild(Nodes[0]) : node6;
        const OctreeNodeT& c7 = n7Subdivided ? node7.GetChild(Nodes[1]) : node7;

        VertexProc(c0, c1, c2, c3, c4, c5, c6, c7);
    }
    else
    {
        /*if ((node0.GetData().IsIsoNear == false) &&
            (node1.GetData().IsIsoNear == false ) &&
            (node2.GetData().IsIsoNear == false ) &&
            (node3.GetData().IsIsoNear == false ) &&
            (node4.GetData().IsIsoNear == false ) &&
            (node5.GetData().IsIsoNear == false ) &&
            (node6.GetData().IsIsoNear == false ) &&
            (node7.GetData().IsIsoNear == false ))
        {
            return;
        }*/

        // can we reserve the vector?
        //DualCell cell(node0, node1, node2, node3, node4, node5, node6, node7);
        AddDualCell(node0, node1, node2, node3, node4, node5, node6, node7);
        CreateBorderCell(node0, node1, node2, node3, node4, node5, node6, node7);
        // Create mesh here
    }
}

template <typename OctreeNodeT>
void Dualgrid<OctreeNodeT>::AddDualCell(const OctreeNodeT& node0, const OctreeNodeT& node1, const OctreeNodeT& node2,
    const OctreeNodeT& node3, const OctreeNodeT& node4, const OctreeNodeT& node5, const OctreeNodeT& node6,
    const OctreeNodeT& node7)
{
    m_DualCells.emplace_back(node0, node1, node2, node3, node4, node5, node6, node7);

    Vector3f corners[8];
    corners[0] = node0.GetData()->Position;
    corners[1] = node1.GetData()->Position;
    corners[2] = node2.GetData()->Position;
    corners[3] = node3.GetData()->Position;
    corners[4] = node4.GetData()->Position;
    corners[5] = node5.GetData()->Position;
    corners[6] = node6.GetData()->Position;
    corners[7] = node7.GetData()->Position;

    Vector4f values[8];
    values[0] = node0.GetData()->Gradient;
    values[1] = node1.GetData()->Gradient;
    values[2] = node2.GetData()->Gradient;
    values[3] = node3.GetData()->Gradient;
    values[4] = node4.GetData()->Gradient;
    values[5] = node5.GetData()->Gradient;
    values[6] = node6.GetData()->Gradient;
    values[7] = node7.GetData()->Gradient;

#if USE_ANALYTICAL_NORMAL
    //const SimplexNoiseSource* noiseSource = static_cast<const SimplexNoiseSource*>(m_Volume);

    Vector3f analyticalNormals[8];
    analyticalNormals[0] = node0.GetData()->AnalyticalNormal;
    analyticalNormals[1] = node1.GetData()->AnalyticalNormal;
    analyticalNormals[2] = node2.GetData()->AnalyticalNormal;
    analyticalNormals[3] = node3.GetData()->AnalyticalNormal;
    analyticalNormals[4] = node4.GetData()->AnalyticalNormal;
    analyticalNormals[5] = node5.GetData()->AnalyticalNormal;
    analyticalNormals[6] = node6.GetData()->AnalyticalNormal;
    analyticalNormals[7] = node7.GetData()->AnalyticalNormal;
#else
    Vector3f analyticalNormals[8];
    analyticalNormals[0] = Vector3f(node0.GetData()->Gradient);
    analyticalNormals[1] = Vector3f(node1.GetData()->Gradient);
    analyticalNormals[2] = Vector3f(node2.GetData()->Gradient);
    analyticalNormals[3] = Vector3f(node3.GetData()->Gradient);
    analyticalNormals[4] = Vector3f(node4.GetData()->Gradient);
    analyticalNormals[5] = Vector3f(node5.GetData()->Gradient);
    analyticalNormals[6] = Vector3f(node6.GetData()->Gradient);
    analyticalNormals[7] = Vector3f(node7.GetData()->Gradient);
#endif

    m_IsoSurface->AddTriangles(corners, values, analyticalNormals);

    //const Vector3f rootPosition = m_OctreeRoot->GetData()->Position;
    //const onyxF32 halfExtents = m_OctreeRoot->GetData()->HalfExtent;
    //Vector3f from = rootPosition - Vector3f(halfExtents, halfExtents, halfExtents);
    //Vector3f to = rootPosition + Vector3f(halfExtents, halfExtents, halfExtents);

    //if (corners[0][2] == from[2]/* && corners[0].z != mTotalFrom.z*/)
    //{
    //    m_MarchingSquaresIsoSurface->AddMarchingSquaresTriangles(corners, values, IsoSurface<onyxF32>::MS_CORNERS_BACK);
    //}
    //if (corners[2][2] == to[2]/* && corners[2].z != mTotalTo.z*/)
    //{
    //    m_MarchingSquaresIsoSurface->AddMarchingSquaresTriangles(corners, values, IsoSurface<onyxF32>::MS_CORNERS_FRONT);
    //}
    //if (corners[0][0] == from[0]/* && corners[0].x != mTotalFrom.x*/)
    //{
    //    m_MarchingSquaresIsoSurface->AddMarchingSquaresTriangles(corners, values, IsoSurface<onyxF32>::MS_CORNERS_LEFT);
    //}
    //if (corners[1][0] == to[0]/* && corners[1].x != mTotalTo.x*/)
    //{
    //    m_MarchingSquaresIsoSurface->AddMarchingSquaresTriangles(corners, values, IsoSurface<onyxF32>::MS_CORNERS_RIGHT);
    //}
    //if (corners[5][1] == to[1]/* && corners[5].y != mTotalTo.y*/)
    //{
    //    m_MarchingSquaresIsoSurface->AddMarchingSquaresTriangles(corners, values, IsoSurface<onyxF32>::MS_CORNERS_TOP);
    //}
    //if (corners[0][1] == from[1]/* && corners[0].y != mTotalFrom.y*/)
    //{
    //    m_MarchingSquaresIsoSurface->AddMarchingSquaresTriangles(corners, values, IsoSurface<onyxF32>::MS_CORNERS_BOTTOM);
    //}
}

template <typename OctreeNodeT>
void Dualgrid<OctreeNodeT>::CreateBorderCell(const OctreeNodeT& node0, const OctreeNodeT& node1, const OctreeNodeT& node2, const OctreeNodeT& node3, const OctreeNodeT& node4, const OctreeNodeT& node5, const OctreeNodeT& node6, const OctreeNodeT& node7)
{
    const auto IsFlagSet = [](const OctreeNodeT& node, VolumeOctreeNodeMetaData flag) -> bool { return static_cast<onyxU8>(node.GetData()->MetaData & flag) != 0; };

    //1.0 = + halfExtent
    //0.0 = center
    //-1.0 = - halfExtent

    const Vector3f CORNER_0(-1.0f, -1.0f, -1.0f);
    const Vector3f CORNER_1(1.0f, -1.0f, -1.0f);
    const Vector3f CORNER_2(1.0f, -1.0f, 1.0f);
    const Vector3f CORNER_3(-1.0f, -1.0f, 1.0f);
    const Vector3f CORNER_4(-1.0f, 1.0f, -1.0f);
    const Vector3f CORNER_5(1.0f, 1.0f, -1.0f);
    const Vector3f CORNER_6(1.0f, 1.0f, 1.0f);
    const Vector3f CORNER_7(-1.0f, 1.0f, 1.0f);

    const Vector3f FACE_TOP(0.0f, 1.0f, 0.0f);
    const Vector3f FACE_BOTTOM(0.0f, -1.0f, 0.0f);
    const Vector3f FACE_RIGHT(1.0f, 0.0f, 0.0f);
    const Vector3f FACE_LEFT(-1.0f, 0.0f, 0.0f);
    const Vector3f FACE_FRONT(-1.0f, 0.0f, 1.0f);
    const Vector3f FACE_BACK(0.0f, 0.0f, -1.0f);

    const Vector3f CENTER_BACK_BOTTOM(0.0f, -1.0f, -1.0f);
    const Vector3f CENTER_FRONT_BOTTOM(0.0f, -1.0f, 1.0f);
    const Vector3f CENTER_LEFT_BOTTOM(-1.0f, -1.0f, 0.0f);
    const Vector3f CENTER_RIGHT_BOTTOM(1.0f, -1.0f, 0.0f);
    
    const Vector3f CENTER_BACK_LEFT(-1.0f, 0.0f, -1.0f);
    const Vector3f CENTER_BACK_RIGHT(1.0f, 0.0f, -1.0f);
    const Vector3f CENTER_FRONT_LEFT(-1.0f, 0.0f, 1.0f);
    const Vector3f CENTER_FRONT_RIGHT(1.0f, 0.0f, 1.0f);

    const Vector3f CENTER_BACK_TOP(0.0f, 1.0f, -1.0f);
    const Vector3f CENTER_LEFT_TOP(-1.0f, 1.0f, 0.0f);
    const Vector3f CENTER_RIGHT_TOP(1.0f, 1.0f, 0.0f);
    const Vector3f CENTER_FRONT_TOP(0.0f, 1.0f, 1.0f);


    const auto GetNodePosition = [](const OctreeNodeT& node, const Vector3f& halfExtentScale = Vector3f(0.0f))
    {
        Vector3f position = node.GetData()->Position;
        position += halfExtentScale * node.GetData()->HalfExtent;
        return position;
    };

    if (IsFlagSet(node0, VolumeOctreeNodeMetaData::BorderBack) &&
        IsFlagSet(node1, VolumeOctreeNodeMetaData::BorderBack) &&
        IsFlagSet(node4, VolumeOctreeNodeMetaData::BorderBack) &&
        IsFlagSet(node5, VolumeOctreeNodeMetaData::BorderBack))
    {
        AddBorderDualCell(GetNodePosition(node0, FACE_BACK), GetNodePosition(node1, FACE_BACK), GetNodePosition(node1), GetNodePosition(node0),
            GetNodePosition(node4, FACE_BACK), GetNodePosition(node5, FACE_BACK), GetNodePosition(node5), GetNodePosition(node4));

        if (IsFlagSet(node4, VolumeOctreeNodeMetaData::BorderTop) && IsFlagSet(node5, VolumeOctreeNodeMetaData::BorderTop))
        {
            AddBorderDualCell(GetNodePosition(node4, FACE_BACK), GetNodePosition(node5, FACE_BACK), GetNodePosition(node5), GetNodePosition(node4),
                GetNodePosition(node4, CENTER_BACK_TOP), GetNodePosition(node5, CENTER_BACK_TOP), GetNodePosition(node5, FACE_TOP), GetNodePosition(node4, FACE_TOP));

            // Generate back top corner cells
            if (IsFlagSet(node4, VolumeOctreeNodeMetaData::BorderLeft))
            {
                AddBorderDualCell(GetNodePosition(node4, CENTER_BACK_LEFT), GetNodePosition(node4, FACE_BACK), GetNodePosition(node4), GetNodePosition(node4, FACE_LEFT),
                    GetNodePosition(node4, CORNER_4), GetNodePosition(node4, CENTER_BACK_TOP), GetNodePosition(node4, FACE_TOP), GetNodePosition(node4, CENTER_LEFT_TOP));
            }
            if (IsFlagSet(node5, VolumeOctreeNodeMetaData::BorderRight))
            {
                AddBorderDualCell(GetNodePosition(node5, FACE_BACK), GetNodePosition(node5, CENTER_BACK_RIGHT), GetNodePosition(node5, FACE_RIGHT), GetNodePosition(node5),
                    GetNodePosition(node5, CENTER_BACK_TOP), GetNodePosition(node5, CORNER_5), GetNodePosition(node5, CENTER_RIGHT_TOP), GetNodePosition(node5, FACE_TOP));
            }
        }
        if (IsFlagSet(node0, VolumeOctreeNodeMetaData::BorderBottom) && IsFlagSet(node1, VolumeOctreeNodeMetaData::BorderBottom))
        {
            AddBorderDualCell(GetNodePosition(node0, CENTER_BACK_BOTTOM), GetNodePosition(node1, CENTER_BACK_BOTTOM), GetNodePosition(node1, FACE_BOTTOM), GetNodePosition(node0, FACE_BOTTOM),
                GetNodePosition(node0, FACE_BACK), GetNodePosition(node1, FACE_BACK), GetNodePosition(node1), GetNodePosition(node0));

            // Generate back bottom corner cells
            if (IsFlagSet(node0, VolumeOctreeNodeMetaData::BorderLeft))
            {
                AddBorderDualCell(GetNodePosition(node0, CORNER_0), GetNodePosition(node0, CENTER_BACK_BOTTOM), GetNodePosition(node0, FACE_BOTTOM), GetNodePosition(node0, CENTER_LEFT_BOTTOM),
                    GetNodePosition(node0, CENTER_BACK_LEFT), GetNodePosition(node0, FACE_BACK), GetNodePosition(node0), GetNodePosition(node0, FACE_LEFT));
            }
            if (IsFlagSet(node1, VolumeOctreeNodeMetaData::BorderRight))
            {
                AddBorderDualCell(GetNodePosition(node1, CENTER_BACK_BOTTOM), GetNodePosition(node1, CORNER_1), GetNodePosition(node1, CENTER_RIGHT_BOTTOM), GetNodePosition(node1, FACE_BOTTOM),
                    GetNodePosition(node1, FACE_BACK), GetNodePosition(node1, CENTER_BACK_RIGHT), GetNodePosition(node1, FACE_RIGHT), GetNodePosition(node1));
            }
        }

    }

    if (IsFlagSet(node2, VolumeOctreeNodeMetaData::BorderFront) &&
        IsFlagSet(node3, VolumeOctreeNodeMetaData::BorderFront) &&
        IsFlagSet(node6, VolumeOctreeNodeMetaData::BorderFront) &&
        IsFlagSet(node7, VolumeOctreeNodeMetaData::BorderFront))
    {
        AddBorderDualCell(GetNodePosition(node3), GetNodePosition(node2), GetNodePosition(node2, FACE_FRONT), GetNodePosition(node3, FACE_FRONT),
            GetNodePosition(node7), GetNodePosition(node6), GetNodePosition(node6, FACE_FRONT), GetNodePosition(node7, FACE_FRONT));

        // Generate front edge border cells
        if (IsFlagSet(node6, VolumeOctreeNodeMetaData::BorderTop) && IsFlagSet(node7, VolumeOctreeNodeMetaData::BorderTop))
        {
            AddBorderDualCell(GetNodePosition(node7), GetNodePosition(node6), GetNodePosition(node6, FACE_FRONT), GetNodePosition(node7, FACE_FRONT),
                GetNodePosition(node7, FACE_TOP), GetNodePosition(node6, FACE_TOP), GetNodePosition(node6, CENTER_FRONT_TOP), GetNodePosition(node7, CENTER_FRONT_TOP));
            // Generate back bottom corner cells
            if (IsFlagSet(node7, VolumeOctreeNodeMetaData::BorderLeft))
            {
                AddBorderDualCell(GetNodePosition(node7, FACE_LEFT), GetNodePosition(node7), GetNodePosition(node7, FACE_FRONT), GetNodePosition(node7, CENTER_FRONT_LEFT),
                    GetNodePosition(node7, CENTER_LEFT_TOP), GetNodePosition(node7, FACE_TOP), GetNodePosition(node7, CENTER_FRONT_TOP), GetNodePosition(node7, CORNER_7));
            }
            if (IsFlagSet(node6, VolumeOctreeNodeMetaData::BorderRight))
            {
                AddBorderDualCell(GetNodePosition(node6), GetNodePosition(node6, FACE_RIGHT), GetNodePosition(node6, CENTER_FRONT_RIGHT), GetNodePosition(node6, FACE_FRONT),
                    GetNodePosition(node6, FACE_TOP), GetNodePosition(node6, CENTER_RIGHT_TOP), GetNodePosition(node6, CORNER_6), GetNodePosition(node6, CENTER_FRONT_TOP));
            }
        }
        if (IsFlagSet(node3, VolumeOctreeNodeMetaData::BorderBottom) && IsFlagSet(node2, VolumeOctreeNodeMetaData::BorderBottom))
        {
            AddBorderDualCell(GetNodePosition(node3, FACE_BOTTOM), GetNodePosition(node2, FACE_BOTTOM), GetNodePosition(node2, CENTER_FRONT_BOTTOM), GetNodePosition(node3, CENTER_FRONT_BOTTOM),
                GetNodePosition(node3), GetNodePosition(node2), GetNodePosition(node2, FACE_FRONT), GetNodePosition(node3, FACE_FRONT));

            // Generate back bottom corner cells
            if (IsFlagSet(node3, VolumeOctreeNodeMetaData::BorderLeft))
            {
                AddBorderDualCell(GetNodePosition(node3, CENTER_LEFT_BOTTOM), GetNodePosition(node3, FACE_BOTTOM), GetNodePosition(node3, CENTER_FRONT_BOTTOM), GetNodePosition(node3, CORNER_3),
                    GetNodePosition(node3, FACE_LEFT), GetNodePosition(node3), GetNodePosition(node3, FACE_FRONT), GetNodePosition(node3, CENTER_FRONT_LEFT));
            }
            if (IsFlagSet(node2, VolumeOctreeNodeMetaData::BorderRight))
            {
                AddBorderDualCell(GetNodePosition(node2, FACE_BOTTOM), GetNodePosition(node2, CENTER_RIGHT_BOTTOM), GetNodePosition(node2, CORNER_2), GetNodePosition(node2, CENTER_FRONT_BOTTOM),
                    GetNodePosition(node2), GetNodePosition(node2, FACE_RIGHT), GetNodePosition(node2, CENTER_FRONT_RIGHT), GetNodePosition(node2, FACE_FRONT));
            }
        }
    }

    if (IsFlagSet(node0, VolumeOctreeNodeMetaData::BorderLeft) &&
        IsFlagSet(node3, VolumeOctreeNodeMetaData::BorderLeft) &&
        IsFlagSet(node4, VolumeOctreeNodeMetaData::BorderLeft) &&
        IsFlagSet(node7, VolumeOctreeNodeMetaData::BorderLeft))
    {
        AddBorderDualCell(GetNodePosition(node0, FACE_LEFT), GetNodePosition(node0), GetNodePosition(node3), GetNodePosition(node3, FACE_LEFT),
            GetNodePosition(node4, FACE_LEFT), GetNodePosition(node4), GetNodePosition(node7), GetNodePosition(node7, FACE_LEFT));

        // Generate left edge border cells
        if (IsFlagSet(node4, VolumeOctreeNodeMetaData::BorderTop) && IsFlagSet(node7, VolumeOctreeNodeMetaData::BorderTop))
        {
            AddBorderDualCell(GetNodePosition(node4, FACE_LEFT), GetNodePosition(node4), GetNodePosition(node7), GetNodePosition(node7, FACE_LEFT),
                GetNodePosition(node4, CENTER_LEFT_TOP), GetNodePosition(node4, FACE_TOP), GetNodePosition(node7, FACE_TOP), GetNodePosition(node7, CENTER_LEFT_TOP));
        }
        if (IsFlagSet(node0, VolumeOctreeNodeMetaData::BorderBottom) && IsFlagSet(node3, VolumeOctreeNodeMetaData::BorderBottom))
        {
            AddBorderDualCell(GetNodePosition(node0, CENTER_LEFT_BOTTOM), GetNodePosition(node0, FACE_BOTTOM), GetNodePosition(node3, FACE_BOTTOM), GetNodePosition(node3, CENTER_LEFT_BOTTOM),
                GetNodePosition(node0, FACE_LEFT), GetNodePosition(node0), GetNodePosition(node3), GetNodePosition(node3, FACE_LEFT));
        }
        if (IsFlagSet(node0, VolumeOctreeNodeMetaData::BorderBack) && IsFlagSet(node4, VolumeOctreeNodeMetaData::BorderBack))
        {
            AddBorderDualCell(GetNodePosition(node0, CENTER_BACK_LEFT), GetNodePosition(node0, FACE_BACK), GetNodePosition(node0), GetNodePosition(node0, FACE_LEFT),
                GetNodePosition(node4, CENTER_BACK_LEFT), GetNodePosition(node4, FACE_BACK), GetNodePosition(node4), GetNodePosition(node4, FACE_LEFT));
        }
        if (IsFlagSet(node3, VolumeOctreeNodeMetaData::BorderFront) && IsFlagSet(node7, VolumeOctreeNodeMetaData::BorderFront))
        {
            AddBorderDualCell(GetNodePosition(node3, FACE_LEFT), GetNodePosition(node3), GetNodePosition(node3, FACE_FRONT), GetNodePosition(node3, CENTER_FRONT_LEFT),
                GetNodePosition(node7, FACE_LEFT), GetNodePosition(node7), GetNodePosition(node7, FACE_FRONT), GetNodePosition(node7, CENTER_FRONT_LEFT));
        }
    }

    if (IsFlagSet(node1, VolumeOctreeNodeMetaData::BorderRight) &&
        IsFlagSet(node2, VolumeOctreeNodeMetaData::BorderRight) &&
        IsFlagSet(node5, VolumeOctreeNodeMetaData::BorderRight) &&
        IsFlagSet(node6, VolumeOctreeNodeMetaData::BorderRight))
    {
        AddBorderDualCell(GetNodePosition(node1), GetNodePosition(node1, FACE_RIGHT), GetNodePosition(node2, FACE_RIGHT), GetNodePosition(node2),
            GetNodePosition(node5), GetNodePosition(node5, FACE_RIGHT), GetNodePosition(node6, FACE_RIGHT), GetNodePosition(node6));

        // Generate right edge border cells
        if (IsFlagSet(node5, VolumeOctreeNodeMetaData::BorderTop) && IsFlagSet(node6, VolumeOctreeNodeMetaData::BorderTop))
        {
            AddBorderDualCell(GetNodePosition(node5), GetNodePosition(node5, FACE_RIGHT), GetNodePosition(node6, FACE_RIGHT), GetNodePosition(node6),
                GetNodePosition(node5, FACE_TOP), GetNodePosition(node5, CENTER_RIGHT_TOP), GetNodePosition(node6, CENTER_RIGHT_TOP), GetNodePosition(node6, FACE_TOP));
        }
        if (IsFlagSet(node1, VolumeOctreeNodeMetaData::BorderBottom) && IsFlagSet(node2, VolumeOctreeNodeMetaData::BorderBottom))
        {
            AddBorderDualCell(GetNodePosition(node1, FACE_BOTTOM), GetNodePosition(node1, CENTER_RIGHT_BOTTOM), GetNodePosition(node2, CENTER_RIGHT_BOTTOM), GetNodePosition(node2, FACE_BOTTOM),
                GetNodePosition(node1), GetNodePosition(node1, FACE_RIGHT), GetNodePosition(node2, FACE_RIGHT), GetNodePosition(node2));
        }
        if (IsFlagSet(node1, VolumeOctreeNodeMetaData::BorderBack) && IsFlagSet(node5, VolumeOctreeNodeMetaData::BorderBack))
        {
            AddBorderDualCell(GetNodePosition(node1, FACE_BACK), GetNodePosition(node1, CENTER_BACK_RIGHT), GetNodePosition(node1, FACE_RIGHT), GetNodePosition(node1),
                GetNodePosition(node5, FACE_BACK), GetNodePosition(node5, CENTER_BACK_RIGHT), GetNodePosition(node5, FACE_RIGHT), GetNodePosition(node5));
        }
        if (IsFlagSet(node2, VolumeOctreeNodeMetaData::BorderFront) && IsFlagSet(node6, VolumeOctreeNodeMetaData::BorderFront))
        {
            AddBorderDualCell(GetNodePosition(node2), GetNodePosition(node2, FACE_RIGHT), GetNodePosition(node2, CENTER_FRONT_RIGHT), GetNodePosition(node2, FACE_FRONT),
                GetNodePosition(node6), GetNodePosition(node6, FACE_RIGHT), GetNodePosition(node6, CENTER_FRONT_RIGHT), GetNodePosition(node6, FACE_FRONT));
        }
    }

    if (IsFlagSet(node4, VolumeOctreeNodeMetaData::BorderTop) &&
        IsFlagSet(node5, VolumeOctreeNodeMetaData::BorderTop) &&
        IsFlagSet(node6, VolumeOctreeNodeMetaData::BorderTop) &&
        IsFlagSet(node7, VolumeOctreeNodeMetaData::BorderTop))
    {

        AddBorderDualCell(GetNodePosition(node4), GetNodePosition(node5), GetNodePosition(node6), GetNodePosition(node7),
            GetNodePosition(node4, FACE_TOP), GetNodePosition(node5, FACE_TOP), GetNodePosition(node6, FACE_TOP), GetNodePosition(node7, FACE_TOP));
    }

    if (IsFlagSet(node0, VolumeOctreeNodeMetaData::BorderBottom) &&
        IsFlagSet(node1, VolumeOctreeNodeMetaData::BorderBottom) &&
        IsFlagSet(node2, VolumeOctreeNodeMetaData::BorderBottom) &&
        IsFlagSet(node3, VolumeOctreeNodeMetaData::BorderBottom))
    {
        AddBorderDualCell(GetNodePosition(node0, FACE_BOTTOM), GetNodePosition(node1, FACE_BOTTOM), GetNodePosition(node2, FACE_BOTTOM), GetNodePosition(node3, FACE_BOTTOM),
            GetNodePosition(node0), GetNodePosition(node1), GetNodePosition(node2), GetNodePosition(node3));
    }
}

template <typename OctreeNodeT>
void Dualgrid<OctreeNodeT>::AddBorderDualCell(const Vector3f& position0, const Vector3f& position1, const Vector3f& position2, const Vector3f& position3, const Vector3f& position4, const Vector3f& position5, const Vector3f& position6, const Vector3f& position7)
{
    // TODO add to debug view
    //m_DualCells.emplace_back(node0, node1, node2, node3, node4, node5, node6, node7);

    Vector3f corners[8]
    {
        position0,
        position1,
        position2,
        position3,
        position4,
        position5,
        position6,
        position7
    };

    Vector4f values[8]
    {
        m_Volume->GetValueAndGradient(position0 + m_OctreeRootWorldPosition),
        m_Volume->GetValueAndGradient(position1 + m_OctreeRootWorldPosition),
        m_Volume->GetValueAndGradient(position2 + m_OctreeRootWorldPosition),
        m_Volume->GetValueAndGradient(position3 + m_OctreeRootWorldPosition),
        m_Volume->GetValueAndGradient(position4 + m_OctreeRootWorldPosition),
        m_Volume->GetValueAndGradient(position5 + m_OctreeRootWorldPosition),
        m_Volume->GetValueAndGradient(position6 + m_OctreeRootWorldPosition),
        m_Volume->GetValueAndGradient(position7 + m_OctreeRootWorldPosition)
    };

    
#if USE_ANALYTICAL_NORMAL
    const SimplexNoiseSource* noiseSource = static_cast<const SimplexNoiseSource*>(m_Volume);
    
    Vector3f analyticalNormals[8];

    analyticalNormals[0] = noiseSource->GetAnalyticalNormal(position0 + m_OctreeRootWorldPosition);
    analyticalNormals[1] = noiseSource->GetAnalyticalNormal(position1 + m_OctreeRootWorldPosition);
    analyticalNormals[2] = noiseSource->GetAnalyticalNormal(position2 + m_OctreeRootWorldPosition);
    analyticalNormals[3] = noiseSource->GetAnalyticalNormal(position3 + m_OctreeRootWorldPosition);
    analyticalNormals[4] = noiseSource->GetAnalyticalNormal(position4 + m_OctreeRootWorldPosition);
    analyticalNormals[5] = noiseSource->GetAnalyticalNormal(position5 + m_OctreeRootWorldPosition);
    analyticalNormals[6] = noiseSource->GetAnalyticalNormal(position6 + m_OctreeRootWorldPosition);
    analyticalNormals[7] = noiseSource->GetAnalyticalNormal(position7 + m_OctreeRootWorldPosition);
#else
    Vector3f analyticalNormals[8];
    analyticalNormals[0] = Vector3f(values[0]);
    analyticalNormals[1] = Vector3f(values[1]);
    analyticalNormals[2] = Vector3f(values[2]);
    analyticalNormals[3] = Vector3f(values[3]);
    analyticalNormals[4] = Vector3f(values[4]);
    analyticalNormals[5] = Vector3f(values[5]);
    analyticalNormals[6] = Vector3f(values[6]);
    analyticalNormals[7] = Vector3f(values[7]);
#endif

    m_IsoSurface->AddTriangles(corners, values, analyticalNormals);

    //const Vector3f rootPosition = m_OctreeRoot->GetData()->Position;
    //const onyxF32 halfExtents = m_OctreeRoot->GetData()->HalfExtent;
    //Vector3f from = rootPosition - Vector3f(halfExtents, halfExtents, halfExtents);
    //Vector3f to = rootPosition + Vector3f(halfExtents, halfExtents, halfExtents);

    // disable marching squares skirts
    //if (corners[0][2] == from[2]/* && corners[0].z != mTotalFrom.z*/)
    //{
    //    m_MarchingSquaresIsoSurface->AddMarchingSquaresTriangles(corners, values, IsoSurface<onyxF32>::MS_CORNERS_BACK);
    //}
    //if (corners[2][2] == to[2]/* && corners[2].z != mTotalTo.z*/)
    //{
    //    m_MarchingSquaresIsoSurface->AddMarchingSquaresTriangles(corners, values, IsoSurface<onyxF32>::MS_CORNERS_FRONT);
    //}
    //if (corners[0][0] == from[0]/* && corners[0].x != mTotalFrom.x*/)
    //{
    //    m_MarchingSquaresIsoSurface->AddMarchingSquaresTriangles(corners, values, IsoSurface<onyxF32>::MS_CORNERS_LEFT);
    //}
    //if (corners[1][0] == to[0]/* && corners[1].x != mTotalTo.x*/)
    //{
    //    m_MarchingSquaresIsoSurface->AddMarchingSquaresTriangles(corners, values, IsoSurface<onyxF32>::MS_CORNERS_RIGHT);
    //}
    //if (corners[5][1] == to[1]/* && corners[5].y != mTotalTo.y*/)
    //{
    //    m_MarchingSquaresIsoSurface->AddMarchingSquaresTriangles(corners, values, IsoSurface<onyxF32>::MS_CORNERS_TOP);
    //}
    //if (corners[0][1] == from[1]/* && corners[0].y != mTotalFrom.y*/)
    //{
    //    m_MarchingSquaresIsoSurface->AddMarchingSquaresTriangles(corners, values, IsoSurface<onyxF32>::MS_CORNERS_BOTTOM);
    //}
}

}
