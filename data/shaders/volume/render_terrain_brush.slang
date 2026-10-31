import includes.math.constants;
import includes.math.matrix;
import includes.viewconstants;
import includes.bindless;

import includes.volume.csg.sphere;

static const uint32_t COLOR_32_RED_CHANNEL = 0;
static const uint32_t COLOR_32_GREEN_CHANNEL = 8;
static const uint32_t COLOR_32_BLUE_CHANNEL = 16;
static const uint32_t COLOR_32_ALPHA_CHANNEL = 24;

static const float WIRE_THICKNESS = 0.02f;
static const uint32_t GRID_SIZE = 8;
static const float EPSILON = 0.001f;

struct VSOutput {
    float4 Position : SV_Position;
};

[ shader( "vertex" ) ]
VSOutput vertexMain( uint vertexID: SV_VertexID ) {
    VSOutput output;

    float2 texCoord = float2( ( vertexID << 1 ) & 2, vertexID & 2 );

    output.Position = float4( texCoord * 2.0 - 1.0, 1.0, 1.0 );
    output.Position.y = -output.Position.y;

    return output;
}

struct RayHit {
    float3 Position;
    uint HasHit;
};

struct PushConstants {
    Ptr< RayHit > HitResult;
    Ptr< ViewConstants > ViewConstants;

    float3 BrushSize;
    uint32_t DepthTexture;

    uint32_t BrushType;
    uint32_t BrushOperation;
};

[ push_constant ]
PushConstants Constants;

float4 getColor( uint color ) {
    float s = 1.0f / 255.0f;
    return float4( ( ( color >> COLOR_32_RED_CHANNEL ) & 0xff ) * s,
                   ( ( color >> COLOR_32_GREEN_CHANNEL ) & 0xff ) * s,
                   ( ( color >> COLOR_32_BLUE_CHANNEL ) & 0xff ) * s,
                   ( ( color >> COLOR_32_ALPHA_CHANNEL ) & 0xff ) * s );
}

bool rayIntersect( in float3 rayOrigin, in float3 rayDirection, in CsgSphere sphere, out float2 hitT ) {
    float3 originToCenter = rayOrigin - sphere.Position;
    float b = dot( originToCenter, rayDirection );
    float3 qc = originToCenter - b * rayDirection;
    float h = sphere.Radius * sphere.Radius - dot( qc, qc );
    if( h < 0.0 ) {
        hitT = -1.0f;
        return false; // no intersection
    }

    h = sqrt( h );
    hitT = float2( -b - h, -b + h );
    return hitT.y >= 0.0f;
}

bool intersect( in float3 rayOrigin,
                in float3 rayDirection,
                in CsgSphere sphere,
                out float hitT,
                out float3 hitNormal ) {
    hitT = -1.0f;
    hitNormal = float3( 0.0f );

    float2 hits;
    if( !rayIntersect( rayOrigin, rayDirection, sphere, hits ) )
        return false;

    // Entry hit, or exit hit if the camera is inside the sphere
    hitT = ( hits.x >= 0.0f ) ? hits.x : hits.y;

    if( ( hitT < Constants.ViewConstants.Near ) || ( hitT > Constants.ViewConstants.Far ) )
        return false;

    float3 hitPos = rayOrigin + rayDirection * hitT;
    hitNormal = normalize( hitPos - sphere.Position );
    return true;
}

[ shader( "fragment" ) ]
float4 fragmentMain( VSOutput input ) {
    if( Constants.HitResult.HasHit == 0 ) {
        discard;
        return float4( 0.0f );
    }

    float2 ndc = float2( ( input.Position.x / Constants.ViewConstants.Viewport.x ) * 2.0f - 1.0f,
                         ( input.Position.y / Constants.ViewConstants.Viewport.y ) * -2.0f + 1.0f );

    float4 clipPos = float4( ndc, 1.0f, 1.0f );
    float4 ray = Constants.ViewConstants.InverseProjectionMatrix * clipPos;
    ray /= ray.w;
    ray = Constants.ViewConstants.InverseViewMatrix * ray;

    float3 rayOrigin = Constants.ViewConstants.CameraPosition;
    float3 rayDirection = ray.xyz - rayOrigin;

    rayDirection = normalize( rayDirection );

    CsgSphere sphereBrush = { Constants.HitResult.Position, Constants.BrushSize.x };

    float hitT;
    float3 hitNormal;
    if( !intersect( rayOrigin, rayDirection, sphereBrush, hitT, hitNormal ) ) {
        discard;
        return float4( 0.0f );
    }

    // Depth test against the scene
    float3 brushHitPosition = rayOrigin + rayDirection * hitT;
    float4 clip = Constants.ViewConstants.ViewProjectionMatrix * float4( brushHitPosition, 1.0f );
    float ndcDepth = clip.z / clip.w;
    float sceneDepth = fetchTexel( Constants.DepthTexture, int3( int2( input.Position.xy ), 0 ) ).x;

    if( ndcDepth < sceneDepth ) {
        discard;
        return float4( 0.0f );
    }

    // Simple facing-ratio shading so the sphere reads as a solid
    float facing = saturate( dot( hitNormal, -rayDirection ) );
    float4 shapeColor = getColor( 0x50FFFFFF );
    shapeColor.rgb *= lerp( 0.4f, 1.0f, facing );
    return shapeColor;
}
