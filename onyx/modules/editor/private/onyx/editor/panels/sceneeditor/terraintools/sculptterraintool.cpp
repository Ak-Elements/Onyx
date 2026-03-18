#include <onyx/editor/panels/sceneeditor/terraintools/sculptterraintool.h>

#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/volume/components/volumeterraincomponent.gen.h>
#include <onyx/volume/graphics/previewterrainedit.h>

#include <imgui_extra_math.h>

namespace onyx::editor
{
    namespace
    {
        void DrawSmoothArrow(const ImVec2& from, const ImVec2& to, float arrow_size = 12.0f, float thickness = 4.0f, bool roundShaft = true, ImU32 color = IM_COL32(255, 255, 255, 255))
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();

            ImVec2 dir = to - from;
            float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
            if (len < 0.001f) return;

            dir.x /= len;
            dir.y /= len;
            ImVec2 perp(-dir.y, dir.x);

            ImVec2 tip_base = to - dir * arrow_size;
            ImVec2 left = tip_base + perp * (arrow_size * 0.8f);
            ImVec2 right = tip_base - perp * (arrow_size * 0.8f);

            float radius = thickness * 0.5f;

            // ---- Draw shaft ----
            draw_list->PathClear();
            //draw_list->PathArcTo(p1, radius, IM_PI, 2 * IM_PI, 8);
            draw_list->PathLineTo(from);
            draw_list->PathLineTo(to - dir * thickness * 0.5f);
            draw_list->PathStroke(color, false, thickness); // false = open path
            // rounded start cap
            if (roundShaft)
                draw_list->AddCircleFilled(from, radius, color);

            // ---- Draw arrowhead ----
            draw_list->PathClear();
            draw_list->PathLineTo(left);
            draw_list->PathLineTo(to - dir * thickness * 0.5f);
            draw_list->PathLineTo(right);
            draw_list->PathStroke(color, ImDrawFlags_None, thickness);

            draw_list->AddCircleFilled(left, radius, color);
            draw_list->AddCircleFilled(right, radius, color);
        }

        // Helper to draw dashed line along a cubic Bezier curve
        void AddDashedCubicBezier(
            ImDrawList* dl,
            const ImVec2& p0, const ImVec2& p1,
            const ImVec2& p2, const ImVec2& p3,
            ImU32 col, float thickness,
            float dash_len = 10.0f, float gap_len = 5.0f,
            int num_segments = 64)
        {
            if ((col & IM_COL32_A_MASK) == 0 || dl == nullptr)
                return;

            //float total_len = dash_len + gap_len;
            float t_step = 1.0f / num_segments;
            ImVec2 prev = p0;
            float dist_accum = 0.0f;
            bool drawing = true;

            for (int i = 1; i <= num_segments; i++)
            {
                float t = t_step * i;
                ImVec2 p = ImBezierCubicCalc(p0, p1, p2, p3, t);

                // segment length
                float seg_len = ImLength(p - prev);

                float remaining = seg_len;
                ImVec2 seg_start = prev;

                while (remaining > 0.0f)
                {
                    float space_left = (drawing ? dash_len : gap_len) - dist_accum;

                    float step = ImMin(remaining, space_left);
                    ImVec2 seg_end = seg_start + (p - seg_start) * (step / seg_len);

                    if (drawing)
                        dl->AddLine(seg_start, seg_end, col, thickness);

                    dist_accum += step;
                    if (dist_accum >= (drawing ? dash_len : gap_len))
                    {
                        drawing = !drawing;
                        dist_accum = 0.0f;
                    }

                    seg_start = seg_end;
                    remaining -= step;
                }

                prev = p;
            }
        }

