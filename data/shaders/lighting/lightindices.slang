implementing lighting;

public struct LightIndex {
    public uint32_t PointLightIndex;
    public uint32_t SpotLightIndex;
};

public struct LightIndices {
    public LightIndex Indices[ ];
};
