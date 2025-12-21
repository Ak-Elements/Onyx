#include <onyx/editor/panels/sceneeditor/terrainpanel.h>

#include <onyx/graphics/commandbuffer.h>

#include <onyx/gamecore/scene/scene.h>
#include <onyx/geometry/rect2.h>
#include <onyx/volume/components/volumeterraincomponent.h>
#include <onyx/volume/systems/volumeterrainsystem.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/ui/scopedcolor.h>

#include <onyx/editor/panels/sceneeditor/terraintools/primitivesterraintool.h>
#include <onyx/editor/panels/sceneeditor/terraintools/sculptterraintool.h>
#include <onyx/graphics/graphicssystem.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/volume/graphics/previewterrainedit.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <ImGuizmo.h>
#include <imgui_extra_math.inl>
#include <imgui_internal.h>


namespace
{
    //void DrawSmoothArrow(const ImVec2& from, const ImVec2& to, float arrow_size = 12.0f, float thickness = 4.0f, bool roundShaft = true, ImU32 color = IM_COL32(255, 255, 255, 255))
    //{
    //    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    //    ImVec2 dir = to - from;
    //    float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
    //    if (len < 0.001f) return;

    //    dir.x /= len;
    //    dir.y /= len;
    //    ImVec2 perp(-dir.y, dir.x);

    //    ImVec2 tip_base = to - dir * arrow_size;
    //    ImVec2 left = tip_base + perp * (arrow_size * 0.8f);
    //    ImVec2 right = tip_base - perp * (arrow_size * 0.8f);

    //    float radius = thickness * 0.5f;

    //    // ---- Draw shaft ----
    //    draw_list->PathClear();
    //    //draw_list->PathArcTo(p1, radius, IM_PI, 2 * IM_PI, 8);
    //    draw_list->PathLineTo(from);
    //    draw_list->PathLineTo(to - dir * thickness * 0.5f);
    //    draw_list->PathStroke(color, false, thickness); // false = open path
    //    // rounded start cap
    //    if (roundShaft)
    //        draw_list->AddCircleFilled(from, radius, color);

    //    // ---- Draw arrowhead ----
    //    draw_list->PathClear();
    //    draw_list->PathLineTo(left);
    //    draw_list->PathLineTo(to - dir * thickness * 0.5f);
    //    draw_list->PathLineTo(right);
    //    draw_list->PathStroke(color, ImDrawFlags_None, thickness);

    //    draw_list->AddCircleFilled(left, radius, color);
    //    draw_list->AddCircleFilled(right, radius, color);
    //}

    //// Helper to draw dashed line along a cubic Bezier curve
    //void AddDashedCubicBezier(
    //    ImDrawList* dl,
    //    const ImVec2& p0, const ImVec2& p1,
    //    const ImVec2& p2, const ImVec2& p3,
    //    ImU32 col, float thickness,
    //    float dash_len = 10.0f, float gap_len = 5.0f,
    //    int num_segments = 64)
    //{
    //    if ((col & IM_COL32_A_MASK) == 0 || dl == nullptr)
    //        return;

    //    //float total_len = dash_len + gap_len;
    //    float t_step = 1.0f / num_segments;
    //    ImVec2 prev = p0;
    //    float dist_accum = 0.0f;
    //    bool drawing = true;

    //    for (int i = 1; i <= num_segments; i++)
    //    {
    //        float t = t_step * i;
    //        ImVec2 p = ImBezierCubicCalc(p0, p1, p2, p3, t);

    //        // segment length
    //        float seg_len = ImLength(p - prev);

    //        float remaining = seg_len;
    //        ImVec2 seg_start = prev;

    //        while (remaining > 0.0f)
    //        {
    //            float space_left = (drawing ? dash_len : gap_len) - dist_accum;

    //            float step = ImMin(remaining, space_left);
    //            ImVec2 seg_end = seg_start + (p - seg_start) * (step / seg_len);

    //            if (drawing)
    //                dl->AddLine(seg_start, seg_end, col, thickness);

    //            dist_accum += step;
    //            if (dist_accum >= (drawing ? dash_len : gap_len))
    //            {
    //                drawing = !drawing;
    //                dist_accum = 0.0f;
    //            }

    //            seg_start = seg_end;
    //            remaining -= step;
    //        }

    //        prev = p;
    //    }
    //}

    //void DrawRaiseIcon(ImVec2 pos, float size, float thickness, ImU32 col)
    //{
    //    ImDrawList* dl = ImGui::GetWindowDrawList();

    //    dl->AddRect(pos, pos + ImVec2(size, size), col, 2.0f, ImDrawFlags_RoundCornersAll, thickness);

    //    float margin = std::ceil(size * 0.2f);

    //    // --- Icon proportions relative to size ---
    //    //float totalW = size - 2 * margin;
    //    float totalAvailableHeight = size - 2 * margin;
    //    float hillH = std::ceil(totalAvailableHeight * 0.3f);
    //    float arrowH = std::ceil(totalAvailableHeight * 0.4f);

    //    // --- center combined block inside square ---
    //    float centerX = pos.x + size * 0.5f;

    //    // hill baseline (bottom of hill, after arrow space)
    //    float baseY = pos.y + size - margin - thickness;

    //    // hill X coordinates
    //    float hillL = pos.x + margin + thickness * 0.5f;
    //    float hillR = pos.x + size - margin - thickness * 0.5f;
    //    float midX = centerX;

    //    // hill peak
    //    float peakY = baseY - hillH;

    //    // --- draw hill ---
    //    dl->PathClear();
    //    dl->PathLineTo(ImVec2(hillL, baseY));
    //    dl->PathBezierCubicCurveTo(
    //        ImVec2(hillL + (midX - hillL) * 0.6f, baseY),
    //        ImVec2(midX - (midX - hillL) * 0.6f, peakY),
    //        ImVec2(midX, peakY)
    //    );
    //    dl->PathBezierCubicCurveTo(
    //        ImVec2(midX + (hillR - midX) * 0.6f, peakY),
    //        ImVec2(hillR - (hillR - midX) * 0.6f, baseY),
    //        ImVec2(hillR, baseY)
    //    );
    //    dl->PathStroke(col, ImDrawListFlags_None, thickness);