        void DrawRaiseIcon(ImVec2 pos, float size, float thickness, ImU32 col)
        {
            ImDrawList* dl = ImGui::GetWindowDrawList();

            dl->AddRect(pos, pos + ImVec2(size, size), col, 2.0f, ImDrawFlags_RoundCornersAll, thickness);

            float margin = std::ceil(size * 0.2f);

            // --- Icon proportions relative to size ---
            //float totalW = size - 2 * margin;
            float totalAvailableHeight = size - 2 * margin;
            float hillH = std::ceil(totalAvailableHeight * 0.3f);
            float arrowH = std::ceil(totalAvailableHeight * 0.4f);

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
            dl->PathLineTo(ImVec2(hillL, baseY));
            dl->PathBezierCubicCurveTo(
                ImVec2(hillL + (midX - hillL) * 0.6f, baseY),
                ImVec2(midX - (midX - hillL) * 0.6f, peakY),
                ImVec2(midX, peakY)
            );
            dl->PathBezierCubicCurveTo(
                ImVec2(midX + (hillR - midX) * 0.6f, peakY),
                ImVec2(hillR - (hillR - midX) * 0.6f, baseY),
                ImVec2(hillR, baseY)
            );
            dl->PathStroke(col, ImDrawListFlags_None, thickness);

            // --- arrow above hill peak ---
            float arrowSize = totalAvailableHeight * 0.2f;
            float arrowX = centerX;
            float arrowToY = pos.y + margin + thickness * 0.5f;
            float arrowFromY = arrowToY + arrowH;
            DrawSmoothArrow(ImVec2(arrowX, arrowFromY), ImVec2(arrowX, arrowToY), arrowSize, thickness, false, col);
        }

        void DrawLowerIcon(ImVec2 pos, float size, float thickness, ImU32 col)
        {
            ImDrawList* dl = ImGui::GetWindowDrawList();

            dl->AddRect(pos, pos + ImVec2(size, size), col, 2.0f, ImDrawFlags_RoundCornersAll, thickness);

            float margin = std::ceil(size * 0.2f);

            // --- Icon proportions relative to size ---
            //float totalW = size - 2 * margin;
            float totalAvailableHeight = size - 2 * margin;
            float hillH = std::ceil(totalAvailableHeight * 0.3f);
            float arrowH = std::ceil(totalAvailableHeight * 0.4f);

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
            dl->PathLineTo(ImVec2(hillL, baseY));
            dl->PathBezierCubicCurveTo(
                ImVec2(hillL + (midX - hillL) * 0.6f, baseY),
                ImVec2(midX - (midX - hillL) * 0.6f, peakY),
                ImVec2(midX, peakY)
            );
            dl->PathBezierCubicCurveTo(
                ImVec2(midX + (hillR - midX) * 0.6f, peakY),
                ImVec2(hillR - (hillR - midX) * 0.6f, baseY),
                ImVec2(hillR, baseY)
            );
            dl->PathStroke(col, ImDrawListFlags_None, thickness);

            // --- arrow above hill peak ---
            float arrowSize = totalAvailableHeight * 0.2f;
            float arrowX = centerX;
            float arrowFromY = pos.y + margin + thickness * 0.5f;
            float arrowToY = arrowFromY + arrowH;
            DrawSmoothArrow(ImVec2(arrowX, arrowFromY), ImVec2(arrowX, arrowToY), arrowSize, thickness, true, col);
        }

