#pragma once

namespace onyx::volume
{
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