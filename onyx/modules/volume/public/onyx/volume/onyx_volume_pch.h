#pragma once

#include <onyx/onyx.h>

namespace Onyx::Volume
{
    // TODO: Move out of onyx-core
    template <typename KeyType>
    class OctreeKey;

    template <typename DataT, typename KeyType>
    class Octree;

    template <typename T>
    class OctreeNode;

    template <typename T>
    class Dualgrid;

    struct VolumeDataContainer;
    using VolumeData = UniquePtr<VolumeDataContainer>;
    using VolumeChunkOctree = Octree<VolumeData, onyxU32>;
    using VolumeChunkDualgrid = Dualgrid<OctreeNode<VolumeData>>;

    enum class VolumeOctreeNodeMetaData : onyxU8
    {
        None = 0,
        IsIsoNear,
        BorderFront,
        BorderBack,
        BorderLeft,
        BorderRight,
        BorderTop,
        BorderBottom
    };

    ONYX_ENABLE_BITMASK_OPERATORS(VolumeOctreeNodeMetaData);
}