        // 2. Flatten Terrain
        void DrawFlattenIcon(ImVec2 pos, float size, float thickness, ImU32 col)
        {
            ImDrawList* dl = ImGui::GetWindowDrawList();

            dl->AddRect(pos, pos + ImVec2(size, size), col, 2.0f, ImDrawFlags_RoundCornersAll, thickness);

            float margin = std::ceil(size * 0.2f);
            float halfThickness = thickness * 0.5f;

            // --- Icon proportions relative to size ---
            float totalAvailableWidth = size - 2 * margin;
            float totalAvailableHeight = size - 2 * margin;
            //float hillH = std::ceil(totalAvailableHeight * 0.25f);
            float arrowH = std::ceil(totalAvailableHeight * 0.4f);

            // --- center combined block inside square ---
            float contentLeft = pos.x + margin;

            // position base terrain for this close to the hill peak of other icons
            float baseY = pos.y + size - margin - thickness - std::ceil(totalAvailableHeight * 0.1f);

            // House proportions
            float flatTerrainLineLengthLeft = totalAvailableWidth * 0.05f;
            float houseW = std::ceil(totalAvailableWidth * 0.3f);
            float houseH = std::ceil(totalAvailableHeight * 0.25f);
            float houseX = contentLeft + flatTerrainLineLengthLeft;

            float houseY = baseY - houseH;

            // --- Terrain line with small bump ---
            dl->PathClear();
            // Left flat section
            dl->PathLineTo(ImVec2(contentLeft, baseY));

            dl->PathLineTo(ImVec2(houseX, baseY));

            // Under the house: keep flat
            dl->PathLineTo(ImVec2(houseX + houseW, baseY));

            // Smooth bump to the right
            float bumpStart = houseX + houseW + std::ceil(totalAvailableHeight * 0.05f);
            float bumpEnd = bumpStart + std::ceil(totalAvailableHeight * 0.1f);
            float bumpPeakY = baseY - totalAvailableHeight * 0.05f;
            float bumpMid = std::ceil((bumpStart + bumpEnd) * 0.5f);

            dl->PathBezierCubicCurveTo(
                ImVec2(bumpStart + (bumpMid - bumpStart) * 0.6f, baseY),
                ImVec2(bumpMid - (bumpMid - bumpStart) * 0.6f, bumpPeakY),
                ImVec2(bumpMid, bumpPeakY)
            );
            dl->PathBezierCubicCurveTo(
                ImVec2(bumpMid + (bumpEnd - bumpMid) * 0.6f, bumpPeakY),
                ImVec2(bumpEnd - (bumpEnd - bumpMid) * 0.6f, baseY),
                ImVec2(bumpEnd, baseY)
            );

            // Continue to far right flat
            float flatTerrainRightEnd = pos.x + margin + totalAvailableWidth * 0.9f;
            dl->PathLineTo(ImVec2(flatTerrainRightEnd, baseY));

            // Smooth bump at the end
            bumpStart = contentLeft + totalAvailableWidth * 0.9f;
            bumpEnd = contentLeft + totalAvailableWidth;
            bumpMid = (bumpStart + bumpEnd) * 0.5f;
            bumpPeakY = baseY - totalAvailableHeight * 0.05f;

            dl->PathBezierCubicCurveTo(
                ImVec2(bumpStart + (bumpMid - bumpStart) * 0.6f, baseY),
                ImVec2(bumpMid - (bumpMid - bumpStart) * 0.6f, bumpPeakY),
                ImVec2(bumpMid, bumpPeakY)
            );

            dl->PathStroke(col, false, thickness);

            // --- House shape ---
            float roofHeight = houseH * 0.4f;
            float rounding = totalAvailableWidth * 0.05f;

            ImVec2 bl = ImVec2(houseX, baseY);                     // bottom-left
            ImVec2 tl = ImVec2(houseX, houseY);                    // top-left wall

            //ImVec2 tlRoof = ImVec2(houseX , houseY);                    // top-left wall
            ImVec2 tr = ImVec2(houseX + houseW, houseY);           // top-right wall
            ImVec2 br = ImVec2(houseX + houseW, baseY);            // bottom-right
            float ridgeWidth = houseW * 0.6f;
            ImVec2 tlRoof = ImVec2(houseX + (houseW - ridgeWidth) * 0.5f, houseY - roofHeight);
            ImVec2 trRoof = ImVec2(tlRoof.x + ridgeWidth, houseY - roofHeight);
            dl->PathClear();

            dl->PathLineTo(bl);
            dl->PathArcTo(tl + ImVec2(rounding, rounding), rounding, IM_PI, IM_PI + IM_PI / 4.0f);
            dl->PathArcTo(tlRoof + ImVec2(rounding, rounding), rounding, IM_PI + IM_PI / 4.0f, IM_PI + IM_PI / 2.0f);
            dl->PathArcTo(trRoof + ImVec2(-rounding, rounding), rounding, IM_PI + IM_PI / 2.0f, IM_PI + IM_PI * 0.75f);
            dl->PathArcTo(tr + ImVec2(-rounding, rounding), rounding, IM_PI + IM_PI * 0.75f, IM_PI + IM_PI);
            dl->PathLineTo(br);

            dl->PathStroke(col, ImDrawFlags_None, thickness);

            float flatTerrainRightStart = houseX + houseW + std::ceil(totalAvailableHeight * 0.05f);
            flatTerrainRightEnd = bumpMid;

            float flatTerrainRightCenter = flatTerrainRightStart + (flatTerrainRightEnd - flatTerrainRightStart) * 0.5f;
            float peakY = pos.y + (pos.y + size - baseY);
            float hillBaseY = baseY - totalAvailableHeight * 0.05f;

            // draw dashed hill background
            float dashLength = 2.0f;
            float gap = 1.0f;
            onyx::onyxU32 dashedColor = (col & 0x00FFFFFF) | (128 << IM_COL32_A_SHIFT);
            dl->PathClear();

            AddDashedCubicBezier(dl,
                ImVec2(flatTerrainRightStart, hillBaseY),
                ImVec2(flatTerrainRightStart, hillBaseY),
                ImVec2(flatTerrainRightCenter - (flatTerrainRightCenter - flatTerrainRightStart) * 0.8f, peakY),
                ImVec2(flatTerrainRightCenter, peakY),
                dashedColor, halfThickness, dashLength, gap);

            AddDashedCubicBezier(dl,
                ImVec2(flatTerrainRightCenter, peakY),
                ImVec2(flatTerrainRightCenter + (flatTerrainRightEnd - flatTerrainRightCenter) * 0.8f, peakY),
                ImVec2(flatTerrainRightEnd, hillBaseY),
                ImVec2(flatTerrainRightEnd, hillBaseY),
                dashedColor, halfThickness, dashLength, gap);

            dl->PathClear();

            // --- arrow ---
            float arrowSize = totalAvailableHeight * 0.2f;
            float arrowX = flatTerrainRightStart + (flatTerrainRightEnd - flatTerrainRightStart) * 0.5f;
            float arrowToY = baseY - std::ceil(totalAvailableHeight * 0.1f);
            float arrowFromY = arrowToY - arrowH;
            DrawSmoothArrow(ImVec2(arrowX, arrowFromY), ImVec2(arrowX, arrowToY), arrowSize, thickness, false, col);
        }

