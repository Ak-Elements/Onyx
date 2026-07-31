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

struct PushConstants {
    Ptr< ViewConstants > ViewConstants;

    float Rotation00;
    float Rotation10;
    float Rotation20;

    float Rotation01;
    float Rotation11;
    float Rotation21;

    float Rotation02;
    float Rotation12;
    float Rotation22;

    uint32_t GridCellCount;
    float LodLevel;

    float AxisLineWidth;
    float MajorLineWidth;
    float MinorLineWidth;

    uint32_t MajorLineColor;
    uint32_t MinorLineColor;

    uint32_t X_AxisColor;
    uint32_t Y_AxisColor;
    uint32_t Z_AxisColor;
    uint32_t Flags;
};

[ push_constant ]
PushConstants Constants;

static const float AntiAliasingFactor = 1.5f;
static const uint LockScaleFlag = 1;

float4 toFloat4Color( uint color ) {
    return float4( ( uint8_t( color ) / 255.0f ),
                   ( uint8_t( color >> 8 ) / 255.0f ),
                   ( uint8_t( color >> 16 ) / 255.0f ),
                   ( uint8_t( color >> 24 ) / 255.0f ) );
}

float3 getRayDirection( float2 fragCoord ) {
    float2 ndc = float2( ( fragCoord.x / Constants.ViewConstants->Viewport.x ) * 2.0f - 1.0f,
                         ( fragCoord.y / Constants.ViewConstants->Viewport.y ) * -2.0f + 1.0f );

    float4 clipPos = float4( ndc, 0.0f, 1.0f );
    float4 ray = Constants.ViewConstants->InverseProjectionMatrix * clipPos;
    ray /= ray.w;

    return normalize( ( Constants.ViewConstants->InverseViewMatrix * float4( ray.xyz, 0.0 ) ).xyz );
}

bool intersectPlane( float3 rayOrigin,
                     float3 rayDirection,
                     float3 planeNormal,
                     float planeOffset,
                     out float3 outHitPosition ) {
    // TODO: add offset
    float t = dot( -rayOrigin, planeNormal ) * safeReciprocal( dot( rayDirection, planeNormal ) );
    outHitPosition = rayOrigin + rayDirection * t;
    return t > 0.0f && t <= Constants.ViewConstants.Far;
}

float computeGridLineFactor( float2 uv, float2 uvDerivatives, float lineWidth, float2 offset, float2 axisLines ) {
    bool invertLine = lineWidth > 0.5f;
    float targetWidth = invertLine ? 1.0f - lineWidth : lineWidth;
    float2 drawWidth = clamp( float2( targetWidth ), uvDerivatives, float2( 0.5f ) );
    float2 lineAA = uvDerivatives * AntiAliasingFactor;
    float2 gridUV = 1.0f - abs( fract( uv ) * 2.0f - 1.0f );
    gridUV += offset;

    float2 gridLines = smoothstep( drawWidth + lineAA, drawWidth - lineAA, gridUV );

    gridLines *= clamp( ( targetWidth / drawWidth ), 0.0f, 1.0f );
    gridLines = clamp( gridLines - axisLines, 0.0f, 1.0f );
    gridLines = lerp( gridLines, float2( targetWidth ), clamp( ( uvDerivatives * 2.0f - 1.0f ), 0.0f, 1.0f ) );
    gridLines = invertLine ? 1.0f - gridLines : gridLines;

    return lerp( gridLines.x, 1.0f, gridLines.y );
}

float computeNormalAxisLineFactor( float2 hitPosition,
                                   float2 ddxHitPosition,
                                   float2 ddyHitPosition,
                                   float scale,
                                   float lineWidth ) {
    float inverseScale = safeReciprocal( scale );
    float distance = length( hitPosition ) * inverseScale;

    float2 radialDirection = hitPosition * safeReciprocal( distance );

    float ddxDistance = dot( ddxHitPosition - hitPosition, radialDirection ) * inverseScale;
    float ddyDistance = dot( ddyHitPosition - hitPosition, radialDirection ) * inverseScale;
    float derivate = length( float2( ddxDistance, ddyDistance ) ) * inverseScale;

    float drawWidth = max( lineWidth, derivate );
    float lineAA = derivate * AntiAliasingFactor;
    float lineFactor = smoothstep( drawWidth + lineAA, drawWidth - lineAA, distance * 2.0f );
    lineFactor *= clamp( lineWidth / drawWidth, 0.0f, 1.0f );
    return lineFactor;
}

