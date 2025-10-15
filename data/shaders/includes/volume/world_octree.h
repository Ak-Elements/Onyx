#include "includes/volume/octree.h"

layout(std430, buffer_reference, buffer_reference_align = 8) buffer WorldOctree
{
    uint Count;
    uint64_t Nodes[];
};