        // 3. Smooth/Soften Terrain
        void DrawSmoothIcon(ImVec2 pos, float size, float thickness, ImU32 col)
        {
            ImDrawList* dl = ImGui::GetWindowDrawList();
            dl->AddRect(pos, pos + ImVec2(size, size), col, 2.0f, ImDrawFlags_RoundCornersAll, thickness);

            float margin = std::ceil(size * 0.2f);
            float halfThickness = thickness * 0.5f;

            // --- Icon proportions relative to size ---
            float totalAvailableHeight = size - 2 * margin;
            float hillH = std::ceil(totalAvailableHeight * 0.25f);
            float arrowH = std::ceil(totalAvailableHeight * 0.2f);

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
            float hillBaseY = baseY - std::ceil(totalAvailableHeight * 0.05f);
            // --- draw hill ---
            dl->PathClear();
            dl->PathLineTo(ImVec2(hillL, hillBaseY));
            dl->PathBezierCubicCurveTo(
                ImVec2(hillL + (midX - hillL) * 0.6f, hillBaseY),
                ImVec2(midX - (midX - hillL) * 0.6f, peakY),
                ImVec2(midX, peakY)
            );
            dl->PathBezierCubicCurveTo(
                ImVec2(midX + (hillR - midX) * 0.6f, peakY),
                ImVec2(hillR - (hillR - midX) * 0.6f, hillBaseY),
                ImVec2(hillR, hillBaseY)
            );
            dl->PathStroke(col, ImDrawListFlags_None, thickness);

            // draw dashed hill background
            float dashedHillLeftStart = pos.x + margin + halfThickness;
            float dashedHillLeftEnd = centerX;
            float dashedHillLeftMid = dashedHillLeftStart + (dashedHillLeftEnd - dashedHillLeftStart) * 0.5f;
            peakY = baseY - std::ceil(totalAvailableHeight * 0.3f);

            float dashLength = 2.0f;
            float gap = 1.0f;

            onyx::onyxU32 dashedColor = (col & 0x00FFFFFF) | (128 << IM_COL32_A_SHIFT);
            dl->PathClear();
            AddDashedCubicBezier(dl,
                ImVec2(dashedHillLeftStart, baseY),
                ImVec2(dashedHillLeftStart + (dashedHillLeftMid - dashedHillLeftStart) * 0.6f, baseY),
                ImVec2(dashedHillLeftMid - (dashedHillLeftMid - dashedHillLeftStart) * 0.6f, peakY),
                ImVec2(dashedHillLeftMid, peakY),
                dashedColor, halfThickness, dashLength, gap);

            AddDashedCubicBezier(dl,
                ImVec2(dashedHillLeftMid, peakY),
                ImVec2(dashedHillLeftMid + (dashedHillLeftEnd - dashedHillLeftMid) * 0.6f, peakY),
                ImVec2(dashedHillLeftEnd - (dashedHillLeftEnd - dashedHillLeftMid) * 0.6f, baseY),
                ImVec2(dashedHillLeftEnd, baseY),
                dashedColor, halfThickness, dashLength, gap);

            dashedHillLeftStart = dashedHillLeftEnd;
            dashedHillLeftEnd = pos.x + size - margin - halfThickness;
            dashedHillLeftMid = dashedHillLeftStart + (dashedHillLeftEnd - dashedHillLeftStart) * 0.5f;

            AddDashedCubicBezier(dl,
                ImVec2(dashedHillLeftStart, baseY),
                ImVec2(dashedHillLeftStart + (dashedHillLeftMid - dashedHillLeftStart) * 0.6f, baseY),
                ImVec2(dashedHillLeftMid - (dashedHillLeftMid - dashedHillLeftStart) * 0.6f, peakY),
                ImVec2(dashedHillLeftMid, peakY),
                dashedColor, halfThickness, dashLength, gap);

            AddDashedCubicBezier(dl,
                ImVec2(dashedHillLeftMid, peakY),
                ImVec2(dashedHillLeftMid + (dashedHillLeftEnd - dashedHillLeftMid) * 0.6f, peakY),
                ImVec2(dashedHillLeftEnd - (dashedHillLeftEnd - dashedHillLeftMid) * 0.6f, baseY),
                ImVec2(dashedHillLeftEnd, baseY),
                dashedColor, halfThickness, dashLength, gap);

            dl->PathClear();


            // Arrows pointing sideways (left + right at baseline)
            float arrowY = pos.y + arrowH + margin;

            float leftArrowX = pos.x + margin + thickness;
            float rightArrowX = pos.x + size - margin - thickness;
            DrawSmoothArrow(ImVec2(centerX, arrowY), ImVec2(leftArrowX, arrowY), arrowH, thickness, false, col);
            DrawSmoothArrow(ImVec2(centerX, arrowY), ImVec2(rightArrowX, arrowY), arrowH, thickness, false, col);
        }

