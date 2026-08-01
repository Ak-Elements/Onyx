implementing lighting;

public struct DirectionalLight {
    public float3 Direction;
    public float Intensity;

    public float3 Color;
    public int32_t ShadowSamples;

    public bool IsShadowCasting;
    float Pad[ 7 ]; // unused
};

public struct DirectionalLights {
    DirectionalLight Lights[ MaxDirectionalLights ];
    int32_t Count;
};

public float3 lightContribution( in DirectionalLights lights, float3 V, float3 N, PbrMaterial material ) {
    float3 Lo = float3( 0.0 );
    for( int i = 0; i < lights.Count; ++i ) {
        DirectionalLight light = lights.Lights[ i ];
        float3 L = normalize( -light.Direction );

        float3 radiance = light.Color * light.Intensity;
        Lo += brdf( L, V, N, radiance, material.Albedo, material.Metalness, material.Roughness );
    }

    return Lo;
}