    //    // --- arrow above hill peak ---
    //    float arrowSize = totalAvailableHeight * 0.2f;
    //    float arrowX = centerX;
    //    float arrowToY = pos.y + margin + thickness * 0.5f;
    //    float arrowFromY = arrowToY + arrowH;
    //    DrawSmoothArrow(ImVec2(arrowX, arrowFromY), ImVec2(arrowX, arrowToY), arrowSize, thickness, false, col);
    //}

    //void DrawLowerIcon(ImVec2 pos, float size, float thickness, ImU32 col)
    //{
    //    ImDrawList* dl = ImGui::GetWindowDrawList();

    //    dl->AddRect(pos, pos + ImVec2(size, size), col, 2.0f, ImDrawFlags_RoundCornersAll, thickness);

    //    float margin = std::ceil(size * 0.2f);

    //    // --- Icon proportions relative to size ---
    //    //float totalW = size - 2 * margin;
    //    float totalAvailableHeight = size - 2 * margin;
    //    float hillH = std::ceil(totalAvailableHeight * 0.3f);
    //    float arrowH = std::ceil(totalAvailableHeight * 0.4f);

    //    // --- center combined block inside square ---
    //    float centerX = pos.x + size * 0.5f;

    //    // hill baseline (bottom of hill, after arrow space)
    //    float peakY = pos.y + size - margin - thickness;
    //    float baseY = peakY - hillH;

    //    // hill X coordinates
    //    float hillL = pos.x + margin + thickness * 0.5f;
    //    float hillR = pos.x + size - margin - thickness * 0.5f;
    //    float midX = centerX;

    //    // --- draw hill ---
    //    dl->PathClear();
    //    dl->PathLineTo(ImVec2(hillL, baseY));
    //    dl->PathBezierCubicCurveTo(
    //        ImVec2(hillL + (midX - hillL) * 0.6f, baseY),
    //        ImVec2(midX - (midX - hillL) * 0.6f, peakY),
    //        ImVec2(midX, peakY)
    //    );
    //    dl->PathBezierCubicCurveTo(
    //        ImVec2(midX + (hillR - midX) * 0.6f, peakY),
    //        ImVec2(hillR - (hillR - midX) * 0.6f, baseY),
    //        ImVec2(hillR, baseY)
    //    );
    //    dl->PathStroke(col, ImDrawListFlags_None, thickness);

    //    // --- arrow above hill peak ---
    //    float arrowSize = totalAvailableHeight * 0.2f;
    //    float arrowX = centerX;
    //    float arrowFromY = pos.y + margin + thickness * 0.5f;
    //    float arrowToY = arrowFromY + arrowH;
    //    DrawSmoothArrow(ImVec2(arrowX, arrowFromY), ImVec2(arrowX, arrowToY), arrowSize, thickness, true, col);
    //}

    //// 2. Flatten Terrain
    //void DrawFlattenIcon(ImVec2 pos, float size, float thickness, ImU32 col)
    //{
    //    ImDrawList* dl = ImGui::GetWindowDrawList();

    //    dl->AddRect(pos, pos + ImVec2(size, size), col, 2.0f, ImDrawFlags_RoundCornersAll, thickness);

    //    float margin = std::ceil(size * 0.2f);
    //    float halfThickness = thickness * 0.5f;

    //    // --- Icon proportions relative to size ---
    //    float totalAvailableWidth = size - 2 * margin;
    //    float totalAvailableHeight = size - 2 * margin;
    //    //float hillH = std::ceil(totalAvailableHeight * 0.25f);
    //    float arrowH = std::ceil(totalAvailableHeight * 0.4f);

    //    // --- center combined block inside square ---
    //    float contentLeft = pos.x + margin;

    //    // position base terrain for this close to the hill peak of other icons
    //    float baseY = pos.y + size - margin - thickness - std::ceil(totalAvailableHeight * 0.1f);

    //    // House proportions
    //    float flatTerrainLineLengthLeft = totalAvailableWidth * 0.05f;
    //    float houseW = std::ceil(totalAvailableWidth * 0.3f);
    //    float houseH = std::ceil(totalAvailableHeight * 0.25f);
    //    float houseX = contentLeft + flatTerrainLineLengthLeft;

    //    float houseY = baseY - houseH;

    //    // --- Terrain line with small bump ---
    //    dl->PathClear();
    //    // Left flat section
    //    dl->PathLineTo(ImVec2(contentLeft, baseY));

    //    dl->PathLineTo(ImVec2(houseX, baseY));

    //    // Under the house: keep flat
    //    dl->PathLineTo(ImVec2(houseX + houseW, baseY));

    //    // Smooth bump to the right
    //    float bumpStart = houseX + houseW + std::ceil(totalAvailableHeight * 0.05f);
    //    float bumpEnd = bumpStart + std::ceil(totalAvailableHeight * 0.1f);
    //    float bumpPeakY = baseY - totalAvailableHeight * 0.05f;
    //    float bumpMid = std::ceil((bumpStart + bumpEnd) * 0.5f);

    //    dl->PathBezierCubicCurveTo(
    //        ImVec2(bumpStart + (bumpMid - bumpStart) * 0.6f, baseY),
    //        ImVec2(bumpMid - (bumpMid - bumpStart) * 0.6f, bumpPeakY),
    //        ImVec2(bumpMid, bumpPeakY)
    //    );
    //    dl->PathBezierCubicCurveTo(
    //        ImVec2(bumpMid + (bumpEnd - bumpMid) * 0.6f, bumpPeakY),
    //        ImVec2(bumpEnd - (bumpEnd - bumpMid) * 0.6f, baseY),
    //        ImVec2(bumpEnd, baseY)
    //    );

