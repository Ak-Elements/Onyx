implementing lighting;

public struct AxisAlignedBoundingBox {
    public float4 Min;
    public float4 Max;
};

public struct LightCluster {
    public AxisAlignedBoundingBox Cells[ ];
};
