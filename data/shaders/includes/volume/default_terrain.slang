import includes.volume.terrainsample;
import includes.volume.volumesource;
import includes.volume.csg.cube;
import includes.volume.csg.sphere;
import includes.volume.csg.plane;
import includes.volume.csg.operations.union;

import includes.triplanar;
import includes.math.constants;

float4 displaceIfNear( float4 valueAndGradient, float3 worldPosition, float heightScale, uint32_t heightTextureId ) {
    if( heightScale <= 0.0f )
        return valueAndGradient;

    float margin = heightScale * 1.0f;
    if( abs( valueAndGradient.w ) > margin )
        return valueAndGradient;

    float h = sampleTriplanar( heightTextureId, worldPosition, valueAndGradient.xyz ).r;
    valueAndGradient.w -= h * heightScale;
    return valueAndGradient;
}

public export SdfSample sampleBaseTerrainValue( float3 worldPosition,
                                                uint32_t textureIndex,
                                                float heightScale,
                                                float textureScale ) {
    float3 textureUvw = worldPosition * textureScale;
    CsgPlane plane = CsgPlane( float3( 0.0f, 1.0f, 0.0f ), float( 0.0f ) );
    float4 planeSample = plane.getValueAndGradient( worldPosition );
    planeSample = displaceIfNear( planeSample, textureUvw, heightScale, textureIndex );

    CsgSphere sphere = CsgSphere( float3( 0, 0, -1000 ), 500 );
    //
    float4 sphereSample = sphere.getValueAndGradient( worldPosition );
    sphereSample = displaceIfNear( sphereSample, textureUvw, heightScale, textureIndex );

    //
    SdfSample sample;
    if( sphereSample.w < planeSample.w ) {
        sample.Gradient = sphereSample.xyz;
        sample.Distance = sphereSample.w;
        return sample;
    }

    sample.Gradient = planeSample.xyz;
    sample.Distance = planeSample.w;
    return sample;
}
