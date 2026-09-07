implementing lighting;

public struct LightEnvironment {
    public Ptr< DirectionalLights > DirectionalLights;
    public Ptr< PointLights > PointLights;
    public Ptr< SpotLights > SpotLights;
    public Ptr< LightGrid > Grid;
    public Ptr< LightIndices > LightIndices;

    public uint2 LightClusterSize;
    public float LightClusterBias;
    public float LightClusterScale;
};