    //    // Continue to far right flat
    //    float flatTerrainRightEnd = pos.x + margin + totalAvailableWidth * 0.9f;
    //    dl->PathLineTo(ImVec2(flatTerrainRightEnd, baseY));

    //    // Smooth bump at the end
    //    bumpStart = contentLeft + totalAvailableWidth * 0.9f;
    //    bumpEnd = contentLeft + totalAvailableWidth;
    //    bumpMid = (bumpStart + bumpEnd) * 0.5f;
    //    bumpPeakY = baseY - totalAvailableHeight * 0.05f;

    //    dl->PathBezierCubicCurveTo(
    //        ImVec2(bumpStart + (bumpMid - bumpStart) * 0.6f, baseY),
    //        ImVec2(bumpMid - (bumpMid - bumpStart) * 0.6f, bumpPeakY),
    //        ImVec2(bumpMid, bumpPeakY)
    //    );

    //    dl->PathStroke(col, false, thickness);

    //    // --- House shape ---
    //    float roofHeight = houseH * 0.4f;
    //    float rounding = totalAvailableWidth * 0.05f;

    //    ImVec2 bl = ImVec2(houseX, baseY);                     // bottom-left
    //    ImVec2 tl = ImVec2(houseX, houseY);                    // top-left wall
    //    
    //    //ImVec2 tlRoof = ImVec2(houseX , houseY);                    // top-left wall
    //    ImVec2 tr = ImVec2(houseX + houseW, houseY);           // top-right wall
    //    ImVec2 br = ImVec2(houseX + houseW, baseY);            // bottom-right
    //    float ridgeWidth = houseW * 0.6f;
    //    ImVec2 tlRoof = ImVec2(houseX + (houseW - ridgeWidth) * 0.5f, houseY - roofHeight);
    //    ImVec2 trRoof = ImVec2(tlRoof.x + ridgeWidth, houseY - roofHeight);
    //    dl->PathClear();

    //    dl->PathLineTo(bl);
    //    dl->PathArcTo(tl + ImVec2(rounding, rounding), rounding, IM_PI, IM_PI + IM_PI /4.0f);
    //    dl->PathArcTo(tlRoof + ImVec2(rounding, rounding), rounding, IM_PI + IM_PI / 4.0f, IM_PI + IM_PI / 2.0f);
    //    dl->PathArcTo(trRoof + ImVec2(-rounding, rounding), rounding, IM_PI + IM_PI / 2.0f, IM_PI + IM_PI * 0.75f);
    //    dl->PathArcTo(tr + ImVec2(-rounding, rounding), rounding, IM_PI + IM_PI * 0.75f, IM_PI + IM_PI);
    //    dl->PathLineTo(br);

    //    dl->PathStroke(col, ImDrawFlags_None, thickness);

    //    float flatTerrainRightStart = houseX + houseW + std::ceil(totalAvailableHeight * 0.05f);
    //    flatTerrainRightEnd = bumpMid;

    //    float flatTerrainRightCenter = flatTerrainRightStart + (flatTerrainRightEnd - flatTerrainRightStart) * 0.5f;
    //    float peakY = pos.y + (pos.y + size - baseY);
    //    float hillBaseY = baseY - totalAvailableHeight * 0.05f;

    //    // draw dashed hill background
    //    float dashLength = 2.0f;
    //    float gap = 1.0f;
    //    Onyx::onyxU32 dashedColor = (col & 0x00FFFFFF) | (128 << IM_COL32_A_SHIFT);
    //    dl->PathClear();

    //    AddDashedCubicBezier(dl,
    //        ImVec2(flatTerrainRightStart, hillBaseY),
    //        ImVec2(flatTerrainRightStart, hillBaseY),
    //        ImVec2(flatTerrainRightCenter - (flatTerrainRightCenter - flatTerrainRightStart) * 0.8f, peakY),
    //        ImVec2(flatTerrainRightCenter, peakY),
    //        dashedColor, halfThickness, dashLength, gap);

    //    AddDashedCubicBezier(dl,
    //        ImVec2(flatTerrainRightCenter, peakY),
    //        ImVec2(flatTerrainRightCenter + (flatTerrainRightEnd - flatTerrainRightCenter) * 0.8f, peakY),
    //        ImVec2(flatTerrainRightEnd, hillBaseY),
    //        ImVec2(flatTerrainRightEnd, hillBaseY),
    //        dashedColor, halfThickness, dashLength, gap);

    //    dl->PathClear();

    //    // --- arrow ---
    //    float arrowSize = totalAvailableHeight * 0.2f;
    //    float arrowX = flatTerrainRightStart + (flatTerrainRightEnd - flatTerrainRightStart) * 0.5f;
    //    float arrowToY = baseY - std::ceil(totalAvailableHeight * 0.1f);
    //    float arrowFromY = arrowToY - arrowH;
    //    DrawSmoothArrow(ImVec2(arrowX, arrowFromY), ImVec2(arrowX, arrowToY), arrowSize, thickness, false, col);
    //}

    //// 3. Smooth/Soften Terrain
    //void DrawSmoothIcon(ImVec2 pos, float size, float thickness, ImU32 col)
    //{
    //    ImDrawList* dl = ImGui::GetWindowDrawList();
    //    dl->AddRect(pos, pos + ImVec2(size, size), col, 2.0f, ImDrawFlags_RoundCornersAll, thickness);
    //    
    //    float margin = std::ceil(size * 0.2f);
    //    float halfThickness = thickness * 0.5f;

    //    // --- Icon proportions relative to size ---
    //    float totalAvailableHeight = size - 2 * margin;
    //    float hillH = std::ceil(totalAvailableHeight * 0.25f);
    //    float arrowH = std::ceil(totalAvailableHeight * 0.2f);

