#include <onyx/editor/panels/sceneeditor/terraintools/sculptterraintool.h>

#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/ui/propertygrid/propertygrid.h>
#include <onyx/volume/components/volumeterraincomponent.gen.h>
#include <onyx/volume/graphics/previewterrainedit.h>

#include <imgui_extra_math.h>

namespace onyx::editor {
namespace {
void drawSmoothArrow( const ImVec2& from,
                      const ImVec2& to,
                      float arrowSize = 12.0f,
                      float thickness = 4.0f,
                      bool roundShaft = true,
                      ImU32 color = IM_COL32( 255, 255, 255, 255 ) ) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ImVec2 dir = to - from;
    float len = sqrtf( dir.x * dir.x + dir.y * dir.y );
    if( len < 0.001f )
        return;

    dir.x /= len;
    dir.y /= len;
    ImVec2 perp( -dir.y, dir.x );

    ImVec2 tipBase = to - dir * arrowSize;
    ImVec2 left = tipBase + perp * ( arrowSize * 0.8f );
    ImVec2 right = tipBase - perp * ( arrowSize * 0.8f );

    float radius = thickness * 0.5f;

    // ---- Draw shaft ----
    drawList->PathClear();
    // draw_list->PathArcTo(p1, radius, IM_PI, 2 * IM_PI, 8);
    drawList->PathLineTo( from );
    drawList->PathLineTo( to - dir * thickness * 0.5f );
    drawList->PathStroke( color, false, thickness ); // false = open path
    // rounded start cap
    if( roundShaft )
        drawList->AddCircleFilled( from, radius, color );

    // ---- Draw arrowhead ----
    drawList->PathClear();
    drawList->PathLineTo( left );
    drawList->PathLineTo( to - dir * thickness * 0.5f );
    drawList->PathLineTo( right );
    drawList->PathStroke( color, ImDrawFlags_None, thickness );