[ shader( "fragment" ) ]
float4 fragmentMain( float4 position: SV_Position )
    : SV_Target0 {
    float4 finalColor = float4( 0.0f );

    uint32_t flags = Constants.Flags;
    bool isScaleLocked = ( flags & LockScaleFlag ) == LockScaleFlag;

    float4 minorLineColor = toFloat4Color( Constants.MinorLineColor );
    float4 majorLineColor = toFloat4Color( Constants.MajorLineColor );
    float4 aAxisColor = toFloat4Color( Constants.X_AxisColor );
    float4 bAxisColor = toFloat4Color( Constants.Z_AxisColor );
    float4 cAxisColor = toFloat4Color( Constants.Y_AxisColor );

    float3x3 gridRotation = float3x3( Constants.Rotation00,
                                      Constants.Rotation10,
                                      Constants.Rotation20,
                                      Constants.Rotation01,
                                      Constants.Rotation11,
                                      Constants.Rotation21,
                                      Constants.Rotation02,
                                      Constants.Rotation12,
                                      Constants.Rotation22 );

    float3 rayOriginWorld = gridRotation * Constants.ViewConstants->CameraPosition;
    float3 rayDirection = gridRotation * getRayDirection( position.xy );
    float3 ddxRayDirection = gridRotation * getRayDirection( float2( position.x + 1.0f, position.y ) );
    float3 ddyRayDirection = gridRotation * getRayDirection( float2( position.x, position.y + 1.0f ) );

    float planeCenter = 0.0f;
    float3 planeNormal = float3( 0, 1, 0 );

    float fractionalScaleFactor = fract( Constants.LodLevel );
    float blendFactor = isScaleLocked ? 0.0f : ( 1.0f - smoothstep( 0.05, 0.95, fractionalScaleFactor ) );

    float minorCellSize = max( pow( 10, floor( Constants.LodLevel ) - 1.0f ), 0.01f );
    float previousMinorCellSize = minorCellSize / Constants.GridCellCount;
    float majorCellSize = minorCellSize * Constants.GridCellCount;
    float axisLineWidth = lerp( Constants.AxisLineWidth,
                                Constants.AxisLineWidth / Constants.GridCellCount,
                                blendFactor );

    float2 cameraCenteringOffset = floor( rayOriginWorld.xz / majorCellSize ) * majorCellSize;
    float3 rayOriginLocal = rayOriginWorld - float3( cameraCenteringOffset.x, 0, cameraCenteringOffset.y );

    // intersect plane that is normal / orthogonal to the grid plane for the 3rd axis line
    float3 normalAxisPlaneNormal = normalize( float3( rayOriginWorld.x, 0, rayOriginWorld.z ) );
    float3 normalAxisHitPositionWorld = float3( 0.0 );
    float3 normalAxisDdxHitPositionWorld = float3( 0.0 );
    float3 normalAxisDdyHitPositionWorld = float3( 0.0 );
    // 0.1f should be the axis line draw width
    bool hasNormalPlaneHit = dot( rayOriginWorld.xz, rayOriginWorld.xz ) > 0.1f &&
                             intersectPlane( rayOriginWorld,
                                             rayDirection,
                                             normalAxisPlaneNormal,
                                             planeCenter,
                                             normalAxisHitPositionWorld ) &&
                             intersectPlane( rayOriginWorld,
                                             ddxRayDirection,
                                             normalAxisPlaneNormal,
                                             planeCenter,
                                             normalAxisDdxHitPositionWorld ) &&
                             intersectPlane( rayOriginWorld,
                                             ddyRayDirection,
                                             normalAxisPlaneNormal,
                                             planeCenter,
                                             normalAxisDdyHitPositionWorld );

    float normalAxisLineFactor = 0.0f;
    // draw normal axis below grid plane (e.g Y axis for XZ plane )
    if( hasNormalPlaneHit ) {
        normalAxisLineFactor = computeNormalAxisLineFactor( normalAxisHitPositionWorld.xz,
                                                            normalAxisDdxHitPositionWorld.xz,
                                                            normalAxisDdyHitPositionWorld.xz,
                                                            minorCellSize,
                                                            axisLineWidth );
    }

    if( normalAxisHitPositionWorld.y < 0 ) {
        float4 cAxisColor = toFloat4Color( Constants.Y_AxisColor );
        finalColor = lerp( finalColor, cAxisColor, cAxisColor.a * normalAxisLineFactor );
    }

    // grid visualization
    float3 hitPositionLocal, ddxHitPositionLocal, ddyHitPositionLocal;
    if( intersectPlane( rayOriginLocal, rayDirection, planeNormal, planeCenter, hitPositionLocal ) &&
        intersectPlane( rayOriginLocal, ddxRayDirection, planeNormal, planeCenter, ddxHitPositionLocal ) &&
        intersectPlane( rayOriginLocal, ddyRayDirection, planeNormal, planeCenter, ddyHitPositionLocal ) ) {
        float3 hitPositionWorld = hitPositionLocal + float3( cameraCenteringOffset.x, 0, cameraCenteringOffset.y );

        float2 centeredUV = ( hitPositionLocal.xz ) / minorCellSize;
        float2 worldUV = hitPositionWorld.xz / minorCellSize;

        // Use analytical derivatives from the ray intersection
        float2 ddx_centered = ( ddxHitPositionLocal.xz / minorCellSize ) - centeredUV;
        float2 ddy_centered = ( ddyHitPositionLocal.xz / minorCellSize ) - centeredUV;

        float2 uvDerivatives = float2( length( float2( ddx_centered.x, ddy_centered.x ) ),
                                       length( float2( ddx_centered.y, ddy_centered.y ) ) );

        // grid main axis lines ( X / Y or Z depending on the grid plane )
        float2 axisDrawWidth = max( float2( axisLineWidth ), uvDerivatives );
        float2 axisLineAA = uvDerivatives * AntiAliasingFactor;
        float2 axisLines = smoothstep( axisDrawWidth + axisLineAA, axisDrawWidth - axisLineAA, abs( worldUV * 2.0f ) );
        axisLines *= clamp( axisLineWidth / axisDrawWidth, 0.0f, 1.0f );

        float majorLineWidth = lerp( Constants.MajorLineWidth, Constants.MinorLineWidth, blendFactor ) /
                               Constants.GridCellCount;
        float2 majorUV = centeredUV / Constants.GridCellCount;
        float2 majorUVDerivatives = uvDerivatives / Constants.GridCellCount;
        float2 majorAxisOffset = ( 1.0f - clamp( abs( worldUV / Constants.GridCellCount * 2.0f ), 0.0f, 1.0f ) ) * 2.0f;
        float majorGridLineFactor = computeGridLineFactor( majorUV,
                                                           majorUVDerivatives,
                                                           majorLineWidth,
                                                           majorAxisOffset,
                                                           axisLines );

        // minor lines
        float minorLineWidth = lerp( Constants.MinorLineWidth,
                                     ( Constants.MajorLineWidth / Constants.GridCellCount ),
                                     blendFactor );
        float2 minorAxisOffset = ( 1.0f -
                                   clamp( ( 1.0f - abs( fract( majorUV ) * 2.0f - 1.0f ) ) * Constants.GridCellCount,
                                          0.0f,
                                          1.0f ) ) *
                                 2.0f;
        float minorGridLineFactor = computeGridLineFactor( centeredUV,
                                                           uvDerivatives,
                                                           minorLineWidth,
                                                           minorAxisOffset,
                                                           axisLines );

        // previous level minor lines - this is just for a smoother look when zooming in / out
        if( isScaleLocked == false ) {
            float previousMinorBlendFactor = 1.0f - smoothstep( 0.0f, 0.3f, fractionalScaleFactor );
            float2 previousMinorUV = centeredUV * Constants.GridCellCount;
            float2 previousMinorUVDerivatives = uvDerivatives * Constants.GridCellCount;
            float2 previousMinorAxisOffset = ( 1.0f - clamp( ( 1.0f - abs( fract( centeredUV ) * 2.0f - 1.0f ) ) *
                                                                 Constants.GridCellCount,
                                                             0.0f,
                                                             1.0f ) ) *
                                             2.0f;
            float previousMinorGridLineFactor = computeGridLineFactor( previousMinorUV,
                                                                       previousMinorUVDerivatives,
                                                                       Constants.MinorLineWidth,
                                                                       previousMinorAxisOffset,
                                                                       axisLines );

            finalColor = lerp( finalColor,
                               minorLineColor,
                               previousMinorGridLineFactor * minorLineColor.a * previousMinorBlendFactor );
        }

        float4 axisLineColor = lerp( aAxisColor * axisLines.y, bAxisColor, axisLines.x );

        // coloring grid lines
        finalColor = lerp( finalColor, minorLineColor, minorGridLineFactor * minorLineColor.a );
        finalColor = lerp( finalColor, majorLineColor, majorGridLineFactor * majorLineColor.a );
        finalColor = finalColor * ( 1.0f - axisLineColor.a ) + axisLineColor;
    }

    // draw normal axis above grid plane (e.g Y axis for XZ plane )
    if( normalAxisHitPositionWorld.y > 0 ) {
        finalColor = lerp( finalColor, cAxisColor, cAxisColor.a * normalAxisLineFactor );
    }

    return finalColor;
}
