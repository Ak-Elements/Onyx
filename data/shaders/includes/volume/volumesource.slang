enum VolumeSourceType : uint8_t {
    Sphere,
    Cube,
    Ellipsoid,
    Plane,
    Grid,
};

enum VolumeOperation : uint8_t { None, Union, Difference, Intersect };

// static const uint32_t VolumeSource_SphereBrush = 5;
// static const uint32_t VolumeSource_CubeBrush = 6;
static const uint32_t VolumePackedDataSize = 8;

struct VolumeSource {
    VolumeSourceType Type;
    VolumeOperation Operation;
    uint32_t Index;
};

// VolumeSourcesList defines each active volume in the area
// each volume type has a certain amount of data which is allocated inside the
// VolumeSourceDataList and retrieved directly from the buffer
struct VolumeSourceList {
    uint32_t Count;
    VolumeSource Sources[ ];
}

struct VolumeSourcesData {
    float Data[ ];
}