    drawList->AddCircleFilled( left, radius, color );
    drawList->AddCircleFilled( right, radius, color );
}

// Helper to draw dashed line along a cubic Bezier curve
void addDashedCubicBezier( ImDrawList* dl,
                           const ImVec2& p0,
                           const ImVec2& p1,
                           const ImVec2& p2,
                           const ImVec2& p3,
                           ImU32 col,
                           float thickness,
                           float dashLen = 10.0f,
                           float gapLen = 5.0f,
                           int numSegments = 64 ) {
    if( ( col & IM_COL32_A_MASK ) == 0 || dl == nullptr )
        return;

    // float total_len = dash_len + gap_len;
    float tStep = 1.0f / numSegments;
    ImVec2 prev = p0;
    float distAccum = 0.0f;
    bool drawing = true;

    for( int i = 1; i <= numSegments; i++ ) {
        float t = tStep * i;
        ImVec2 p = ImBezierCubicCalc( p0, p1, p2, p3, t );

        // segment length
        float segLen = ImLength( p - prev );

        float remaining = segLen;
        ImVec2 segStart = prev;

        while( remaining > 0.0f ) {
            float spaceLeft = ( drawing ? dashLen : gapLen ) - distAccum;

            float step = ImMin( remaining, spaceLeft );
            ImVec2 segEnd = segStart + ( p - segStart ) * ( step / segLen );

            if( drawing )
                dl->AddLine( segStart, segEnd, col, thickness );

            distAccum += step;
            if( distAccum >= ( drawing ? dashLen : gapLen ) ) {
                drawing = !drawing;
                distAccum = 0.0f;
            }

            segStart = segEnd;
            remaining -= step;
        }

        prev = p;
    }
}

void drawRaiseIcon( ImVec2 pos, float size, float thickness, ImU32 col ) {
    ImDrawList* dl = ImGui::GetWindowDrawList();

    dl->AddRect( pos, pos + ImVec2( size, size ), col, 2.0f, ImDrawFlags_RoundCornersAll, thickness );

    float margin = std::ceil( size * 0.2f );

    // --- Icon proportions relative to size ---
    // float totalW = size - 2 * margin;
    float totalAvailableHeight = size - 2 * margin;
    float hillH = std::ceil( totalAvailableHeight * 0.3f );
    float arrowH = std::ceil( totalAvailableHeight * 0.4f );

    // --- center combined block inside square ---
    float centerX = pos.x + size * 0.5f;

    // hill baseline (bottom of hill, after arrow space)
    float baseY = pos.y + size - margin - thickness;

    // hill X coordinates
    float hillL = pos.x + margin + thickness * 0.5f;
    float hillR = pos.x + size - margin - thickness * 0.5f;
    float midX = centerX;

    // hill peak
    float peakY = baseY - hillH;

    // --- draw hill ---
    dl->PathClear();
    dl->PathLineTo( ImVec2( hillL, baseY ) );
    dl->PathBezierCubicCurveTo( ImVec2( hillL + ( midX - hillL ) * 0.6f, baseY ),
                                ImVec2( midX - ( midX - hillL ) * 0.6f, peakY ),
                                ImVec2( midX, peakY ) );
    dl->PathBezierCubicCurveTo( ImVec2( midX + ( hillR - midX ) * 0.6f, peakY ),
                                ImVec2( hillR - ( hillR - midX ) * 0.6f, baseY ),
                                ImVec2( hillR, baseY ) );
    dl->PathStroke( col, ImDrawListFlags_None, thickness );

    // --- arrow above hill peak ---
    float arrowSize = totalAvailableHeight * 0.2f;
    float arrowX = centerX;
    float arrowToY = pos.y + margin + thickness * 0.5f;
    float arrowFromY = arrowToY + arrowH;
    drawSmoothArrow( ImVec2( arrowX, arrowFromY ), ImVec2( arrowX, arrowToY ), arrowSize, thickness, false, col );
}

void drawLowerIcon( ImVec2 pos, float size, float thickness, ImU32 col ) {
    ImDrawList* dl = ImGui::GetWindowDrawList();

    dl->AddRect( pos, pos + ImVec2( size, size ), col, 2.0f, ImDrawFlags_RoundCornersAll, thickness );

    float margin = std::ceil( size * 0.2f );

    // --- Icon proportions relative to size ---
    // float totalW = size - 2 * margin;
    float totalAvailableHeight = size - 2 * margin;
    float hillH = std::ceil( totalAvailableHeight * 0.3f );
    float arrowH = std::ceil( totalAvailableHeight * 0.4f );

    // --- center combined block inside square ---
    float centerX = pos.x + size * 0.5f;

    // hill baseline (bottom of hill, after arrow space)
    float peakY = pos.y + size - margin - thickness;
    float baseY = peakY - hillH;

    // hill X coordinates
    float hillL = pos.x + margin + thickness * 0.5f;
    float hillR = pos.x + size - margin - thickness * 0.5f;
    float midX = centerX;

    // --- draw hill ---
    dl->PathClear();
    dl->PathLineTo( ImVec2( hillL, baseY ) );
    dl->PathBezierCubicCurveTo( ImVec2( hillL + ( midX - hillL ) * 0.6f, baseY ),
                                ImVec2( midX - ( midX - hillL ) * 0.6f, peakY ),
                                ImVec2( midX, peakY ) );
    dl->PathBezierCubicCurveTo( ImVec2( midX + ( hillR - midX ) * 0.6f, peakY ),
                                ImVec2( hillR - ( hillR - midX ) * 0.6f, baseY ),
                                ImVec2( hillR, baseY ) );
    dl->PathStroke( col, ImDrawListFlags_None, thickness );

    // --- arrow above hill peak ---
    float arrowSize = totalAvailableHeight * 0.2f;
    float arrowX = centerX;
    float arrowFromY = pos.y + margin + thickness * 0.5f;
    float arrowToY = arrowFromY + arrowH;
    drawSmoothArrow( ImVec2( arrowX, arrowFromY ), ImVec2( arrowX, arrowToY ), arrowSize, thickness, true, col );
}

// 2. Flatten Terrain
void drawFlattenIcon( ImVec2 pos, float size, float thickness, ImU32 col ) {
    ImDrawList* dl = ImGui::GetWindowDrawList();

    dl->AddRect( pos, pos + ImVec2( size, size ), col, 2.0f, ImDrawFlags_RoundCornersAll, thickness );

    float margin = std::ceil( size * 0.2f );
    float halfThickness = thickness * 0.5f;

    // --- Icon proportions relative to size ---
    float totalAvailableWidth = size - 2 * margin;
    float totalAvailableHeight = size - 2 * margin;
    // float hillH = std::ceil(totalAvailableHeight * 0.25f);
    float arrowH = std::ceil( totalAvailableHeight * 0.4f );

    // --- center combined block inside square ---
    float contentLeft = pos.x + margin;

    // position base terrain for this close to the hill peak of other icons
    float baseY = pos.y + size - margin - thickness - std::ceil( totalAvailableHeight * 0.1f );

    // House proportions
    float flatTerrainLineLengthLeft = totalAvailableWidth * 0.05f;
    float houseW = std::ceil( totalAvailableWidth * 0.3f );
    float houseH = std::ceil( totalAvailableHeight * 0.25f );
    float houseX = contentLeft + flatTerrainLineLengthLeft;

    float houseY = baseY - houseH;

    // --- Terrain line with small bump ---
    dl->PathClear();
    // Left flat section
    dl->PathLineTo( ImVec2( contentLeft, baseY ) );

    dl->PathLineTo( ImVec2( houseX, baseY ) );

    // Under the house: keep flat
    dl->PathLineTo( ImVec2( houseX + houseW, baseY ) );

    // Smooth bump to the right
    float bumpStart = houseX + houseW + std::ceil( totalAvailableHeight * 0.05f );
    float bumpEnd = bumpStart + std::ceil( totalAvailableHeight * 0.1f );
    float bumpPeakY = baseY - totalAvailableHeight * 0.05f;
    float bumpMid = std::ceil( ( bumpStart + bumpEnd ) * 0.5f );

    dl->PathBezierCubicCurveTo( ImVec2( bumpStart + ( bumpMid - bumpStart ) * 0.6f, baseY ),
                                ImVec2( bumpMid - ( bumpMid - bumpStart ) * 0.6f, bumpPeakY ),
                                ImVec2( bumpMid, bumpPeakY ) );
    dl->PathBezierCubicCurveTo( ImVec2( bumpMid + ( bumpEnd - bumpMid ) * 0.6f, bumpPeakY ),
                                ImVec2( bumpEnd - ( bumpEnd - bumpMid ) * 0.6f, baseY ),
                                ImVec2( bumpEnd, baseY ) );

    // Continue to far right flat
    float flatTerrainRightEnd = pos.x + margin + totalAvailableWidth * 0.9f;
    dl->PathLineTo( ImVec2( flatTerrainRightEnd, baseY ) );

    // Smooth bump at the end
    bumpStart = contentLeft + totalAvailableWidth * 0.9f;
    bumpEnd = contentLeft + totalAvailableWidth;
    bumpMid = ( bumpStart + bumpEnd ) * 0.5f;
    bumpPeakY = baseY - totalAvailableHeight * 0.05f;

    dl->PathBezierCubicCurveTo( ImVec2( bumpStart + ( bumpMid - bumpStart ) * 0.6f, baseY ),
                                ImVec2( bumpMid - ( bumpMid - bumpStart ) * 0.6f, bumpPeakY ),
                                ImVec2( bumpMid, bumpPeakY ) );

    dl->PathStroke( col, false, thickness );

    // --- House shape ---
    float roofHeight = houseH * 0.4f;
    float rounding = totalAvailableWidth * 0.05f;

    ImVec2 bl = ImVec2( houseX, baseY );  // bottom-left
    ImVec2 tl = ImVec2( houseX, houseY ); // top-left wall

    // ImVec2 tlRoof = ImVec2(houseX , houseY);                    // top-left wall
    ImVec2 tr = ImVec2( houseX + houseW, houseY ); // top-right wall
    ImVec2 br = ImVec2( houseX + houseW, baseY );  // bottom-right
    float ridgeWidth = houseW * 0.6f;
    ImVec2 tlRoof = ImVec2( houseX + ( houseW - ridgeWidth ) * 0.5f, houseY - roofHeight );
    ImVec2 trRoof = ImVec2( tlRoof.x + ridgeWidth, houseY - roofHeight );
    dl->PathClear();

    dl->PathLineTo( bl );
    dl->PathArcTo( tl + ImVec2( rounding, rounding ), rounding, IM_PI, IM_PI + IM_PI / 4.0f );
    dl->PathArcTo( tlRoof + ImVec2( rounding, rounding ), rounding, IM_PI + IM_PI / 4.0f, IM_PI + IM_PI / 2.0f );
    dl->PathArcTo( trRoof + ImVec2( -rounding, rounding ), rounding, IM_PI + IM_PI / 2.0f, IM_PI + IM_PI * 0.75f );
    dl->PathArcTo( tr + ImVec2( -rounding, rounding ), rounding, IM_PI + IM_PI * 0.75f, IM_PI + IM_PI );
    dl->PathLineTo( br );

    dl->PathStroke( col, ImDrawFlags_None, thickness );

    float flatTerrainRightStart = houseX + houseW + std::ceil( totalAvailableHeight * 0.05f );
    flatTerrainRightEnd = bumpMid;

    float flatTerrainRightCenter = flatTerrainRightStart + ( flatTerrainRightEnd - flatTerrainRightStart ) * 0.5f;
    float peakY = pos.y + ( pos.y + size - baseY );
    float hillBaseY = baseY - totalAvailableHeight * 0.05f;

    // draw dashed hill background
    float dashLength = 2.0f;
    float gap = 1.0f;
    uint32_t dashedColor = ( col & 0x00FFFFFF ) | ( 128 << IM_COL32_A_SHIFT );
    dl->PathClear();

    addDashedCubicBezier(
        dl,
        ImVec2( flatTerrainRightStart, hillBaseY ),
        ImVec2( flatTerrainRightStart, hillBaseY ),
        ImVec2( flatTerrainRightCenter - ( flatTerrainRightCenter - flatTerrainRightStart ) * 0.8f, peakY ),
        ImVec2( flatTerrainRightCenter, peakY ),
        dashedColor,
        halfThickness,
        dashLength,
        gap );

    addDashedCubicBezier(
        dl,
        ImVec2( flatTerrainRightCenter, peakY ),
        ImVec2( flatTerrainRightCenter + ( flatTerrainRightEnd - flatTerrainRightCenter ) * 0.8f, peakY ),
        ImVec2( flatTerrainRightEnd, hillBaseY ),
        ImVec2( flatTerrainRightEnd, hillBaseY ),
        dashedColor,
        halfThickness,
        dashLength,
        gap );

    dl->PathClear();

    // --- arrow ---
    float arrowSize = totalAvailableHeight * 0.2f;
    float arrowX = flatTerrainRightStart + ( flatTerrainRightEnd - flatTerrainRightStart ) * 0.5f;
    float arrowToY = baseY - std::ceil( totalAvailableHeight * 0.1f );
    float arrowFromY = arrowToY - arrowH;
    drawSmoothArrow( ImVec2( arrowX, arrowFromY ), ImVec2( arrowX, arrowToY ), arrowSize, thickness, false, col );
}

// 3. Smooth/Soften Terrain
void drawSmoothIcon( ImVec2 pos, float size, float thickness, ImU32 col ) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRect( pos, pos + ImVec2( size, size ), col, 2.0f, ImDrawFlags_RoundCornersAll, thickness );