    //    // --- center combined block inside square ---
    //    float centerX = pos.x + size * 0.5f;

    //    // hill baseline (bottom of hill, after arrow space)
    //    float baseY = pos.y + size - margin - thickness;

    //    // hill X coordinates
    //    float hillL = pos.x + margin + halfThickness;
    //    float hillR = pos.x + size - margin - halfThickness;
    //    float midX = centerX;

    //    // hill peak
    //    float peakY = baseY - hillH;
    //    float hillBaseY = baseY - std::ceil(totalAvailableHeight * 0.05f);
    //    // --- draw hill ---
    //    dl->PathClear();
    //    dl->PathLineTo(ImVec2(hillL, hillBaseY));
    //    dl->PathBezierCubicCurveTo(
    //        ImVec2(hillL + (midX - hillL) * 0.6f, hillBaseY),
    //        ImVec2(midX - (midX - hillL) * 0.6f, peakY),
    //        ImVec2(midX, peakY)
    //    );
    //    dl->PathBezierCubicCurveTo(
    //        ImVec2(midX + (hillR - midX) * 0.6f, peakY),
    //        ImVec2(hillR - (hillR - midX) * 0.6f, hillBaseY),
    //        ImVec2(hillR, hillBaseY)
    //    );
    //    dl->PathStroke(col, ImDrawListFlags_None, thickness);

    //    // draw dashed hill background
    //    float dashedHillLeftStart = pos.x + margin + halfThickness;
    //    float dashedHillLeftEnd = centerX;
    //    float dashedHillLeftMid = dashedHillLeftStart + (dashedHillLeftEnd - dashedHillLeftStart) * 0.5f;
    //    peakY = baseY - std::ceil(totalAvailableHeight * 0.3f);

    //    float dashLength = 2.0f;
    //    float gap = 1.0f;
    //    
    //    Onyx::onyxU32 dashedColor = (col & 0x00FFFFFF) | (128 << IM_COL32_A_SHIFT);
    //    dl->PathClear();
    //    AddDashedCubicBezier(dl,
    //        ImVec2(dashedHillLeftStart, baseY),
    //        ImVec2(dashedHillLeftStart + (dashedHillLeftMid - dashedHillLeftStart) * 0.6f, baseY),
    //        ImVec2(dashedHillLeftMid - (dashedHillLeftMid - dashedHillLeftStart) * 0.6f, peakY),
    //        ImVec2(dashedHillLeftMid, peakY),
    //        dashedColor, halfThickness, dashLength, gap);

    //    AddDashedCubicBezier(dl,
    //        ImVec2(dashedHillLeftMid, peakY),
    //        ImVec2(dashedHillLeftMid + (dashedHillLeftEnd - dashedHillLeftMid) * 0.6f, peakY),
    //        ImVec2(dashedHillLeftEnd - (dashedHillLeftEnd - dashedHillLeftMid) * 0.6f, baseY),
    //        ImVec2(dashedHillLeftEnd, baseY),
    //        dashedColor, halfThickness, dashLength, gap);

    //    dashedHillLeftStart = dashedHillLeftEnd;
    //    dashedHillLeftEnd = pos.x + size - margin - halfThickness;
    //    dashedHillLeftMid = dashedHillLeftStart + (dashedHillLeftEnd - dashedHillLeftStart) * 0.5f;

    //    AddDashedCubicBezier(dl,
    //        ImVec2(dashedHillLeftStart, baseY),
    //        ImVec2(dashedHillLeftStart + (dashedHillLeftMid - dashedHillLeftStart) * 0.6f, baseY),
    //        ImVec2(dashedHillLeftMid - (dashedHillLeftMid - dashedHillLeftStart) * 0.6f, peakY),
    //        ImVec2(dashedHillLeftMid, peakY),
    //        dashedColor, halfThickness, dashLength, gap);

    //    AddDashedCubicBezier(dl,
    //        ImVec2(dashedHillLeftMid, peakY),
    //        ImVec2(dashedHillLeftMid + (dashedHillLeftEnd - dashedHillLeftMid) * 0.6f, peakY),
    //        ImVec2(dashedHillLeftEnd - (dashedHillLeftEnd - dashedHillLeftMid) * 0.6f, baseY),
    //        ImVec2(dashedHillLeftEnd, baseY),
    //        dashedColor, halfThickness, dashLength, gap);

    //    dl->PathClear();


    //    // Arrows pointing sideways (left + right at baseline)
    //    float arrowY = pos.y + arrowH + margin;

    //    float leftArrowX = pos.x + margin + thickness;
    //    float rightArrowX = pos.x + size - margin - thickness;
    //    DrawSmoothArrow(ImVec2(centerX, arrowY), ImVec2(leftArrowX, arrowY), arrowH, thickness, false, col);
    //    DrawSmoothArrow(ImVec2(centerX, arrowY), ImVec2(rightArrowX, arrowY), arrowH, thickness, false, col);
    //}

    //void DrawRoundedTriangle(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, float radius, ImU32 color)
    //{
    //    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    //    struct Corner {
    //        ImVec2 pos;
    //        ImVec2 dirPrev;
    //        ImVec2 dirNext;
    //    };

    //    // List corners in CCW order
    //    Corner corners[3] = {
    //        { p1, p3 - p1, p2 - p1 },
    //        { p2, p1 - p2, p3 - p2 },
    //        { p3, p2 - p3, p1 - p3 }
    //    };

    //    auto normalize = [](ImVec2 v) {
    //        float len = sqrtf(v.x * v.x + v.y * v.y);
    //        return len > 0 ? ImVec2(v.x / len, v.y / len) : ImVec2();
    //        };

    //    draw_list->PathClear();

    //    for (int i = 0; i < 3; i++)
    //    {
    //        Corner& c = corners[i];
    //        ImVec2 prev = normalize(c.dirPrev);
    //        ImVec2 next = normalize(c.dirNext);