        // Simple 2D pseudo-Perlin noise function
        float Noise2D(float x, float y)
        {
            int n = (int)x + (int)y * 57;
            n = (n << 13) ^ n;
            int nn = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
            return 1.0f - ((float)nn / 1073741824.0f); // range [-1, 1]
        }


        // Linear interpolation
        float Lerp(float a, float b, float t) { return a + t * (b - a); }

        // Smooth noise sampling
        float SmoothNoise(float x, float y)
        {
            int ix = (int)x;
            int iy = (int)y;
            float fx = x - ix;
            float fy = y - iy;

            float n00 = Noise2D((float)ix, (float)iy);
            float n10 = Noise2D((float)ix + 1, (float)iy);
            float n01 = Noise2D((float)ix, (float)iy + 1);
            float n11 = Noise2D((float)ix + 1, (float)iy + 1);

            float nx0 = Lerp(n00, n10, fx);
            float nx1 = Lerp(n01, n11, fx);
            return Lerp(nx0, nx1, fy);
        }


        // 4. Noise/Randomize Terrain
        void DrawNoiseIcon(ImVec2 pos, float size, ImU32 col)
        {
            ImDrawList* dl = ImGui::GetWindowDrawList();

            const int resolution = 32; // number of points per row/col
            const float step = size / resolution;
            const float dotSize = step * 0.6f;

            for (int y = 0; y < resolution; y++)
            {
                for (int x = 0; x < resolution; x++)
                {
                    float fx = x * step + step * 0.5f;
                    float fy = y * step + step * 0.5f;

                    // smooth noise sample
                    float n = SmoothNoise(x * 0.15f, y * 0.15f); // lower frequency for smoothness
                    n = (n + 1.0f) * 0.5f; // normalize [0,1]

                    // vary alpha based on noise
                    ImU32 colAlpha = IM_COL32(
                        (col >> 0) & 0xFF,
                        (col >> 8) & 0xFF,
                        (col >> 16) & 0xFF,
                        (int)(n * 255)
                    );

                    ImVec2 p = ImVec2(pos.x + fx, pos.y + fy);
                    dl->AddRectFilled(
                        ImVec2(p.x - dotSize * 0.5f, p.y - dotSize * 0.5f),
                        ImVec2(p.x + dotSize * 0.5f, p.y + dotSize * 0.5f),
                        colAlpha
                    );
                }
            }
        }