    float margin = std::ceil( size * 0.2f );
    float halfThickness = thickness * 0.5f;

    // --- Icon proportions relative to size ---
    float totalAvailableHeight = size - 2 * margin;
    float hillH = std::ceil( totalAvailableHeight * 0.25f );
    float arrowH = std::ceil( totalAvailableHeight * 0.2f );

    // --- center combined block inside square ---
    float centerX = pos.x + size * 0.5f;

    // hill baseline (bottom of hill, after arrow space)
    float baseY = pos.y + size - margin - thickness;

    // hill X coordinates
    float hillL = pos.x + margin + halfThickness;
    float hillR = pos.x + size - margin - halfThickness;
    float midX = centerX;

    // hill peak
    float peakY = baseY - hillH;
    float hillBaseY = baseY - std::ceil( totalAvailableHeight * 0.05f );
    // --- draw hill ---
    dl->PathClear();
    dl->PathLineTo( ImVec2( hillL, hillBaseY ) );
    dl->PathBezierCubicCurveTo( ImVec2( hillL + ( midX - hillL ) * 0.6f, hillBaseY ),
                                ImVec2( midX - ( midX - hillL ) * 0.6f, peakY ),
                                ImVec2( midX, peakY ) );
    dl->PathBezierCubicCurveTo( ImVec2( midX + ( hillR - midX ) * 0.6f, peakY ),
                                ImVec2( hillR - ( hillR - midX ) * 0.6f, hillBaseY ),
                                ImVec2( hillR, hillBaseY ) );
    dl->PathStroke( col, ImDrawListFlags_None, thickness );