    //        // Bisector of the corner
    //        ImVec2 bisector = normalize(prev + next);
    //        float angleBetween = acosf(prev.x * next.x + prev.y * next.y);

    //        // Distance from corner along each edge to start/end of arc
    //        float offset = radius / tanf(angleBetween * 0.5f);

    //        // Entry/exit points of the rounded corner
    //        ImVec2 pIn = c.pos + prev * offset;
    //        ImVec2 pOut = c.pos + next * offset;

    //        // Compute arc center
    //        ImVec2 center = c.pos + bisector * (radius / sinf(angleBetween * 0.5f));

    //        // Angles for the arc
    //        float startAngle = atan2f(pIn.y - center.y, pIn.x - center.x);
    //        float endAngle = atan2f(pOut.y - center.y, pOut.x - center.x);

    //        // ImGui arcs must sweep CCW; ensure correct ordering
    //        if (endAngle < startAngle) {
    //            endAngle += 2 * IM_PI;
    //        }

    //        draw_list->PathArcTo(center, radius, startAngle, endAngle, 0);
    //    }

    //    draw_list->PathFillConvex(color);
    //}

    //// 3. Smooth/Soften Terrain
    //void DrawSculptIcon(ImVec2 pos, float size, float thickness, ImU32 col)
    //{
    //    ImDrawList* drawList = ImGui::GetWindowDrawList();
    //    drawList->AddRect(pos, pos + ImVec2(size, size), col, 2.0f, ImDrawFlags_RoundCornersAll, thickness);

    //   // float margin = std::ceil(size * 0.2f);
    //    

    //    // --- Icon proportions relative to size ---
    //   // float totalAvailableHeight = size - 2 * margin;

    //    // --- center combined block inside square ---
    //    float centerX = pos.x + size * 0.5f;
    //    float centerY = pos.y + size * 0.5f;
    //    //
    //    const ImVec2 tipOffset = ImVec2(0.2f, 0.15f);
    //    const ImVec2 topOffset = ImVec2(0.06f, 0.05f);
    //    const ImVec2 bottomOffset = ImVec2(0.05f, 0.10f);
    //    //
    //    float scale = size * 1.5f;
    //    //
    //    ImVec2 chiselTip(centerX - tipOffset.x * scale, centerY + tipOffset.y * scale);
    //    ImVec2 chiselTop(centerX - topOffset.x * scale, centerY - topOffset.y * scale);
    //    ImVec2 chiselBottom(centerX + bottomOffset.x * scale, centerY + bottomOffset.y * scale);
    //    DrawRoundedTriangle(chiselTip, chiselTop, chiselBottom, 1.0f, 0xFF0000FF);

    //    Onyx::Vector2f32 direction(chiselTop.x - chiselBottom.x, chiselTop.y - chiselBottom.y);
    //    direction.Normalize();

    //    float handleWidth = 0.02f * size;
    //    ImVec2 handleCenter = chiselBottom + (chiselTop - chiselBottom) * 0.5f;
    //    ImVec2 handleBottomLeft = handleCenter + ImVec2(direction.X, direction.Y) * handleWidth;
    //    ImVec2 handleBottomRight = handleCenter - ImVec2(direction.X, direction.Y) * handleWidth;

    //    float handleLength = 8.0f;
    //    float handleHeight = 5.0f;
    //    float radius = handleWidth;
    //    drawList->PathLineTo(handleBottomRight + ImVec2(handleLength, -handleHeight - 5.0f));
    //    drawList->PathLineTo(handleBottomRight + ImVec2(0, -handleHeight));
    //    drawList->PathLineTo(handleBottomRight);
    //    drawList->PathArcTo(handleCenter, radius, Onyx::ToRadians(50.0f), Onyx::ToRadians(230.0f));
    //    drawList->PathLineTo(handleBottomLeft);
    //    drawList->PathLineTo(handleBottomLeft + ImVec2(0, -handleHeight));
    //    drawList->PathLineTo(handleBottomLeft + ImVec2(handleLength, -handleHeight - 5.0f));
    //    drawList->PathStroke(0xFF00FF00, ImDrawFlags_None, thickness);
    //}

    //// Simple 2D pseudo-Perlin noise function
    //float Noise2D(float x, float y)
    //{
    //    int n = (int)x + (int)y * 57;   
    //    n = (n << 13) ^ n;
    //    int nn = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
    //    return 1.0f - ((float)nn / 1073741824.0f); // range [-1, 1]
    //}


    //// Linear interpolation
    //float Lerp(float a, float b, float t) { return a + t * (b - a); }

    //// Smooth noise sampling
    //float SmoothNoise(float x, float y)
    //{
    //    int ix = (int)x;
    //    int iy = (int)y;
    //    float fx = x - ix;
    //    float fy = y - iy;

    //    float n00 = Noise2D((float)ix, (float)iy);
    //    float n10 = Noise2D((float)ix + 1, (float)iy);
    //    float n01 = Noise2D((float)ix, (float)iy + 1);
    //    float n11 = Noise2D((float)ix + 1, (float)iy + 1);

    //    float nx0 = Lerp(n00, n10, fx);
    //    float nx1 = Lerp(n01, n11, fx);
    //    return Lerp(nx0, nx1, fy);
    //}


    //// 4. Noise/Randomize Terrain
    //void DrawNoiseIcon(ImVec2 pos, float size, ImU32 col)
    //{
    //    ImDrawList* dl = ImGui::GetWindowDrawList();

    //    const int resolution = 32; // number of points per row/col
    //    const float step = size / resolution;
    //    const float dotSize = step * 0.6f;

    //    for (int y = 0; y < resolution; y++)
    //    {
    //        for (int x = 0; x < resolution; x++)
    //        {
    //            float fx = x * step + step * 0.5f;
    //            float fy = y * step + step * 0.5f;

