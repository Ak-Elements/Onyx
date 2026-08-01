implementing lighting;

struct LightIndex {
    uint PointLightIndex;
    uint SpotLightIndex;
};

public struct LightIndices {
    LightIndex Indices[ ];
};