    // draw dashed hill background
    float dashedHillLeftStart = pos.x + margin + halfThickness;
    float dashedHillLeftEnd = centerX;
    float dashedHillLeftMid = dashedHillLeftStart + ( dashedHillLeftEnd - dashedHillLeftStart ) * 0.5f;
    peakY = baseY - std::ceil( totalAvailableHeight * 0.3f );

    float dashLength = 2.0f;
    float gap = 1.0f;

    uint32_t dashedColor = ( col & 0x00FFFFFF ) | ( 128 << IM_COL32_A_SHIFT );
    dl->PathClear();
    addDashedCubicBezier( dl,
                          ImVec2( dashedHillLeftStart, baseY ),
                          ImVec2( dashedHillLeftStart + ( dashedHillLeftMid - dashedHillLeftStart ) * 0.6f, baseY ),
                          ImVec2( dashedHillLeftMid - ( dashedHillLeftMid - dashedHillLeftStart ) * 0.6f, peakY ),
                          ImVec2( dashedHillLeftMid, peakY ),
                          dashedColor,
                          halfThickness,
                          dashLength,
                          gap );

    addDashedCubicBezier( dl,
                          ImVec2( dashedHillLeftMid, peakY ),
                          ImVec2( dashedHillLeftMid + ( dashedHillLeftEnd - dashedHillLeftMid ) * 0.6f, peakY ),
                          ImVec2( dashedHillLeftEnd - ( dashedHillLeftEnd - dashedHillLeftMid ) * 0.6f, baseY ),
                          ImVec2( dashedHillLeftEnd, baseY ),
                          dashedColor,
                          halfThickness,
                          dashLength,
                          gap );

