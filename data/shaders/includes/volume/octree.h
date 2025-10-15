struct OctreeNode
{
    uint LeafMask; // 8 bits
    uint ValidMask; // 8 bits
    bool IsFar; // 1 bit
    bool IsFarData; // 1 bit
    uint ChildrenOffset; // 23 bits
    uint DataOffset; // 23 bits
};

const uint OffsetBitMask = (1 << 24) - 1;
const uint64_t IsFarMask = 1 << 47ul;
const uint64_t IsFarDataMask = 1 << 46ul;

uint64_t PackOctreeNode(OctreeNode node)
{
    return (uint64_t(node.LeafMask) << 56ul) | (uint64_t(node.ValidMask) << 48ul) |  (node.IsFar ? IsFarMask : 0) | (node.IsFarData ? IsFarDataMask : 0) | uint64_t(node.ChildrenOffset & OffsetBitMask) << 23ul | uint64_t(node.DataOffset & OffsetBitMask);
}

OctreeNode UnpackOctreeNode(uint64_t packedNode)
{
    OctreeNode outNode;
    outNode.LeafMask = uint(packedNode >> 56ul);
    outNode.ValidMask = uint(packedNode >> 48ul) & 255;
    outNode.IsFar = (packedNode & IsFarMask) == 1;
    outNode.IsFarData = (packedNode & IsFarDataMask) == 1;
    outNode.ChildrenOffset = uint(packedNode >> 23ul) & OffsetBitMask;
    outNode.DataOffset = uint(packedNode & OffsetBitMask);
    return outNode;
}