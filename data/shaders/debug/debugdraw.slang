import includes.math.constants;
import includes.math.matrix;
import includes.math.functions;
import includes.viewconstants;

struct VSOutput {
    float4 Position : SV_Position;
};

[ shader( "vertex" ) ]
VSOutput vertexMain( uint vertexID: SV_VertexID ) {
    VSOutput output;

    float2 texCoord = float2( ( vertexID << 1 ) & 2, vertexID & 2 );

    output.Position = float4( texCoord * 2.0 - 1.0, 0.0, 1.0 );
    output.Position.y = -output.Position.y;

    return output;
}

static const uint32_t COLOR_32_RED_CHANNEL = 0;
static const uint32_t COLOR_32_GREEN_CHANNEL = 8;
static const uint32_t COLOR_32_BLUE_CHANNEL = 16;
static const uint32_t COLOR_32_ALPHA_CHANNEL = 24;

static const float WIRE_THICKNESS = 0.02f;
static const uint32_t GRID_SIZE = 8;
static const float EPSILON = 0.001f;

struct DebugSphere {
    float3 Position;
    float Radius;

    uint Color;
};

struct DebugBox {
    float3 Position;
    uint Color;

    float3x3 Rotation;
    float3 HalfExtents;
};

struct DebugCapsule {
    float3 Position;
    uint Color;

    float3x3 Rotation;
    float Radius;
    float HalfHeight;
};

struct SphereList {
    DebugSphere[ ] Spheres;
};

struct BoxList {
    DebugBox[ ] Boxes;
};

struct CapsuleList {
    DebugCapsule[ ] Capsules;
};

struct PushConstants {
    Ptr< ViewConstants > ViewConstantsBuffer;

    Ptr< BoxList > WireframeBoxes;
    Ptr< CapsuleList > WireframeCapsules;
    Ptr< SphereList > WireframeSpheres;

    uint32_t WireframeBoxesCount;
    uint32_t WireframeCapsulesCount;
    uint32_t WireframeSpheresCount;
};

[ push_constant ]
PushConstants Constants;