    dashedHillLeftStart = dashedHillLeftEnd;
    dashedHillLeftEnd = pos.x + size - margin - halfThickness;
    dashedHillLeftMid = dashedHillLeftStart + ( dashedHillLeftEnd - dashedHillLeftStart ) * 0.5f;

    addDashedCubicBezier( dl,
                          ImVec2( dashedHillLeftStart, baseY ),
                          ImVec2( dashedHillLeftStart + ( dashedHillLeftMid - dashedHillLeftStart ) * 0.6f, baseY ),
                          ImVec2( dashedHillLeftMid - ( dashedHillLeftMid - dashedHillLeftStart ) * 0.6f, peakY ),
                          ImVec2( dashedHillLeftMid, peakY ),
                          dashedColor,
                          halfThickness,
                          dashLength,
                          gap );

    addDashedCubicBezier( dl,
                          ImVec2( dashedHillLeftMid, peakY ),
                          ImVec2( dashedHillLeftMid + ( dashedHillLeftEnd - dashedHillLeftMid ) * 0.6f, peakY ),
                          ImVec2( dashedHillLeftEnd - ( dashedHillLeftEnd - dashedHillLeftMid ) * 0.6f, baseY ),
                          ImVec2( dashedHillLeftEnd, baseY ),
                          dashedColor,
                          halfThickness,
                          dashLength,
                          gap );