        void DrawStampIcon(ImVec2 topLeft, float size, ImU32 col)
        {
            ImDrawList* dl = ImGui::GetWindowDrawList();
            float w = size;
            float h = size;

            ImVec2 center(topLeft.x + w * 0.5f, topLeft.y + h * 0.5f);

            // --- base (wide rectangle at bottom) ---
            float baseH = h * 0.22f;
            ImVec2 baseMin(center.x - w * 0.4f, topLeft.y + h - baseH);
            ImVec2 baseMax(center.x + w * 0.4f, topLeft.y + h - 1.0f);
            dl->AddRectFilled(baseMin, baseMax, col, 2.0f);

            // --- neck (slightly wider than stem, under handle) ---
            float neckH = h * 0.12f;
            ImVec2 neckMin(center.x - w * 0.18f, baseMin.y - neckH);
            ImVec2 neckMax(center.x + w * 0.18f, baseMin.y);
            dl->AddRectFilled(neckMin, neckMax, col, 2.0f);

            // --- stem (connector) ---
            ImVec2 stemMin(center.x - w * 0.08f, neckMin.y - h * 0.20f);
            ImVec2 stemMax(center.x + w * 0.08f, neckMin.y);
            dl->AddRectFilled(stemMin, stemMax, col);

            // --- handle (circle on top) ---
            float knobR = w * 0.22f;
            ImVec2 knobCenter(center.x, stemMin.y - knobR * 0.2f);
            dl->AddCircleFilled(knobCenter, knobR, col, 16);

            // Optional: outline for readability
            dl->AddRect(baseMin, baseMax, IM_COL32(0, 0, 0, 160), 2.0f, 0, 1.0f);
            dl->AddCircle(knobCenter, knobR, IM_COL32(0, 0, 0, 160), 16, 1.0f);
        }
    }

    SculptTerrainTool::SculptTerrainTool(rhi::GraphicsSystem& graphicsSystem)
        : m_CreateVolumeSourceShader(graphicsSystem.CreateShaderInstance("engine:/shaders/compute/volume/createvolumebrush.oshader"))
    {
    }

    StringView SculptTerrainTool::GetTitle()
    {
        return "Sculpt";
    }

    void SculptTerrainTool::Render()
    {
        onyxF32 iconSize = 64;

        ImGui::BeginHorizontal("##group", ImVec2(0, 0));
        {
            ui::ScopedImGuiStyle style
            {
                { ImGuiStyleVar_FrameBorderSize, 2.0f }
            };

            ui::ScopedImGuiColor colors
            {
                {ImGuiCol_Button, 0xFFAA8877},
                {ImGuiCol_ButtonHovered, 0xFFBB9977},
                {ImGuiCol_ButtonActive, 0xFFEEAA77}
            };
            RenderBrushToolbarButton(SculptType::Raise, iconSize);
            RenderBrushToolbarButton(SculptType::Lower, iconSize);

            // not implemented yet
            //focusSceneView |= RenderBrushToolbarButton(SculptType::Smooth, iconSize);
            //focusSceneView |= RenderBrushToolbarButton(SculptType::Flatten, iconSize);
            //focusSceneView |= RenderBrushToolbarButton(SculptType::Noise, iconSize);
            //focusSceneView |= RenderBrushToolbarButton(SculptType::Stamp, iconSize);

        }
        ImGui::EndHorizontal();

        RenderProperties();

        volume::PreviewTerrainEditPass::BrushSize = m_BrushSize;
        volume::PreviewTerrainEditPass::BrushType = 0;
        volume::PreviewTerrainEditPass::BrushOperation = 0;
    }

    void SculptTerrainTool::ApplyOperation(rhi::CommandBuffer& commandBuffer, const rhi::BufferHandle& hitBuffer, volume::TerrainWorldOctreeComponent& terrainOctree)
    {
        struct CreateVolumeSourcePushConstants
        {
            onyxU64 WorldVolumesList;
            onyxU64 WorldVolumesData;

            onyxU64 HitBufferAddress;
            onyxU32 BrushType;
            onyxU32 BrushOperation;

            Vector3f32 BrushSize;
            onyxF32 Smoothness;
        };

        CreateVolumeSourcePushConstants createVolumeSourceConstants;
        commandBuffer.Barrier(terrainOctree.VolumeObjects, rhi::Context::Compute, rhi::Access::ShaderWrite);
        commandBuffer.Barrier(terrainOctree.VolumeObjectsData, rhi::Context::Compute, rhi::Access::ShaderWrite);
        createVolumeSourceConstants.WorldVolumesList = terrainOctree.VolumeObjects.GetGpuAddress();
        createVolumeSourceConstants.WorldVolumesData = terrainOctree.VolumeObjectsData.GetGpuAddress();
        createVolumeSourceConstants.HitBufferAddress = hitBuffer.GetGpuAddress();
        createVolumeSourceConstants.BrushSize = m_BrushSize;
        createVolumeSourceConstants.BrushType = 5;
        createVolumeSourceConstants.BrushOperation = m_Type == SculptType::Lower ? 1 :  0;
        createVolumeSourceConstants.Smoothness = m_Smoothness;
        commandBuffer.BindShaderEffect(m_CreateVolumeSourceShader);
        commandBuffer.BindPushConstants(rhi::ShaderStage::Compute, 0, createVolumeSourceConstants);
        commandBuffer.Dispatch(1, 1, 1);

        commandBuffer.Barrier(terrainOctree.VolumeObjects, rhi::Context::Compute, rhi::Access::ShaderRead);
        commandBuffer.Barrier(terrainOctree.VolumeObjectsData, rhi::Context::Compute, rhi::Access::ShaderRead);
    }

    void SculptTerrainTool::OnHitPositionReadback(game_core::Scene& /*scene*/, const ecs::ComponentFactory& /*componentFactory*/, const Vector3f32& /*hitPosition*/)
    {
        // Nothing to do for sculpting as we do not create entities;
    }

    void SculptTerrainTool::OnBrushSizeInput(onyxF32 value)
    {
        m_BrushSize += Vector3f32(value);
    }

    void SculptTerrainTool::RenderProperties()
    {
        ImGui::BeginChild("Panel", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);

        ui::property_grid::BeginPropertyGrid("Properties", 80.0f);

        ui::property_grid::DrawProperty("Brush Size", m_BrushSize.X);
        ui::property_grid::DrawProperty("Smoothness", m_Smoothness);

        ui::property_grid::EndPropertyGrid();
        ImGui::EndChild();
    }

    bool SculptTerrainTool::RenderBrushToolbarButton(SculptType type, onyxF32 buttonSize)
    {
        bool isClicked = false;
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();

        if (ImGui::InvisibleButton(enums::ToString(type).data(), ImVec2(buttonSize, buttonSize)))
        {
            m_Type = type;
            isClicked = true;
        }

        onyxF32 thickness = ImGui::GetStyle().FrameBorderSize;
        onyxU32 color = m_Type == type ? ImGui::GetColorU32(ImGuiCol_ButtonActive) : ImGui::IsItemHovered() ? ImGui::GetColorU32(ImGuiCol_ButtonHovered) : ImGui::GetColorU32(ImGuiCol_Button);
        switch (type)
        {
        case SculptType::Raise:
            DrawRaiseIcon(cursorPos, buttonSize, thickness, color);
            break;
        case SculptType::Lower:
            DrawLowerIcon(cursorPos, buttonSize, thickness, color);
            break;
        case SculptType::Flatten:
            DrawFlattenIcon(cursorPos, buttonSize, thickness, color);
            break;
        case SculptType::Smooth:
            DrawSmoothIcon(cursorPos, buttonSize, thickness, color);
            break;
        case SculptType::Noise:
            DrawNoiseIcon(cursorPos, buttonSize, color);
            break;
        case SculptType::Stamp:
            DrawStampIcon(cursorPos, buttonSize, color);
            break;
        case SculptType::None:
            break;
        }

        return isClicked;
    }
}