    //            // smooth noise sample
    //            float n = SmoothNoise(x * 0.15f, y * 0.15f); // lower frequency for smoothness
    //            n = (n + 1.0f) * 0.5f; // normalize [0,1]

    //            // vary alpha based on noise
    //            ImU32 colAlpha = IM_COL32(
    //                (col >> 0) & 0xFF,
    //                (col >> 8) & 0xFF,
    //                (col >> 16) & 0xFF,
    //                (int)(n * 255)
    //            );

    //            ImVec2 p = ImVec2(pos.x + fx, pos.y + fy);
    //            dl->AddRectFilled(
    //                ImVec2(p.x - dotSize * 0.5f, p.y - dotSize * 0.5f),
    //                ImVec2(p.x + dotSize * 0.5f, p.y + dotSize * 0.5f),
    //                colAlpha
    //            );
    //        }
    //    }
    //}

    //void DrawStampIcon(ImVec2 topLeft, float size, ImU32 col)
    //{
    //    ImDrawList* dl = ImGui::GetWindowDrawList();
    //    float w = size;
    //    float h = size;

    //    ImVec2 center(topLeft.x + w * 0.5f, topLeft.y + h * 0.5f);

    //    // --- base (wide rectangle at bottom) ---
    //    float baseH = h * 0.22f;
    //    ImVec2 baseMin(center.x - w * 0.4f, topLeft.y + h - baseH);
    //    ImVec2 baseMax(center.x + w * 0.4f, topLeft.y + h - 1.0f);
    //    dl->AddRectFilled(baseMin, baseMax, col, 2.0f);

    //    // --- neck (slightly wider than stem, under handle) ---
    //    float neckH = h * 0.12f;
    //    ImVec2 neckMin(center.x - w * 0.18f, baseMin.y - neckH);
    //    ImVec2 neckMax(center.x + w * 0.18f, baseMin.y);
    //    dl->AddRectFilled(neckMin, neckMax, col, 2.0f);

    //    // --- stem (connector) ---
    //    ImVec2 stemMin(center.x - w * 0.08f, neckMin.y - h * 0.20f);
    //    ImVec2 stemMax(center.x + w * 0.08f, neckMin.y);
    //    dl->AddRectFilled(stemMin, stemMax, col);

    //    // --- handle (circle on top) ---
    //    float knobR = w * 0.22f;
    //    ImVec2 knobCenter(center.x, stemMin.y - knobR * 0.2f);
    //    dl->AddCircleFilled(knobCenter, knobR, col, 16);

    //    // Optional: outline for readability
    //    dl->AddRect(baseMin, baseMax, IM_COL32(0, 0, 0, 160), 2.0f, 0, 1.0f);
    //    dl->AddCircle(knobCenter, knobR, IM_COL32(0, 0, 0, 160), 16, 1.0f);
    //}
}

namespace Onyx::Editor::SceneEditor
{
    TerrainPanel::TerrainPanel(Input::InputActionSystem& actionSystem, Graphics::GraphicsSystem& graphicsSystem, GameCore::GameCoreSystem& gameCore)
        : m_GraphicsSystem(graphicsSystem)
        , m_GameCore(gameCore)
    {
        actionSystem.OnInput<&TerrainPanel::OnTerrainPanelBrushSizeInput>("TerrainBrushScale"_id64, this);

        // Should be transient buffers
        Graphics::BufferProperties ssboBufferProps;
        ssboBufferProps.m_DebugName = "Terrain Brush Hit";
        ssboBufferProps.m_Size = sizeof(Vector3f32) + sizeof(onyxU32);
        ssboBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress | Graphics::BufferUsage::Conditional);
        ssboBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;
        ssboBufferProps.m_IsWritable = true;
        graphicsSystem.CreateBuffer(m_HitBuffer, ssboBufferProps);