    dl->PathClear();

    // Arrows pointing sideways (left + right at baseline)
    float arrowY = pos.y + arrowH + margin;

    float leftArrowX = pos.x + margin + thickness;
    float rightArrowX = pos.x + size - margin - thickness;
    drawSmoothArrow( ImVec2( centerX, arrowY ), ImVec2( leftArrowX, arrowY ), arrowH, thickness, false, col );
    drawSmoothArrow( ImVec2( centerX, arrowY ), ImVec2( rightArrowX, arrowY ), arrowH, thickness, false, col );
}

// Simple 2D pseudo-Perlin noise function
float noise2D( float x, float y ) {
    int n = (int)x + (int)y * 57;
    n = ( n << 13 ) ^ n;
    int nn = ( n * ( n * n * 15731 + 789221 ) + 1376312589 ) & 0x7fffffff;
    return 1.0f - ( (float)nn / 1073741824.0f ); // range [-1, 1]
}

// Linear interpolation
float lerp( float a, float b, float t ) {
    return a + t * ( b - a );
}

// Smooth noise sampling
float smoothNoise( float x, float y ) {
    int ix = (int)x;
    int iy = (int)y;
    float fx = x - ix;
    float fy = y - iy;

    float n00 = noise2D( (float)ix, (float)iy );
    float n10 = noise2D( (float)ix + 1, (float)iy );
    float n01 = noise2D( (float)ix, (float)iy + 1 );
    float n11 = noise2D( (float)ix + 1, (float)iy + 1 );

    float nx0 = lerp( n00, n10, fx );
    float nx1 = lerp( n01, n11, fx );
    return lerp( nx0, nx1, fy );
}

// 4. Noise/Randomize Terrain
void drawNoiseIcon( ImVec2 pos, float size, ImU32 col ) {
    ImDrawList* dl = ImGui::GetWindowDrawList();

    const int resolution = 32; // number of points per row/col
    const float step = size / resolution;
    const float dotSize = step * 0.6f;

    for( int y = 0; y < resolution; y++ ) {
        for( int x = 0; x < resolution; x++ ) {
            float fx = x * step + step * 0.5f;
            float fy = y * step + step * 0.5f;

            // smooth noise sample
            float n = smoothNoise( x * 0.15f, y * 0.15f ); // lower frequency for smoothness
            n = ( n + 1.0f ) * 0.5f;                       // normalize [0,1]

            // vary alpha based on noise
            ImU32 colAlpha = IM_COL32( ( col >> 0 ) & 0xFF,
                                       ( col >> 8 ) & 0xFF,
                                       ( col >> 16 ) & 0xFF,
                                       (int)( n * 255 ) );

            ImVec2 p = ImVec2( pos.x + fx, pos.y + fy );
            dl->AddRectFilled( ImVec2( p.x - dotSize * 0.5f, p.y - dotSize * 0.5f ),
                               ImVec2( p.x + dotSize * 0.5f, p.y + dotSize * 0.5f ),
                               colAlpha );
        }
    }
}

