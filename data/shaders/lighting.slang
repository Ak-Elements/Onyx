module lighting;

__include "lighting/constants";

__include "lighting/directionallight";
__include "lighting/pointlight";
__include "lighting/spotlight";

__include "lighting/lightenvironment";
__include "lighting/lightgrid";
__include "lighting/lightindices";
__include "lighting/lightcluster";

__include "lighting/brdf";
__include "lighting/pbrmaterial";

public float3 lightContribution( in LightEnvironment environment,
                                 in ViewConstants viewConstants,
                                 float3 worldPosition,
                                 float3 worldNormal,
                                 float4 screenPosition,
                                 float3 cameraPosition,
                                 PbrMaterial material ) {
    uint32_t clusterIndex = getClusterIndex( environment, viewConstants, screenPosition );

    float3 N = normalize( worldNormal );
    float3 V = normalize( cameraPosition - worldPosition );

    float3 Lo = lightContribution( *environment.DirectionalLights, V, N, material );
    Lo += lightContribution( *environment.PointLights, environment, clusterIndex, V, N, worldPosition, material );
    Lo += lightContribution( *environment.SpotLights, environment, clusterIndex, V, N, worldPosition, material );
    return Lo;
}
