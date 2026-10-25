import includes.volume.terrainsample;
import includes.volume.volumesource;

import includes.volume.csg.cube;
import includes.volume.csg.ellipsoid;
import includes.volume.csg.sphere;
import includes.volume.csg.plane;

import includes.volume.csg.operations.difference;
import includes.volume.csg.operations.intersect;
import includes.volume.csg.operations.union;

import includes.math.functions;

static const float TextureScale = 1.0f / 200.0f;
static const uint32_t MaxSamples = 512;

// forward declare - defined in default_terrain.h or a custom volumegraph
public extern SdfSample sampleBaseTerrainValue( float3 worldPosition,
                                                uint32_t textureIndex,
                                                float heightScale,
                                                float textureScale );

// // TODO: Move out of this
// float4 raiseLowerBrush( float3 worldPosition, float3 brushCenter, float radius, float strength ) {
//     float3 direction = normalize( worldPosition - brushCenter );
//     float distance = length( worldPosition - brushCenter );
//     float falloff = 1 - smoothstep( 0.0, radius, distance );
//     float delta = -strength * falloff;
//     return float4( direction * delta, delta );
// }

void sampleVolumeSources( float3 worldPosition,
                          inout SdfSample terrainSample,
                          in Ptr< VolumeSourceList > sources,
                          in Ptr< VolumeSourcesData > sourcesData ) {
    float4 terrainSampleVec4 = float4( terrainSample.Gradient, terrainSample.Distance );
    for( uint32_t i = 0; i < sources.Count; ++i ) {
        VolumeSource source = sources.Sources[ i ];
        float4 sourceGradient = float4( 0.0 );
        uint32_t dataStartIndex = source.Index * VolumePackedDataSize;

        float strength = -1.0f;
        float smoothness = -1.0f;

        switch( source.Type ) {
        case VolumeSourceType::Sphere: {
            CsgSphere sphere = CsgSphere( sourcesData, dataStartIndex );
            sourceGradient = sphere.getValueAndGradient( worldPosition );
            break;
        }
        case VolumeSourceType::Cube: {
            CsgCube cube = CsgCube( sourcesData, dataStartIndex );
            sourceGradient = cube.getValueAndGradient( worldPosition );
            break;
        }
        case VolumeSourceType::Ellipsoid: {
            CsgEllipsoid ellipsoid = CsgEllipsoid( sourcesData, dataStartIndex );
            sourceGradient = ellipsoid.getValueAndGradient( worldPosition );
            break;
        }
        case VolumeSourceType::Plane: {
            break;
        }
        case VolumeSourceType::Grid: {
            break;
        }
        }

        switch( source.Operation ) {
        case VolumeOperation::Union: {
            if( smoothness < 0.0f )
                terrainSample = getUnion( sourceGradient, terrainSampleVec4 );
            else
                terrainSample = getUnionSmooth( sourceGradient, terrainSampleVec4, smoothness );
            break;
        }
        case VolumeOperation::Difference: {
            if( smoothness < 0.0f ) {
                terrainSample = getDifference( sourceGradient, terrainSampleVec4 );
            } else {
                terrainSample = getUnionSmooth( sourceGradient, -terrainSampleVec4, smoothness );
                terrainSample.Gradient = -terrainSample.Gradient;
                terrainSample.Distance = -terrainSample.Distance;
            }
            break;
        }
        case VolumeOperation::Intersect: {
            terrainSample = getIntersection( sourceGradient, terrainSampleVec4 );
            break;
        }
        }
    }
}

float getHeightScale( float distance, float2 heightDisplacmentRange, float maxHeightDisplacment ) {
    float t = clamp( ( distance - heightDisplacmentRange.x ) / ( heightDisplacmentRange.y - heightDisplacmentRange.x ),
                     0.0f,
                     1.0f );
    return lerp( maxHeightDisplacment, 0.0f, t );
}

public SdfSample sampleTerrain( float3 worldPosition,
                                uint32_t textureIndex,
                                float heightScale,
                                float textureScale,
                                in Ptr< VolumeSourceList > sources,
                                in Ptr< VolumeSourcesData > sourcesData ) {
    SdfSample sample = sampleBaseTerrainValue( worldPosition, textureIndex, heightScale, textureScale );
    sampleVolumeSources( worldPosition, sample, sources, sourcesData );
    return sample;
}

public Optional< TerrainHit > raymarchTerrain( float3 rayOrigin,
                                               float3 rayDirection,
                                               float3 rayDirectionDdx,
                                               float3 rayDirectionDdy,
                                               float maxDistance,
                                               float maxHeightDisplacment,
                                               float2 heightDisplacmentRange,
                                               uint32_t heightTextureIndex,
                                               float textureScale,
                                               in Ptr< VolumeSourceList > sources,
                                               in Ptr< VolumeSourcesData > sourcesData ) {
    float distance = 0.0f;
    for( uint i = 0; i < MaxSamples; ++i ) {
        float3 position = rayOrigin + rayDirection * distance;
        float heightScale = getHeightScale( distance, heightDisplacmentRange, maxHeightDisplacment );
        float epsilon = distance * 0.0001f;
        SdfSample sample = sampleTerrain( position,
                                          heightTextureIndex,
                                          heightScale,
                                          textureScale,
                                          sources,
                                          sourcesData );

        float scaleDistance = abs( sample.Distance ) > ( heightScale ) || ( heightScale <= 0.0f ) ? 1.0f : 0.5f;

        if( abs( sample.Distance ) <= epsilon ) {
            TerrainHit hit;
            hit.Position = position;
            hit.Iteration = i;
            hit.Normal = sample.Gradient;
            hit.Direction = rayOrigin - position;
            hit.Distance = distance;
            float d = dot( rayDirection, hit.Normal );
            hit.PositionDdx = distance *
                              ( ( rayDirectionDdx * d * safeReciprocal( dot( rayDirectionDdx, hit.Normal ) ) ) -
                                rayDirection );
            hit.PositionDdy = distance *
                              ( ( rayDirectionDdy * d * safeReciprocal( dot( rayDirectionDdy, hit.Normal ) ) ) -
                                rayDirection );
            return hit;
        }

        distance += sample.Distance * scaleDistance;
        if( distance > maxDistance )
            break;
    }

    return none;
}

public Optional< TerrainHit > raymarchTerrain( float3 rayOrigin,
                                               float3 rayDirection,
                                               float maxDistance,
                                               in Ptr< VolumeSourceList > sources,
                                               in Ptr< VolumeSourcesData > sourcesData ) {
    float distance = 0.0f;
    for( uint i = 0; i < MaxSamples; ++i ) {
        float3 position = rayOrigin + rayDirection * distance;
        float epsilon = distance * 0.0001f;
        SdfSample sample = sampleTerrain( position, 0, 0.0f, 1.0f, sources, sourcesData );

        if( abs( sample.Distance ) <= epsilon ) {
            TerrainHit hit;
            hit.Position = position;
            hit.Iteration = i;
            hit.Normal = sample.Gradient;
            hit.Direction = rayOrigin - position;
            hit.Distance = distance;
            return hit;
        }

        distance += sample.Distance;
        if( distance > maxDistance )
            break;
    }

    return none;
}
