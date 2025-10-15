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
        IsIsoNear = 1 << 0,
        BorderFront = 1 << 1,
        BorderBack = 1 << 2,
        BorderLeft = 1 << 3,
        BorderRight = 1 << 4,
        BorderTop = 1 << 5,
        BorderBottom = 1 << 6
    };
}