const int VolumeSource_Sphere = 0;
const int VolumeSource_Cube = 1;
const int VolumeSource_Ellipsoid = 2;
const int VolumeSource_Plane = 3;
const int VolumeSource_Grid = 4;

const int VolumeSource_SphereBrush = 5;
const int VolumeSource_CubeBrush = 6;

const int VolumeOperation_Union = 0;
const int VolumeOperation_Difference = 1;
const int VolumeOperation_Intersect = 2;

const int VolumeSources_ItemSize = 8;

struct VolumeSource
{
    uint16_t Type;
    uint16_t Operation;
    uint32_t Index;
};

layout(std430, buffer_reference, buffer_reference_align = 8) buffer WorldVolumeSourcesList
{
    uint Count;
    VolumeSource Sources[];
};

layout(std430, buffer_reference, buffer_reference_align = 4) buffer WorldVolumeSources
{
    float SourcesData[];
};