void drawStampIcon( ImVec2 topLeft, float size, ImU32 col ) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float w = size;
    float h = size;

    ImVec2 center( topLeft.x + w * 0.5f, topLeft.y + h * 0.5f );

    // --- base (wide rectangle at bottom) ---
    float baseH = h * 0.22f;
    ImVec2 baseMin( center.x - w * 0.4f, topLeft.y + h - baseH );
    ImVec2 baseMax( center.x + w * 0.4f, topLeft.y + h - 1.0f );
    dl->AddRectFilled( baseMin, baseMax, col, 2.0f );

    // --- neck (slightly wider than stem, under handle) ---
    float neckH = h * 0.12f;
    ImVec2 neckMin( center.x - w * 0.18f, baseMin.y - neckH );
    ImVec2 neckMax( center.x + w * 0.18f, baseMin.y );
    dl->AddRectFilled( neckMin, neckMax, col, 2.0f );

    // --- stem (connector) ---
    ImVec2 stemMin( center.x - w * 0.08f, neckMin.y - h * 0.20f );
    ImVec2 stemMax( center.x + w * 0.08f, neckMin.y );
    dl->AddRectFilled( stemMin, stemMax, col );

    // --- handle (circle on top) ---
    float knobR = w * 0.22f;
    ImVec2 knobCenter( center.x, stemMin.y - knobR * 0.2f );
    dl->AddCircleFilled( knobCenter, knobR, col, 16 );

    // Optional: outline for readability
    dl->AddRect( baseMin, baseMax, IM_COL32( 0, 0, 0, 160 ), 2.0f, 0, 1.0f );
    dl->AddCircle( knobCenter, knobR, IM_COL32( 0, 0, 0, 160 ), 16, 1.0f );
}
} // namespace

SculptTerrainTool::SculptTerrainTool( rhi::GraphicsSystem& graphicsSystem )
    : m_CreateVolumeSourceShader(
          graphicsSystem.createShaderInstance( "engine:/shaders/compute/volume/createvolumebrush.oshader" ) ) {}

StringView SculptTerrainTool::GetTitle() {
    return "Sculpt";
}

void SculptTerrainTool::Render() {
    float32 iconSize = 64;

    ImGui::BeginHorizontal( "##group", ImVec2( 0, 0 ) );
    {
        ui::ScopedImGuiStyle style{ { ImGuiStyleVar_FrameBorderSize, 2.0f } };

        ui::ScopedImGuiColor colors{ { ImGuiCol_Button, 0xFFAA8877 },
                                     { ImGuiCol_ButtonHovered, 0xFFBB9977 },
                                     { ImGuiCol_ButtonActive, 0xFFEEAA77 } };
        RenderBrushToolbarButton( SculptType::Raise, iconSize );
        RenderBrushToolbarButton( SculptType::Lower, iconSize );

        // not implemented yet
        // focusSceneView |= RenderBrushToolbarButton(SculptType::Smooth, iconSize);
        // focusSceneView |= RenderBrushToolbarButton(SculptType::Flatten, iconSize);
        // focusSceneView |= RenderBrushToolbarButton(SculptType::Noise, iconSize);
        // focusSceneView |= RenderBrushToolbarButton(SculptType::Stamp, iconSize);
    }
    ImGui::EndHorizontal();

    RenderProperties();

    volume::PreviewTerrainEditPass::s_brushSize = m_BrushSize;
    volume::PreviewTerrainEditPass::s_brushType = 0;
    volume::PreviewTerrainEditPass::s_brushOperation = 0;
}