bool rayIntersect( in float3 rayOrigin, in float3 rayDirection, in DebugSphere sphere, out float2 hitT ) {
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

float2 rayIntersect( in float3 rayOrigin,
                     in float3 rayDirection,
                     in DebugBox box,
                     out float3 hitNormalEntry,
                     out float3 hitNormalExit ) {
    float3 m = 1.0 / rayDirection;
    float3 n = m * rayOrigin;
    float3 k = abs( m ) * box.HalfExtents;
    float3 t1 = -n - k;
    float3 t2 = -n + k;

    float tN = max( max( t1.x, t1.y ), t1.z );
    float tF = min( min( t2.x, t2.y ), t2.z );
    if( tN > tF || tF < 0.0 ) {
        hitNormalEntry = float3( -1.0f );
        hitNormalExit = float3( -1.0f );
        return float2( -1.0f ); // no intersection
    }

    float3 raySign = -sign( rayDirection );
    hitNormalEntry = step( float3( tN ), t1 ) * raySign;
    hitNormalExit = step( t2, float3( tF ) ) * raySign;

    return float2( tN, tF );
}

float3 capsuleCapNormal( in float3 pos, in float3 a, in float3 b, in float r ) {
    float3 ba = b - a;
    float3 pa = pos - a;
    float h = clamp( dot( pa, ba ) / dot( ba, ba ), 0.0, 1.0 );
    return ( pa - h * ba ) / r;
}

bool rayIntersect( in float3 rayOrigin,
                   in float3 rayDirection,
                   in DebugCapsule capsule,
                   out float2 hitT,
                   out float3 hitNormalEntry,
                   out float3 hitNormalExit ) {
    float3 pa = float3( 0.0f, -capsule.HalfHeight, 0.0f );
    float3 pb = float3( 0.0f, capsule.HalfHeight, 0.0f );
    float r = capsule.Radius;

    float3 ba = rayOrigin - rayOrigin;
    ba = pb - pa;
    float3 oa = rayOrigin - pa;
    float baba = dot( ba, ba );
    float bard = dot( ba, rayDirection );
    float baoa = dot( ba, oa );
    float rdoa = dot( rayDirection, oa );
    float oaoa = dot( oa, oa );
    float a = baba - bard * bard;
    float b = baba * rdoa - baoa * bard;
    float c = baba * oaoa - baoa * baoa - r * r * baba;
    float h = b * b - a * c;

    hitT = float2( -1.0f );
    hitNormalEntry = float3( 0.0f );
    hitNormalExit = float3( 0.0f );

    if( h < 0.0f )
        return false;

    float sh = sqrt( h );

    float tNear = -1.0f;
    {
        float t = ( -b - sh ) / a;
        float y = baoa + t * bard;
        if( y > 0.0f && y < baba ) {
            tNear = t;
        } else {
            float3 oc = ( y <= 0.0f ) ? oa : rayOrigin - pb;
            float bb = dot( rayDirection, oc );
            float cc = dot( oc, oc ) - r * r;
            float hh = bb * bb - cc;
            if( hh > 0.0f )
                tNear = -bb - sqrt( hh );
        }
    }

    float tFar = -1.0f;
    {
        float t = ( -b + sh ) / a;
        float y = baoa + t * bard;
        if( y > 0.0f && y < baba ) {
            tFar = t;
        } else {
            float3 oc = ( y <= 0.0f ) ? oa : rayOrigin - pb;
            float bb = dot( rayDirection, oc );
            float cc = dot( oc, oc ) - r * r;
            float hh = bb * bb - cc;
            if( hh > 0.0f )
                tFar = -bb + sqrt( hh );
        }
    }

    if( tNear < 0.0f && tFar < 0.0f )
        return false;

    hitT = float2( tNear, tFar );

    if( tNear >= 0.0f )
        hitNormalEntry = capsuleCapNormal( rayOrigin + rayDirection * tNear, pa, pb, r );
    if( tFar >= 0.0f )
        hitNormalExit = capsuleCapNormal( rayOrigin + rayDirection * tFar, pa, pb, r );

    return hitT.y >= 0.0f;
}

float getWirteThicknessSilhouette( float3 hitNormal, float3 rayDirection ) {
    return smoothstep( WIRE_THICKNESS * 2.0f, 0.0, abs( dot( hitNormal, rayDirection ) ) );
}

float getWireThickness( in DebugSphere sphere, in float3 rayOrigin, in float3 rayDirection, in float hitT ) {
    float3 hitPos = rayOrigin + rayDirection * hitT;
    float3 hitNormal = normalize( hitPos - sphere.Position );

    float phi = atan2( hitNormal.z, hitNormal.x );
    float theta = asin( clamp( hitNormal.y, -1.0f, 1.0f ) );

    float latitude = ( phi / ( TwoPi ) ) * GRID_SIZE;
    float longitude = ( ( theta / Pi ) + 0.5f ) * GRID_SIZE;

    float latitudeWeight = abs( fract( latitude + 0.5 ) - 0.5f );
    float longitudeWeight = abs( fract( longitude + 0.5 ) - 0.5f );
    float wireThickness = smoothstep( WIRE_THICKNESS, 0.0f, min( latitudeWeight, longitudeWeight ) );
    return max( getWirteThicknessSilhouette( hitNormal, rayDirection ), wireThickness );
}

float getWireThickness( in DebugBox box,
                        in float3 rayOrigin,
                        in float3 rayDirection,
                        in float hitT,
                        in float3 hitNormal ) {
    float3 hitPos = rayOrigin + rayDirection * hitT;
    float3 uv = hitPos / box.HalfExtents;

    float2 faceUV;
    if( abs( hitNormal.x ) > 0.5f ) {
        faceUV = uv.yz;
    } else if( abs( hitNormal.y ) > 0.5f ) {
        faceUV = uv.xz;
    } else {
        faceUV = uv.xy;
    }

    float2 edgeDistance = abs( abs( faceUV ) - 1.0f );
    float edge = min( edgeDistance.x, edgeDistance.y );

    float wireThickness = smoothstep( WIRE_THICKNESS, 0.0f, edge );
    return max( getWirteThicknessSilhouette( hitNormal, rayDirection ), wireThickness );
}

float getWireThickness( in DebugCapsule capsule,
                        in float3 rayOrigin,
                        in float3 rayDirection,
                        in float hitT,
                        in float3 hitNormal ) {
    float3 hitPos = rayOrigin + rayDirection * hitT;
    float wireThickness;

    if( abs( hitPos.y ) <= capsule.HalfHeight + EPSILON ) {
        float phi = atan2( hitNormal.z, hitNormal.x );
        float longitude = ( phi / ( TwoPi ) ) * GRID_SIZE;
        float longitudeWeight = abs( fract( longitude + 0.5f ) - 0.5f );

        wireThickness = smoothstep( WIRE_THICKNESS, 0.0f, longitudeWeight );
    } else {
        float phi = atan2( hitNormal.z, hitNormal.x );
        float theta = asin( clamp( hitNormal.y, -1.0f, 1.0f ) );

        float latitude = ( phi / ( TwoPi ) ) * GRID_SIZE;
        float longitude = ( ( theta / Pi ) + 0.5f ) * GRID_SIZE;

        float latitudeWeight = abs( fract( latitude + 0.5f ) - 0.5f );
        float longitudeWeight = abs( fract( longitude + 0.5f ) - 0.5f );
        wireThickness = smoothstep( WIRE_THICKNESS, 0.0f, min( latitudeWeight, longitudeWeight ) );
    }

    return max( getWirteThicknessSilhouette( hitNormal, rayDirection ), wireThickness );
}

float4 getColor( uint color ) {
    float s = 1.0f / 255.0f;
    return float4( ( ( color >> COLOR_32_RED_CHANNEL ) & 0xff ) * s,
                   ( ( color >> COLOR_32_GREEN_CHANNEL ) & 0xff ) * s,
                   ( ( color >> COLOR_32_BLUE_CHANNEL ) & 0xff ) * s,
                   ( ( color >> COLOR_32_ALPHA_CHANNEL ) & 0xff ) * s );
}

bool intersect( in float3 rayOrigin,
                in float3 rayDirection,
                in DebugSphere sphere,
                inout float closestT,
                inout float4 finalColor ) {
    float2 hits;
    bool hasHit = rayIntersect( rayOrigin, rayDirection, sphere, hits );
    if( hasHit == false )
        return false;

    float wireThickness = 0.0f;
    float hitT = -1.0f;

    if( hits.x >= 0.0f ) {
        // check entry hit
        hitT = hits.x;
        wireThickness = getWireThickness( sphere, rayOrigin, rayDirection, hitT );
        if( wireThickness < EPSILON ) {
            // check exit hit
            hitT = hits.y;
            wireThickness = getWireThickness( sphere, rayOrigin, rayDirection, hitT );
        }
    } else {
        // check exit hit
        hitT = hits.y;
        wireThickness = getWireThickness( sphere, rayOrigin, rayDirection, hitT );
    }

    if( ( hitT < Constants.ViewConstantsBuffer->Near ) || ( hitT > Constants.ViewConstantsBuffer->Far ) ) {
        return false;
    }

    if( ( hitT < closestT ) && wireThickness > EPSILON ) {
        closestT = hitT;
        float4 shapeColor = getColor( sphere.Color );
        finalColor = lerp( finalColor, shapeColor, wireThickness );
        return true;
    }

    return false;
}

bool intersect( in float3 rayOrigin,
                in float3 rayDirection,
                in DebugBox box,
                inout float closestT,
                inout float4 finalColor ) {
    float3x3 worldToBoxSpace = transpose( box.Rotation );

    float3 rayOriginBoxSpace = worldToBoxSpace * ( rayOrigin - box.Position );
    float3 rayDirectionBoxSpace = worldToBoxSpace * rayDirection;

    float3 hitNormalBoxSpaceEntry;
    float3 hitNormalBoxSpaceExit;
    float2 hits = rayIntersect( rayOriginBoxSpace,
                                rayDirectionBoxSpace,
                                box,
                                hitNormalBoxSpaceEntry,
                                hitNormalBoxSpaceExit );
    if( hits.y < 0.0f ) {
        return false;
    }

    float wireThickness = 0.0f;
    float hitT = -1.0f;

    if( hits.x >= 0.0f ) {
        // check entry hit
        hitT = hits.x;
        wireThickness = getWireThickness( box, rayOriginBoxSpace, rayDirectionBoxSpace, hitT, hitNormalBoxSpaceEntry );
        if( wireThickness < EPSILON ) {
            // check exit hit
            hitT = hits.y;
            wireThickness = getWireThickness( box,
                                              rayOriginBoxSpace,
                                              rayDirectionBoxSpace,
                                              hitT,
                                              hitNormalBoxSpaceExit );
        }
    } else {
        // check exit hit
        hitT = hits.y;
        wireThickness = getWireThickness( box, rayOriginBoxSpace, rayDirectionBoxSpace, hitT, hitNormalBoxSpaceExit );
    }

    if( ( hitT < Constants.ViewConstantsBuffer->Near ) || ( hitT > Constants.ViewConstantsBuffer->Far ) ) {
        return false;
    }

    if( ( hitT < closestT ) && wireThickness > EPSILON ) {
        closestT = hitT;
        float4 shapeColor = getColor( box.Color );
        finalColor = lerp( finalColor, shapeColor, wireThickness );
        return true;
    }

    return false;
}

bool intersect( in float3 rayOrigin,
                in float3 rayDirection,
                in DebugCapsule capsule,
                inout float closestT,
                inout float4 finalColor ) {
    float3x3 worldToLocal = transpose( capsule.Rotation );

    float3 rayOriginLocal = worldToLocal * ( rayOrigin - capsule.Position );
    float3 rayDirectionLocal = worldToLocal * rayDirection;

    float2 hits;
    float3 hitNormalLocalEntry;
    float3 hitNormalLocalExit;
    bool hasHit = rayIntersect( rayOriginLocal,
                                rayDirectionLocal,
                                capsule,
                                hits,
                                hitNormalLocalEntry,
                                hitNormalLocalExit );
    if( hasHit == false )
        return false;

    float wireThickness = 0.0f;
    float hitT = -1.0f;

    if( hits.x >= 0.0f ) {
        hitT = hits.x;
        wireThickness = getWireThickness( capsule, rayOriginLocal, rayDirectionLocal, hitT, hitNormalLocalEntry );
        if( wireThickness < EPSILON ) {
            hitT = hits.y;
            wireThickness = getWireThickness( capsule, rayOriginLocal, rayDirectionLocal, hitT, hitNormalLocalExit );
        }
    } else {
        hitT = hits.y;
        wireThickness = getWireThickness( capsule, rayOriginLocal, rayDirectionLocal, hitT, hitNormalLocalExit );
    }

    if( ( hitT < Constants.ViewConstantsBuffer->Near ) || ( hitT > Constants.ViewConstantsBuffer->Far ) ) {
        return false;
    }

    if( ( hitT < closestT ) && wireThickness > EPSILON ) {
        closestT = hitT;
        float4 shapeColor = getColor( capsule.Color );
        finalColor = lerp( finalColor, shapeColor, wireThickness );
        return true;
    }

    return false;
}

[ shader( "fragment" ) ]
float4 fragmentMain( VSOutput input )
    : SV_Target0 {
    float2 ndc = float2( ( input.Position.x / Constants.ViewConstantsBuffer->Viewport.x ) * 2.0f - 1.0f,
                         ( input.Position.y / Constants.ViewConstantsBuffer->Viewport.y ) * -2.0f + 1.0f );

    float4 clipPos = float4( ndc, 0.0f, 1.0f );
    float4 ray = Constants.ViewConstantsBuffer->InverseProjectionMatrix * clipPos;
    ray /= ray.w;
    ray = Constants.ViewConstantsBuffer->InverseViewMatrix * ray;

    float3 rayOrigin = Constants.ViewConstantsBuffer->CameraPosition;
    float3 rayDirection = normalize( ray.xyz - rayOrigin );

    bool hasHit = false;
    float closestT = 1e9;
    float4 finalColor = float4( 0.0f );

    for( int i = 0; i < Constants.WireframeSpheresCount; ++i ) {
        DebugSphere sphere = Constants.WireframeSpheres->Spheres[ i ];
        bool hit = intersect( rayOrigin, rayDirection, sphere, closestT, finalColor );
        if( hit ) {
            hasHit = true;
        }
    }

    for( int i = 0; i < Constants.WireframeBoxesCount; ++i ) {
        DebugBox box = Constants.WireframeBoxes->Boxes[ i ];
        bool hit = intersect( rayOrigin, rayDirection, box, closestT, finalColor );
        if( hit ) {
            hasHit = true;
        }
    }
    for( int i = 0; i < Constants.WireframeCapsulesCount; ++i ) {
        DebugCapsule capsule = Constants.WireframeCapsules->Capsules[ i ];
        bool hit = intersect( rayOrigin, rayDirection, capsule, closestT, finalColor );
        if( hit ) {
            hasHit = true;
        }
    }

    if( hasHit == false )
        discard;

    return finalColor;
}
