implementing lighting;

public struct LightEnvironment {
    Ptr< DirectionalLights > DirectionalLights;
    Ptr< PointLights > PointLights;
    Ptr< SpotLights > SpotLights;
    Ptr< LightGrid > Grid;
    Ptr< LightIndices > LightIndices;

    uint3 LightClusterGridSize;
    float LightClusterScale;

    uint2 LightClusterSize;
    float LightClusterBias;
};

