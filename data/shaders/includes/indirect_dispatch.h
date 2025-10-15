struct VkIndirectDispatch
{
    uint X;
    uint Y;
    uint Z;
};

layout(std430, buffer_reference, buffer_reference_align = 8) buffer IndirectDispatchBuffer
{
   VkIndirectDispatch Dispatch;
};
