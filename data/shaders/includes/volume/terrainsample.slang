import includes.volume.volumesource;

public struct SdfSample {
    public float3 Gradient;
    public float Distance;

    public VolumeSourceType Primitive0Type;
    public VolumeSourceType Primitive1Type;

    public float Primitive0[ VolumePackedDataSize ];
    public float Primitive1[ VolumePackedDataSize ];

    public VolumeOperation Operation;
};

public struct TerrainHit {
    public float3 Position;
    public float3 PositionDdx;
    public float3 PositionDdy;
    public float Distance;

    public float3 Direction;
    public float3 Normal;
    public float3 Tangent;
    public float3 BiTangent;

    public float2 Uv;
    public float2 UvDdx;
    public float2 UvDdy;
    public int Iteration;
};
