
struct IsoSurfaceRequest
{
    uint64_t Morton;
    uint Depth;
    uint ChunkIndex;
};

layout(std430, buffer_reference, buffer_reference_align = 8) buffer IsoSurfaceRequests
{
    uint Count;
    IsoSurfaceRequest Requests[]; // requests for mesh extraction
};