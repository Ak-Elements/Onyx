#pragma once

#include <onyx/volume/octree/octreenode.h>

namespace Onyx::Volume
{
    class VolumeBase;
    struct VolumeDataContainer;

// remove this template argument
template <typename Scalar>
class OctreeSplitPolicy
{
public:
    OctreeSplitPolicy(onyxU8 maxOctreeLevel, Scalar octreeRootSize, const VolumeBase& volumeBase)
        : m_OctreeRootSize(octreeRootSize)
        , m_MaxOctreeLevel(maxOctreeLevel)
        , m_VolumeSource(&volumeBase)
    {
    }
    virtual ~OctreeSplitPolicy() = default;

    virtual bool ShouldSplit(OctreeNode<UniquePtr<VolumeDataContainer>>& node, const Vector3<Scalar>& nodeWorldPosition, Scalar halfExtent, onyxU8 nodeLevel) = 0;

protected:
    const Scalar m_OctreeRootSize;
    const onyxU8 m_MaxOctreeLevel;
    const VolumeBase* m_VolumeSource;
};

}
