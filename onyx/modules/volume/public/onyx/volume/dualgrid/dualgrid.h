#pragma once

namespace Onyx::Volume
{
class VolumeBase;

template<typename Scalar>
class IsoSurface;

template<typename Scalar>
class MarchingSquaresSurface;

template <typename OctreeNodeT>
class Dualgrid
{
public:
    struct DualCell
    {
        DualCell(const OctreeNodeT& node0, const OctreeNodeT& node1, const OctreeNodeT& node2, const OctreeNodeT& node3, const OctreeNodeT& node4, const OctreeNodeT& node5, const OctreeNodeT& node6, const OctreeNodeT& node7)
            : node0(node0)
            , node1(node1)
            , node2(node2)
            , node3(node3)
            , node4(node4)
            , node5(node5)
            , node6(node6)
            , node7(node7)
        {}

        // Debug operator
        bool operator<(const DualCell& other) const
        {
            std::vector<intptr_t> ptrs = { reinterpret_cast<intptr_t>(&node0), reinterpret_cast<intptr_t>(&node1), reinterpret_cast<intptr_t>(&node2), reinterpret_cast<intptr_t>(&node3), reinterpret_cast<intptr_t>(&node4), reinterpret_cast<intptr_t>(&node5), reinterpret_cast<intptr_t>(&node6), reinterpret_cast<intptr_t>(&node7) };
            std::vector<intptr_t> ptrsOther = { reinterpret_cast<intptr_t>(&other.node0), reinterpret_cast<intptr_t>(&other.node1), reinterpret_cast<intptr_t>(&other.node2), reinterpret_cast<intptr_t>(&other.node3), reinterpret_cast<intptr_t>(&other.node4), reinterpret_cast<intptr_t>(&other.node5), reinterpret_cast<intptr_t>(&other.node6), reinterpret_cast<intptr_t>(&other.node7) };
        
            std::sort(ptrs.begin(), ptrs.end());
            std::sort(ptrsOther.begin(), ptrsOther.end());

            return ptrs < ptrsOther;
        }

        const OctreeNodeT& node0;
        const OctreeNodeT& node1;
        const OctreeNodeT& node2;
        const OctreeNodeT& node3;
        const OctreeNodeT& node4;
        const OctreeNodeT& node5;
        const OctreeNodeT& node6;
        const OctreeNodeT& node7;
    };

    void SetIsoSurface(IsoSurface<onyxF32>* isoSurface) { m_IsoSurface = isoSurface; }
    void SetMarchingSquaresIsoSurface(MarchingSquaresSurface<onyxF32>* isoSurface) { m_MarchingSquaresIsoSurface = isoSurface; }

    void GenerateDualgrid(const OctreeNodeT& root, const Vector3f& rootWorldPosition, const VolumeBase& volumeSource);
//#ifdef DEBUG
    std::vector<DualCell>& GetDualCells() { return m_DualCells; }
//#endif
private:
    void NodeProc(const OctreeNodeT& node);

    void EdgeProcX(const OctreeNodeT& node0, const OctreeNodeT& node1, const OctreeNodeT& node2, const OctreeNodeT& node3);
    void EdgeProcY(const OctreeNodeT& node0, const OctreeNodeT& node1, const OctreeNodeT& node2, const OctreeNodeT& node3);
    void EdgeProcZ(const OctreeNodeT& node0, const OctreeNodeT& node1, const OctreeNodeT& node2, const OctreeNodeT& node3);

    void FaceProcXY(const OctreeNodeT& node0, const OctreeNodeT& node1);
    void FaceProcZY(const OctreeNodeT& node0, const OctreeNodeT& node1);
    void FaceProcXZ(const OctreeNodeT& node0, const OctreeNodeT& node1);

    void VertexProc(const OctreeNodeT& node0, const OctreeNodeT& node1, const OctreeNodeT& node2, const OctreeNodeT& node3, const OctreeNodeT& node4, const OctreeNodeT& node5, const OctreeNodeT& node6, const OctreeNodeT& node7);

    void AddDualCell(const OctreeNodeT& node0, const OctreeNodeT& node1, const OctreeNodeT& node2, const OctreeNodeT& node3, const OctreeNodeT& node4, const OctreeNodeT& node5, const OctreeNodeT& node6, const OctreeNodeT& node7);
    void CreateBorderCell(const OctreeNodeT& node0, const OctreeNodeT& node1, const OctreeNodeT& node2, const OctreeNodeT& node3, const OctreeNodeT& node4, const OctreeNodeT& node5, const OctreeNodeT& node6, const OctreeNodeT& node7);
    void AddBorderDualCell(const Vector3f& position0, const Vector3f& position1, const Vector3f& position2, const Vector3f& position3, const Vector3f& position4, const Vector3f& position5, const Vector3f& position6, const Vector3f& position7);
    //TODO: debug only
//#ifdef DEBUG
    std::vector<DualCell> m_DualCells;
//#endif

    IsoSurface<onyxF32>* m_IsoSurface = nullptr;
    MarchingSquaresSurface<onyxF32>* m_MarchingSquaresIsoSurface = nullptr;

    const VolumeBase* m_Volume = nullptr;
    const OctreeNodeT* m_OctreeRoot = nullptr;
    Vector3f m_OctreeRootWorldPosition;
};

}

#include <onyx/volume/dualgrid/dualgrid.hpp>