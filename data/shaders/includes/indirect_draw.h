struct VkDrawIndirectCommand
{
    uint VertexCount;
    uint InstanceCount;
    uint FirstVertex;
    uint FirstInstance;
};

layout(std430, buffer_reference, buffer_reference_align = 8) buffer IndirectDrawBuffer
{
   VkDrawIndirectCommand Draw;
};
