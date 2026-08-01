implementing lighting;

struct PointLight {
    float3 Position;
    float Intensity;

    float3 Color;
    float Radius;

    uint32_t IsEnabled;
    uint32_t IsShadowCasting;
    float Pad[ 6 ]; // unused
};

public struct PointLights {
    PointLight Lights[ MaxLights ];
    uint32_t Count;
}

float3 lightContribution( in PointLight light, float3 V, float3 N, float3 worldPosition, PbrMaterial material ) {
    float3 lightToFragPos = light.Position - worldPosition;

    float3 L = normalize( lightToFragPos );

    float distance = length( lightToFragPos );

    float attenuation = clamp( 1.0 - ( distance * distance ) / ( light.Radius * light.Radius ), 0.0, 1.0 );
    attenuation *= lerp( attenuation, 1.0, 0.5 );

    float3 radiance = light.Color * light.Intensity * attenuation;
    return brdf( L, V, N, radiance, material.Albedo, material.Metalness, material.Roughness );
}

public float3 lightContribution( in PointLights lights,
                                 in LightEnvironment environment,
                                 uint32_t clusterIndex,
                                 float3 V,
                                 float3 N,
                                 float3 worldPosition,
                                 PbrMaterial material ) {
    float3 Lo = float3( 0.0 );

    uint32_t pointLightCount = environment.Grid->Cells[ clusterIndex ].PointLightCount;
    uint32_t lightIndexOffset = environment.Grid->Cells[ clusterIndex ].PointLightOffset;
    for( uint i = 0; i < pointLightCount; ++i ) {
        uint32_t lightIndex = environment.LightIndices->Indices[ lightIndexOffset + i ].PointLightIndex;
        PointLight light = lights.Lights[ lightIndex ];
        Lo += lightContribution( light, V, N, worldPosition, material );
    }

    return Lo;
}

public float3 lightContribution( in PointLights lights,
                                 float3 V,
                                 float3 N,
                                 float3 worldPosition,
                                 PbrMaterial material ) {
    float3 Lo = float3( 0.0 );

    uint32_t pointLightCount = lights.Count;
    for( uint i = 0; i < pointLightCount; ++i ) {
        PointLight light = lights.Lights[ i ];
        Lo += lightContribution( light, V, N, worldPosition, material );
    }

    return Lo;
}