        ssboBufferProps.m_DebugName = "Terrain Brush Hit Readback";
        ssboBufferProps.m_Size = sizeof(Vector3f32) + sizeof(onyxU32);
        ssboBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage);
        ssboBufferProps.m_GpuAccess = Graphics::GPUAccess::Staging;
        //ssboBufferProps.m_GpuAccess = Graphics::GPUAccess::Read;
        ssboBufferProps.m_IsWritable = true;
        graphicsSystem.CreateBuffer(m_HitReadbackBuffer, ssboBufferProps);

        ssboBufferProps.m_DebugName = "Split Update Request";
        ssboBufferProps.m_Size = sizeof(onyxU64) + 2 * sizeof(onyxU32);
        ssboBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
        ssboBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;
        ssboBufferProps.m_IsWritable = true;
        graphicsSystem.CreateBuffer(m_SplitRequestsBuffer, ssboBufferProps);

        ssboBufferProps.m_DebugName = "Collapse Update Request";
        ssboBufferProps.m_Size = sizeof(onyxU64) + 2 * sizeof(onyxU32);
        ssboBufferProps.m_UsageFlags = static_cast<onyxU8>(Graphics::BufferUsage::Storage | Graphics::BufferUsage::DeviceAddress);
        ssboBufferProps.m_GpuAccess = Graphics::GPUAccess::Write;
        ssboBufferProps.m_IsWritable = true;
        graphicsSystem.CreateBuffer(m_CollapseRequestsBuffer, ssboBufferProps);

        m_Tools.push_back(MakeUnique<SculptTerrainTool>(graphicsSystem));
        m_Tools.push_back(MakeUnique<PrimitivesTerrainTool>(graphicsSystem));
    }

    TerrainPanel::~TerrainPanel() = default;

    void TerrainPanel::Render(GameCore::Scene& scene)
    {
        ImGuiWindow* sceneViewWindow = ImGui::FindWindowByName("Scene###SceneViewPanel0");
        if (sceneViewWindow == nullptr)
        {
            return;
        }
        
        RenderToolbar(sceneViewWindow);

        bool isSceneViewFocused = sceneViewWindow == GImGui->NavWindow;
        bool isUsingAnyGizmo = ImGuizmo::IsUsingAny();
        bool isHoveringGizmo = ImGuizmo::IsOver();

        Reference<Graphics::RenderGraph>& renderGraph = m_GraphicsSystem.GetRenderGraph();
        Graphics::RenderGraphResourceCache& renderGraphResourceCache = renderGraph->GetResourceCache();

        if (isSceneViewFocused == false || isUsingAnyGizmo || isHoveringGizmo)
        {
            renderGraphResourceCache.erase(HIT_BUFFER_RESOURCE_ID);
            return;
        }

        Entity::EntityRegistry& registry = scene.GetRegistry();
        auto runtimeComponentsView = registry.GetView<Volume::TerrainSettingsComponent, Volume::TerrainWorldOctreeComponent, const Volume::VolumeGenerationComponent>();

        if (runtimeComponentsView.begin() == runtimeComponentsView.end())
            return;

        const Entity::EntityId terrainEntity = runtimeComponentsView.front();
        const Volume::TerrainSettingsComponent& terrainSettings = runtimeComponentsView.get<Volume::TerrainSettingsComponent>(terrainEntity);
        Volume::TerrainWorldOctreeComponent& terrainOctree = runtimeComponentsView.get<Volume::TerrainWorldOctreeComponent>(terrainEntity);
        const Volume::VolumeGenerationComponent& volumeGenerationComponent = runtimeComponentsView.get<const Volume::VolumeGenerationComponent>(terrainEntity);

        if (terrainOctree.OctreeGpuBuffer == false)
        {
            return;
        }

        auto& hitBufferResource = renderGraphResourceCache[HIT_BUFFER_RESOURCE_ID];
        hitBufferResource.Info.Id = HIT_BUFFER_RESOURCE_ID;
        hitBufferResource.Info.Name = "terrain hit";
        
        hitBufferResource.Info.Type = Graphics::RenderGraphResourceType::Buffer;
        hitBufferResource.Handle = m_HitBuffer;

        Rect2f32 sceneViewport{ sceneViewWindow->Pos.x, sceneViewWindow->Pos.y, sceneViewWindow->Viewport->Size.x,sceneViewWindow->Viewport->Size.y };

        Graphics::CommandBuffer& computeCommandBuffer = m_GraphicsSystem.GetCommandBuffer(m_GraphicsSystem.GetFrameIndex(), true);
        TraceTerrain(computeCommandBuffer, terrainOctree, volumeGenerationComponent, sceneViewport);

        bool hasClickedLeft = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
        if (hasClickedLeft)
        {
            Graphics::ConditionalRender conditional(computeCommandBuffer, m_HitBuffer, sizeof(Vector3f32));
            
            {
                computeCommandBuffer.Barrier(m_HitBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead);
                ReadbackTerrainHit(computeCommandBuffer);
                m_Tools[m_SelectedTab]->ApplyOperation(computeCommandBuffer, m_HitBuffer, terrainOctree);
            }
            //  after applying the operation we find the affected octree nodes and re-generate the terrain
            FindWorldOctreeNode(computeCommandBuffer, terrainSettings, terrainOctree, volumeGenerationComponent);
            // generate
            UpdateTerrainMesh(computeCommandBuffer, terrainSettings, terrainOctree);
        }

        if (void* data = m_HitReadbackBuffer.Buffer->Map(Graphics::MapMode::Read))
        {
            struct HitData
            {
                Vector3f32 HitPositon;
                bool HasHit;
            };

            HitData* hitData = static_cast<HitData*>(data);
            if (hitData->HasHit)
            {
                hitData->HasHit = false;
                m_Tools[m_SelectedTab]->OnHitPositionReadback(scene, m_GameCore.GetComponentFactory(), hitData->HitPositon);
                registry.AddComponent<Volume::Terrain::InitTerrainFlag>(runtimeComponentsView.front());
            }

        }

        m_HitReadbackBuffer.Buffer->Unmap();
    }

    void TerrainPanel::RenderTabs()
    {
        onyxU32 index = 0;
        for (const UniquePtr<TerrainTool>& tool : m_Tools)
        {
            
            //DrawSculptIcon(ImGui::GetCursorScreenPos(), 64.0f, 1.5f, 0xFF);
            //ImGui::InvisibleButton("##", ImVec2(64.0f, 64.0f));
            if (ImGui::Button(tool->GetTitle().data()))
            {
                m_SelectedTab = index;
            }
            ++index;
        }
    }

    void TerrainPanel::RenderToolbar(ImGuiWindow* sceneViewWindow)
    {
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize;

        ImGui::SetNextWindowClass(&sceneViewWindow->WindowClass);
        ImGui::SetNextWindowSize(ImVec2(700, 300));
        bool isVisible = ImGui::Begin("Terrain Toolbar", nullptr, flags);
        if (isVisible)
        {
            Ui::ScopedImGuiColor styleColor
            {
                { ImGuiCol_TableRowBg, 0x0 },
                { ImGuiCol_NavCursor, 0x0 }
            };

            bool focusSceneView = false;

            if (ImGui::BeginTable("##scene", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                RenderTabs();

                ImGui::TableNextColumn();
                if (m_Tools.empty() == false)
                {
                    m_Tools[m_SelectedTab]->Render();
                }

                ImGui::EndTable();
            }

            if (focusSceneView)
            {
                ImGui::FocusWindow(sceneViewWindow);
            }
        }

        //if (m_IsDocked == false)
        {
            ImGui::End();
        }
    }

    void TerrainPanel::TraceTerrain(Graphics::CommandBuffer& computeCommandBuffer, Volume::TerrainWorldOctreeComponent& terrainOctree, const Volume::VolumeGenerationComponent& volumeGenerationComponent, Rect2f32 sceneViewport)
    {
        struct RayTraceTerrainPushConstants
        {
            Vector2f32 MousePosition;
            onyxU64 ViewConstantsAddress;

            onyxU64 VolumeSourcesList;
            onyxU64 VolumeSourcesData;

            onyxU64 HitBufferAddress;
        };

        ImVec2 mousePos = ImGui::GetMousePos();
        RayTraceTerrainPushConstants constants;

        constants.MousePosition[0] = ((mousePos.x - sceneViewport.Position[0]) / sceneViewport.Extents[0]) * 2.0f - 1.0f;
        constants.MousePosition[1] = (((mousePos.y - sceneViewport.Position[1]) / sceneViewport.Extents[1]) * -2.0f + 1.0f);
        constants.ViewConstantsAddress = m_GraphicsSystem.GetViewConstantsBuffer().GetGpuAddress();
        constants.HitBufferAddress = m_HitBuffer.GetGpuAddress();
        constants.VolumeSourcesList = terrainOctree.VolumeObjects.GetGpuAddress();
        constants.VolumeSourcesData = terrainOctree.VolumeObjectsData.GetGpuAddress();

        computeCommandBuffer.Barrier(m_HitBuffer, Graphics::Context::Compute, Graphics::Access::ShaderWrite);
        computeCommandBuffer.Barrier(terrainOctree.VolumeObjects, Graphics::Context::Compute, Graphics::Access::ShaderRead);
        computeCommandBuffer.Barrier(terrainOctree.VolumeObjectsData, Graphics::Context::Compute, Graphics::Access::ShaderRead);
        computeCommandBuffer.BindShaderEffect(volumeGenerationComponent.RayTraceTerrainShader);
        computeCommandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, constants);
        computeCommandBuffer.Dispatch(1, 1, 1);
        computeCommandBuffer.Barrier(m_HitBuffer, Graphics::Context::Compute, Graphics::Access::ShaderRead | Graphics::Access::IndirectRead);
    }

    void TerrainPanel::ReadbackTerrainHit(Graphics::CommandBuffer& computeCommandBuffer)
    {
        computeCommandBuffer.Copy(m_HitBuffer, m_HitReadbackBuffer);
    }

    void TerrainPanel::FindWorldOctreeNode(Graphics::CommandBuffer& computeCommandBuffer, const Volume::TerrainSettingsComponent& terrainSettings, Volume::TerrainWorldOctreeComponent& terrainOctree, const Volume::VolumeGenerationComponent& volumeGenerationComponent)
    {
        struct FindOctreeNodePushConstants
        {
            onyxU64 OctreeBufferAddress;
            onyxF32 RootHalfExtents;
            onyxU32 MaxDepth;

            onyxU64 VolumeSourcesList;
            onyxU64 VolumeSourcesData;

            onyxU64 QueuedCollapseBuffer;
            onyxU64 QueuedSplitBuffer;

            onyxU64 HitBufferAddress;
            onyxF32 BrushSizeSquared;
            onyxF32 MaxGeometricError;

            onyxF32 ComplexSurfaceThreshold;
            onyxF32 Padding[3];
        };

        computeCommandBuffer.GlobalBarrier(0, 0x00000020 | 0x00000040);
        FindOctreeNodePushConstants findOctreeNodeConstants;
        findOctreeNodeConstants.OctreeBufferAddress = terrainOctree.OctreeGpuBuffer.GetGpuAddress();
        findOctreeNodeConstants.RootHalfExtents = terrainOctree.RootSize * 0.5f;
        findOctreeNodeConstants.MaxDepth = terrainOctree.ChunkMaxDepth;

        findOctreeNodeConstants.VolumeSourcesList = terrainOctree.VolumeObjects.GetGpuAddress();
        findOctreeNodeConstants.VolumeSourcesData = terrainOctree.VolumeObjectsData.GetGpuAddress();

        findOctreeNodeConstants.HitBufferAddress = m_HitBuffer.GetGpuAddress();
        findOctreeNodeConstants.QueuedCollapseBuffer = m_CollapseRequestsBuffer.GetGpuAddress();
        findOctreeNodeConstants.QueuedSplitBuffer= m_SplitRequestsBuffer.GetGpuAddress();

        onyxF32 bounds = m_Tools[m_SelectedTab]->GetBounds();
        findOctreeNodeConstants.BrushSizeSquared = bounds * bounds;

        findOctreeNodeConstants.MaxGeometricError = terrainSettings.MaxGeometricError;
        findOctreeNodeConstants.ComplexSurfaceThreshold = terrainSettings.ComplexSurfaceThreshold;

        computeCommandBuffer.Barrier(terrainOctree.VolumeObjects, Graphics::Context::Compute, Graphics::Access::ShaderRead);
        computeCommandBuffer.Barrier(terrainOctree.VolumeObjectsData, Graphics::Context::Compute, Graphics::Access::ShaderRead);
        computeCommandBuffer.BindShaderEffect(volumeGenerationComponent.FindRayTracedOctreeNodeShader);
        computeCommandBuffer.BindPushConstants(Graphics::ShaderStage::Compute, 0, findOctreeNodeConstants);
        computeCommandBuffer.Dispatch(1, 1, 1);
    }

    void TerrainPanel::UpdateTerrainMesh(const Graphics::CommandBuffer& commandBuffer, const Volume::TerrainSettingsComponent& terrainSettings, Volume::TerrainWorldOctreeComponent& terrainOctree)
    {
        ONYX_UNUSED(commandBuffer);
        ONYX_UNUSED(terrainSettings);
        ONYX_UNUSED(terrainOctree);
    }

    void TerrainPanel::OnTerrainPanelBrushSizeInput(const Input::InputActionEvent& inputEvent)
    {
        onyxF32 inputValue = inputEvent.GetData<onyxF32>();
        m_Tools[m_SelectedTab]->OnBrushSizeInput(inputValue);
    }
}