void SculptTerrainTool::ApplyOperation( rhi::CommandBuffer& commandBuffer,
                                        const rhi::BufferHandle& hitBuffer,
                                        volume::TerrainWorldOctreeComponent& terrainOctree ) {
    struct CreateVolumeSourcePushConstants {
        uint64_t WorldVolumesList;
        uint64_t WorldVolumesData;

        uint64_t HitBufferAddress;
        uint32_t BrushType;
        uint32_t BrushOperation;

        Vector3f32 BrushSize;
        float32 Smoothness;
    };

    CreateVolumeSourcePushConstants createVolumeSourceConstants;
    commandBuffer.barrier( terrainOctree.VolumeObjects, rhi::Context::Compute, rhi::Access::ShaderWrite );
    commandBuffer.barrier( terrainOctree.VolumeObjectsData, rhi::Context::Compute, rhi::Access::ShaderWrite );
    createVolumeSourceConstants.WorldVolumesList = terrainOctree.VolumeObjects.getGpuAddress();
    createVolumeSourceConstants.WorldVolumesData = terrainOctree.VolumeObjectsData.getGpuAddress();
    createVolumeSourceConstants.HitBufferAddress = hitBuffer.getGpuAddress();
    createVolumeSourceConstants.BrushSize = m_BrushSize;
    createVolumeSourceConstants.BrushType = 5;
    createVolumeSourceConstants.BrushOperation = m_Type == SculptType::Lower ? 1 : 0;
    createVolumeSourceConstants.Smoothness = m_Smoothness;
    commandBuffer.bindShaderEffect( m_CreateVolumeSourceShader );
    commandBuffer.bindPushConstants( rhi::ShaderStage::Compute, 0, createVolumeSourceConstants );
    commandBuffer.dispatch( 1, 1, 1 );

    commandBuffer.barrier( terrainOctree.VolumeObjects, rhi::Context::Compute, rhi::Access::ShaderRead );
    commandBuffer.barrier( terrainOctree.VolumeObjectsData, rhi::Context::Compute, rhi::Access::ShaderRead );
}

void SculptTerrainTool::OnHitPositionReadback( game_core::Scene& /*scene*/,
                                               const ecs::ComponentFactory& /*componentFactory*/,
                                               const Vector3f32& /*hitPosition*/ ) {
    // Nothing to do for sculpting as we do not create entities;
}

void SculptTerrainTool::OnBrushSizeInput( float32 value ) {
    m_BrushSize += Vector3f32( value );
}

void SculptTerrainTool::RenderProperties() {
    ImGui::BeginChild( "Panel", ImVec2( 0, 0 ), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar );

    ui::property_grid::beginPropertyGrid( "Properties", 80.0f );

    ui::property_grid::drawProperty( "Brush Size", m_BrushSize.X );
    ui::property_grid::drawProperty( "Smoothness", m_Smoothness );

    ui::property_grid::endPropertyGrid();
    ImGui::EndChild();
}

bool SculptTerrainTool::RenderBrushToolbarButton( SculptType type, float32 buttonSize ) {
    bool isClicked = false;
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    if( ImGui::InvisibleButton( enums::toString( type ).data(), ImVec2( buttonSize, buttonSize ) ) ) {
        m_Type = type;
        isClicked = true;
    }

    float32 thickness = ImGui::GetStyle().FrameBorderSize;
    uint32_t color = m_Type == type           ? ImGui::GetColorU32( ImGuiCol_ButtonActive )
                     : ImGui::IsItemHovered() ? ImGui::GetColorU32( ImGuiCol_ButtonHovered )
                                              : ImGui::GetColorU32( ImGuiCol_Button );
    switch( type ) {
    case SculptType::Raise:
        drawRaiseIcon( cursorPos, buttonSize, thickness, color );
        break;
    case SculptType::Lower:
        drawLowerIcon( cursorPos, buttonSize, thickness, color );
        break;
    case SculptType::Flatten:
        drawFlattenIcon( cursorPos, buttonSize, thickness, color );
        break;
    case SculptType::Smooth:
        drawSmoothIcon( cursorPos, buttonSize, thickness, color );
        break;
    case SculptType::Noise:
        drawNoiseIcon( cursorPos, buttonSize, color );
        break;
    case SculptType::Stamp:
        drawStampIcon( cursorPos, buttonSize, color );
        break;
    case SculptType::None:
        break;
    }

    return isClicked;
}
} // namespace onyx::editor
