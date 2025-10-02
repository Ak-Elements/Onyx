#include <onyx/ui/controls/colorcontrol.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <onyx/platforms/windows/platform.h>
#include <onyx/ui/scopedid.h>
#include <onyx/ui/controls/vectorcontrol.h>

#include <onyx/input/inputsystem.h>
#include <onyx/ui/imguisystem.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <onyx/graphics/window/windows/nativewindow.h>

namespace Onyx::Ui
{
    namespace
    {
        // Convert hsv floats ([0-1],[0-1],[0-1]) to rgb uint , from Foley & van Dam p593
        // also http://en.wikipedia.org/wiki/HSL_and_HSV
        onyxU32 ColorConvertHSVtoBGR(float h, float s, float v)
        {
            auto ConvertToU32 = [](float value) { return static_cast<onyxU32>(value * 255.0f + 0.5f); };

            if (s == 0.0f)
            {
                // gray
                onyxU32 colorValue = ConvertToU32(v);
                return colorValue << 16 | colorValue << 8 | colorValue;
            }

            h = ImFmod(h, 1.0f) / (60.0f / 360.0f);
            int   i = (int)h;
            float f = h - (float)i;
            float p = v * (1.0f - s);
            float q = v * (1.0f - s * f);
            float t = v * (1.0f - s * (1.0f - f));

            switch (i)
            {
            case 0: return (255 << 24) | ConvertToU32(v) | ConvertToU32(t) << 8 | ConvertToU32(p) << 16;
            case 1: return (255 << 24) |ConvertToU32(q) | ConvertToU32(v) << 8 | ConvertToU32(p) << 16;
            case 2: return (255 << 24) |ConvertToU32(p) | ConvertToU32(v) << 8 | ConvertToU32(t) << 16;
            case 3: return (255 << 24) |ConvertToU32(p) | ConvertToU32(q) << 8 | ConvertToU32(v) << 16;
            case 4: return (255 << 24) |ConvertToU32(t) | ConvertToU32(p) << 8 | ConvertToU32(v) << 16;
            case 5: default: return (255 << 24) | ConvertToU32(v) | ConvertToU32(p) << 8 | ConvertToU32(q) << 16;
            }
        }

        template <typename VectorT> requires (IsVector3<VectorT> || IsVector4<VectorT>)
        bool ColorEdit(StringView id, VectorT& outColor, ImGuiColorEditFlags flags);

        template <typename VectorT> requires (IsVector3<VectorT> || IsVector4<VectorT>)
        bool ColorPicker(StringView label, VectorT& outColor, ImGuiColorEditFlags flags);

        template <typename VectorT> requires (IsVector3<VectorT> || IsVector4<VectorT>)
        bool ColorPicker(StringView label, VectorT& outColor, ImGuiColorEditFlags flags, const ImVec4* ref_col);

        template <typename VectorT> requires (IsVector3<VectorT> || IsVector4<VectorT>)
        void ColorEditRestoreH(const VectorT& col, typename VectorT::ScalarT& H)
        {
            ::ImGuiContext& g = *GImGui;
            IM_ASSERT(g.ColorEditCurrentID != 0);
            if (g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(col.X, col.Y, col.Z, 0)))
                return;
            H = g.ColorEditSavedHue;
        }

        // ColorEdit supports RGB and HSV inputs. In case of RGB input resulting color may have undefined hue and/or saturation.
        // Since widget displays both RGB and HSV values we must preserve hue and saturation to prevent these values resetting.
        template <typename VectorT> requires (IsVector3<VectorT> || IsVector4<VectorT>)
        void ColorEditRestoreHS(const VectorT& col, typename VectorT::ScalarT& H, typename VectorT::ScalarT& S, typename VectorT::ScalarT& V)
        {
            ::ImGuiContext& g = *GImGui;
            IM_ASSERT(g.ColorEditCurrentID != 0);
            if (g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(col.X, col.Y, col.Z, 0)))
                return;

            // When S == 0, H is undefined.
            // When H == 1 it wraps around to 0.
            if (S == 0.0f || (H == 0.0f && g.ColorEditSavedHue == 1))
                H = g.ColorEditSavedHue;

            // When V == 0, S is undefined.
            if (V == 0.0f)
                S = g.ColorEditSavedSat;
        }

        void RenderArrowsForVerticalBar(ImDrawList* draw_list, ImVec2 pos, ImVec2 half_sz, float bar_w, float alpha)
        {
            ImU32 alpha8 = IM_F32_TO_INT8_SAT(alpha);
            ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + half_sz.x + 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), ImGuiDir_Right, IM_COL32(0, 0, 0, alpha8));
            ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + half_sz.x, pos.y), half_sz, ImGuiDir_Right, IM_COL32(255, 255, 255, alpha8));
            ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + bar_w - half_sz.x - 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), ImGuiDir_Left, IM_COL32(0, 0, 0, alpha8));
            ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + bar_w - half_sz.x, pos.y), half_sz, ImGuiDir_Left, IM_COL32(255, 255, 255, alpha8));
        }

        template <typename VectorT> requires (IsVector3<VectorT> || IsVector4<VectorT>)
        void ColorPickerOptionsPopup(const VectorT& ref_col, ImGuiColorEditFlags flags)
        {
            bool allow_opt_picker = !(flags & ImGuiColorEditFlags_PickerMask_);
            bool allow_opt_alpha_bar = !(flags & ImGuiColorEditFlags_NoAlpha) && !(flags & ImGuiColorEditFlags_AlphaBar);
            if ((!allow_opt_picker && !allow_opt_alpha_bar) || !ImGui::BeginPopup("context"))
                return;

            ::ImGuiContext& g = *GImGui;
            ImGui::PushItemFlag(ImGuiItemFlags_NoMarkEdited, true);
            if (allow_opt_picker)
            {
                ImVec2 picker_size(g.FontSize * 8, ImMax(g.FontSize * 8 - (ImGui::GetFrameHeight() + g.Style.ItemInnerSpacing.x), 1.0f)); // FIXME: Picker size copied from main picker function
                ImGui::PushItemWidth(picker_size.x);
                for (int picker_type = 0; picker_type < 2; picker_type++)
                {
                    // Draw small/thumbnail version of each picker type (over an invisible button for selection)
                    if (picker_type > 0) ImGui::Separator();
                    ImGui::PushID(picker_type);
                    ImGuiColorEditFlags picker_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview | (flags & ImGuiColorEditFlags_NoAlpha);
                    if (picker_type == 0) picker_flags |= ImGuiColorEditFlags_PickerHueBar;
                    if (picker_type == 1) picker_flags |= ImGuiColorEditFlags_PickerHueWheel;
                    ImVec2 backup_pos = ImGui::GetCursorScreenPos();
                    if (ImGui::Selectable("##selectable", false, 0, picker_size)) // By default, Selectable() is closing popup
                        g.ColorEditOptions = (g.ColorEditOptions & ~ImGuiColorEditFlags_PickerMask_) | (picker_flags & ImGuiColorEditFlags_PickerMask_);
                    ImGui::SetCursorScreenPos(backup_pos);
                    VectorT previewing_ref_col = ref_col;
                    ColorPicker("##previewing_picker", previewing_ref_col, picker_flags);
                    ImGui::PopID();
                }
                ImGui::PopItemWidth();
            }
            if (allow_opt_alpha_bar)
            {
                if (allow_opt_picker) ImGui::Separator();
                ImGui::CheckboxFlags("Alpha Bar", &g.ColorEditOptions, ImGuiColorEditFlags_AlphaBar);
            }
            ImGui::PopItemFlag();
            ImGui::EndPopup();
        }

        template <typename VectorT> requires (IsVector3<VectorT> || IsVector4<VectorT>)
        bool ColorPicker(StringView label, VectorT& outColor, ImGuiColorEditFlags flags)
        {
            return ColorPicker(label, outColor, flags, nullptr);
        }

        // Note: Templated version of ImGui::ColorPicker4().
        // FIXME: we adjust the big color square height based on item width, which may cause a flickering feedback loop (if automatic height makes a vertical scrollbar appears, affecting automatic width..)
        // FIXME: this is trying to be aware of style.Alpha but not fully correct. Also, the color wheel will have overlapping glitches with (style.Alpha < 1.0)
        template <typename VectorT> requires (IsVector3<VectorT> || IsVector4<VectorT>)
        bool ColorPicker(StringView label, VectorT& outColor, ImGuiColorEditFlags flags, const ImVec4* ref_col)
        {
            constexpr bool hasAlpha = IsVector4<VectorT>;
            using ScalarT = typename VectorT::ScalarT;

            ::ImGuiContext& g = *GImGui;
            ::ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems)
                return false;

            ImDrawList* draw_list = window->DrawList;
            ImGuiStyle& style = g.Style;
            ImGuiIO& io = g.IO;

            const float width = ImGui::CalcItemWidth();
            const bool is_readonly = ((g.NextItemData.ItemFlags | g.CurrentItemFlags) & ImGuiItemFlags_ReadOnly) != 0;
            g.NextItemData.ClearFlags();

            ScopedImGuiId scopedId(label);
            const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
            if (set_current_color_edit_id)
                g.ColorEditCurrentID = window->IDStack.back();
            ImGui::BeginGroup();

            if (!(flags & ImGuiColorEditFlags_NoSidePreview))
                flags |= ImGuiColorEditFlags_NoSmallPreview;

            // Context menu: display and store options.
            if (!(flags & ImGuiColorEditFlags_NoOptions))
                ColorPickerOptionsPopup(outColor, flags);

            // Read stored options
            if (!(flags & ImGuiColorEditFlags_PickerMask_))
                flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_PickerMask_;
            if (!(flags & ImGuiColorEditFlags_InputMask_))
                flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_InputMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_InputMask_;
            IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_PickerMask_)); // Check that only 1 is selected
            IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));  // Check that only 1 is selected
            if (!(flags & ImGuiColorEditFlags_NoOptions))
                flags |= (g.ColorEditOptions & ImGuiColorEditFlags_AlphaBar);

            // Setup
            bool alpha_bar = (flags & ImGuiColorEditFlags_AlphaBar) && !(flags & ImGuiColorEditFlags_NoAlpha);
            ImVec2 picker_pos = window->DC.CursorPos;
            float square_sz = ImGui::GetFrameHeight();
            float bars_width = square_sz; // Arbitrary smallish width of Hue/Alpha picking bars
            float sv_picker_size = ImMax(bars_width * 1, width - (alpha_bar ? 2 : 1) * (bars_width + style.ItemInnerSpacing.x)); // Saturation/Value picking box
            float bar0_pos_x = picker_pos.x + sv_picker_size + style.ItemInnerSpacing.x;
            float bar1_pos_x = bar0_pos_x + bars_width + style.ItemInnerSpacing.x;
            float bars_triangles_half_sz = IM_TRUNC(bars_width * 0.20f);

            VectorT backup_initial_col = outColor;

            float wheel_thickness = sv_picker_size * 0.08f;
            float wheel_r_outer = sv_picker_size * 0.50f;
            float wheel_r_inner = wheel_r_outer - wheel_thickness;
            ImVec2 wheel_center(picker_pos.x + (sv_picker_size + bars_width) * 0.5f, picker_pos.y + sv_picker_size * 0.5f);

            // Note: the triangle is displayed rotated with triangle_pa pointing to Hue, but most coordinates stays unrotated for logic.
            float triangle_r = wheel_r_inner - (int)(sv_picker_size * 0.027f);
            ImVec2 triangle_pa = ImVec2(triangle_r, 0.0f); // Hue point.
            ImVec2 triangle_pb = ImVec2(triangle_r * -0.5f, triangle_r * -0.866025f); // Black point.
            ImVec2 triangle_pc = ImVec2(triangle_r * -0.5f, triangle_r * +0.866025f); // White point.

            float H = outColor.X, S = outColor.Y, V = outColor.Z;
            float R = outColor.X, G = outColor.Y, B = outColor.Z;
            if (flags & ImGuiColorEditFlags_InputRGB)
            {
                // Hue is lost when converting from grayscale rgb (saturation=0). Restore it.
                ImGui::ColorConvertRGBtoHSV(R, G, B, H, S, V);
                ColorEditRestoreHS(outColor, H, S, V);
            }
            else if (flags & ImGuiColorEditFlags_InputHSV)
            {
                ImGui::ColorConvertHSVtoRGB(H, S, V, R, G, B);
            }

            bool value_changed = false, value_changed_h = false, value_changed_sv = false;

            ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);
            if (flags & ImGuiColorEditFlags_PickerHueWheel)
            {
                // Hue wheel + SV triangle logic
                ImGui::InvisibleButton("hsv", ImVec2(sv_picker_size + style.ItemInnerSpacing.x + bars_width, sv_picker_size));
                if (ImGui::IsItemActive() && !is_readonly)
                {
                    ImVec2 initial_off = g.IO.MouseClickedPos[0] - wheel_center;
                    ImVec2 current_off = g.IO.MousePos - wheel_center;
                    float initial_dist2 = ImLengthSqr(initial_off);
                    if (initial_dist2 >= (wheel_r_inner - 1) * (wheel_r_inner - 1) && initial_dist2 <= (wheel_r_outer + 1) * (wheel_r_outer + 1))
                    {
                        // Interactive with Hue wheel
                        H = ImAtan2(current_off.y, current_off.x) / IM_PI * 0.5f;
                        if (H < 0.0f)
                            H += 1.0f;
                        value_changed = value_changed_h = true;
                    }
                    float cos_hue_angle = ImCos(-H * 2.0f * IM_PI);
                    float sin_hue_angle = ImSin(-H * 2.0f * IM_PI);
                    if (ImTriangleContainsPoint(triangle_pa, triangle_pb, triangle_pc, ImRotate(initial_off, cos_hue_angle, sin_hue_angle)))
                    {
                        // Interacting with SV triangle
                        ImVec2 current_off_unrotated = ImRotate(current_off, cos_hue_angle, sin_hue_angle);
                        if (!ImTriangleContainsPoint(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated))
                            current_off_unrotated = ImTriangleClosestPoint(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated);
                        float uu, vv, ww;
                        ImTriangleBarycentricCoords(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated, uu, vv, ww);
                        V = ImClamp(1.0f - vv, 0.0001f, 1.0f);
                        S = ImClamp(uu / V, 0.0001f, 1.0f);
                        value_changed = value_changed_sv = true;
                    }
                }
                if (!(flags & ImGuiColorEditFlags_NoOptions))
                    ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
            }
            else if (flags & ImGuiColorEditFlags_PickerHueBar)
            {
                // SV rectangle logic
                ImGui::InvisibleButton("sv", ImVec2(sv_picker_size, sv_picker_size));
                if (ImGui::IsItemActive() && !is_readonly)
                {
                    S = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size - 1));
                    V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
                    ColorEditRestoreH(outColor, H); // Greatly reduces hue jitter and reset to 0 when hue == 255 and color is rapidly modified using SV square.
                    value_changed = value_changed_sv = true;
                }
                if (!(flags & ImGuiColorEditFlags_NoOptions))
                    ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

                // Hue bar logic
                ImGui::SetCursorScreenPos(ImVec2(bar0_pos_x, picker_pos.y));
                ImGui::InvisibleButton("hue", ImVec2(bars_width, sv_picker_size));
                if (ImGui::IsItemActive() && !is_readonly)
                {
                    H = ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
                    value_changed = value_changed_h = true;
                }
            }

            // Alpha bar logic
            if constexpr (hasAlpha)
            {
                if (alpha_bar)
                {
                    ImGui::SetCursorScreenPos(ImVec2(bar1_pos_x, picker_pos.y));
                    ImGui::InvisibleButton("alpha", ImVec2(bars_width, sv_picker_size));
                    if (ImGui::IsItemActive())
                    {
                        outColor.W = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
                        value_changed = true;
                    }
                }
            }

            ImGui::PopItemFlag(); // ImGuiItemFlags_NoNav

            if (!(flags & ImGuiColorEditFlags_NoSidePreview))
            {
                ImGui::SameLine(0, style.ItemInnerSpacing.x);
                ImGui::BeginGroup();
            }

            if (!(flags & ImGuiColorEditFlags_NoLabel))
            {
                const char* label_display_end = ImGui::FindRenderedTextEnd(label.data());
                if (label != label_display_end)
                {
                    if ((flags & ImGuiColorEditFlags_NoSidePreview))
                        ImGui::SameLine(0, style.ItemInnerSpacing.x);
                    ImGui::TextEx(label.data(), label_display_end);
                }
            }

            if (!(flags & ImGuiColorEditFlags_NoSidePreview))
            {
                ImGui::PushItemFlag(ImGuiItemFlags_NoNavDefaultFocus, true);
                ImVec4 col_v4(static_cast<onyxF32>(outColor.X), static_cast<onyxF32>(outColor.Y), static_cast<onyxF32>(outColor.Z), 1.0f);

                if constexpr (hasAlpha)
                {
                    col_v4.w = (flags & ImGuiColorEditFlags_NoAlpha) ? 1.0f : static_cast<onyxF32>(outColor.W);
                }

                if ((flags & ImGuiColorEditFlags_NoLabel))
                    ImGui::Text("Current");

                ImGuiColorEditFlags sub_flags_to_forward = ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_NoTooltip;
                ImGui::ColorButton("##current", col_v4, (flags & sub_flags_to_forward), ImVec2(square_sz * 3, square_sz * 2));
                if (ref_col != nullptr)
                {
                    ImGui::Text("Original");
                    ImVec4 ref_col_v4(ref_col->x, ref_col->y, ref_col->z, (flags& ImGuiColorEditFlags_NoAlpha) ? 1.0f : ref_col->w);
                    if (ImGui::ColorButton("##original", ref_col_v4, (flags & sub_flags_to_forward), ImVec2(square_sz * 3, square_sz * 2)))
                    {
                        outColor.X = static_cast<ScalarT>(col_v4.x);
                        outColor.Y = static_cast<ScalarT>(col_v4.y);
                        outColor.Z = static_cast<ScalarT>(col_v4.z);
                        if constexpr (hasAlpha)
                        {
                            outColor.W = static_cast<ScalarT>(col_v4.w);
                        }
                        value_changed = true;
                    }
                }

#if ONYX_IS_WINDOWS
                ImGuiID colorPickStateId = ImGui::GetID("isColorPickActive");
                ImGuiStorage* storage = ImGui::GetStateStorage();
                bool& colorPickState = *storage->GetBoolRef(colorPickStateId, false);
                //ImGuiID buttonID = ImGui::GetID("Pick");
                if (ImGui::Button("Pick"))
                {
                    colorPickState = true;
                    g_UiContext.InputSystem->EnableSystemMouseCapture(true);
                }

                if ( colorPickState )
                {
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) 
                    {
                        constexpr onyxF32 U8ToF32 = 1.0f / 255.0f;
                        Vector3u8 pixelColor = Platform::GetPixelColorAtMousePosition();
                        Vector3f32 saturatedPixelColor(pixelColor.X* U8ToF32, pixelColor.Y* U8ToF32, pixelColor.Z* U8ToF32);

                        if (outColor != saturatedPixelColor)
                        {
                            outColor = saturatedPixelColor;
                            value_changed = true;
                        }

                        colorPickState = false;
                        g_UiContext.InputSystem->EnableSystemMouseCapture(false);
                        
                        io.MouseClicked[0] = false; //consume mouse click to not lose focus
                        io.MouseDown[0] = false;
                    }
                }
#endif
                ImGui::PopItemFlag();
                ImGui::EndGroup();
            }

            // Convert back color to RGB
            if (value_changed_h || value_changed_sv)
            {
                if (flags & ImGuiColorEditFlags_InputRGB)
                {
                    ImGui::ColorConvertHSVtoRGB(H, S, V, outColor.X, outColor.Y, outColor.Z);
                    g.ColorEditSavedHue = H;
                    g.ColorEditSavedSat = S;
                    g.ColorEditSavedID = g.ColorEditCurrentID;
                    g.ColorEditSavedColor = ImGui::ColorConvertFloat4ToU32(ImVec4(outColor.X, outColor.Y, outColor.Z, 0));
                }
                else if (flags & ImGuiColorEditFlags_InputHSV)
                {
                    outColor.X = static_cast<ScalarT>(H);
                    outColor.X = static_cast<ScalarT>(S);
                    outColor.X = static_cast<ScalarT>(V);
                }
            }

            // R,G,B and H,S,V slider color editor
            bool value_changed_fix_hue_wrap = false;
            if ((flags & ImGuiColorEditFlags_NoInputs) == 0)
            {
                ImGui::PushItemWidth((alpha_bar ? bar1_pos_x : bar0_pos_x) + bars_width - picker_pos.x);
                ImGuiColorEditFlags sub_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf;
                ImGuiColorEditFlags sub_flags = (flags & sub_flags_to_forward) | ImGuiColorEditFlags_NoPicker;
                if (flags & ImGuiColorEditFlags_DisplayRGB || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
                    if (ColorEdit("##rgb", outColor, sub_flags | ImGuiColorEditFlags_DisplayRGB))
                    {
                        // FIXME: Hackily differentiating using the DragInt (ActiveId != 0 && !ActiveIdAllowOverlap) vs. using the InputText or DropTarget.
                        // For the later we don't want to run the hue-wrap canceling code. If you are well versed in HSV picker please provide your input! (See #2050)
                        value_changed_fix_hue_wrap = (g.ActiveId != 0 && !g.ActiveIdAllowOverlap);
                        value_changed = true;
                    }
                if (flags & ImGuiColorEditFlags_DisplayHSV || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
                    value_changed |= ColorEdit("##hsv", outColor, sub_flags | ImGuiColorEditFlags_DisplayHSV);
                if (flags & ImGuiColorEditFlags_DisplayHex || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
                    value_changed |= ColorEdit("##hex", outColor, sub_flags | ImGuiColorEditFlags_DisplayHex);
                ImGui::PopItemWidth();
            }

            // Try to cancel hue wrap (after ColorEdit4 call), if any
            if (value_changed_fix_hue_wrap && (flags & ImGuiColorEditFlags_InputRGB))
            {
                ScalarT new_H, new_S, new_V;
                ImGui::ColorConvertRGBtoHSV(outColor.X, outColor.Y, outColor.Z, new_H, new_S, new_V);
                if (new_H <= 0 && H > 0)
                {
                    if (new_V <= 0 && V != new_V)
                        ImGui::ColorConvertHSVtoRGB(H, S, new_V <= 0 ? V * 0.5f : new_V, outColor.X, outColor.Y, outColor.Z);
                    else if (new_S <= 0)
                        ImGui::ColorConvertHSVtoRGB(H, new_S <= 0 ? S * 0.5f : new_S, new_V, outColor.X, outColor.Y, outColor.Z);
                }
            }

            if (value_changed)
            {
                if (flags & ImGuiColorEditFlags_InputRGB)
                {
                    R = outColor.X;
                    G = outColor.Y;
                    B = outColor.Z;
                    ImGui::ColorConvertRGBtoHSV(R, G, B, H, S, V);
                    ColorEditRestoreHS(outColor, H, S, V);   // Fix local Hue as display below will use it immediately.
                }
                else if (flags & ImGuiColorEditFlags_InputHSV)
                {
                    H = outColor.X;
                    S = outColor.Y;
                    V = outColor.Z;
                    ImGui::ColorConvertHSVtoRGB(H, S, V, R, G, B);
                }
            }

            const int style_alpha8 = IM_F32_TO_INT8_SAT(style.Alpha);
            const ImU32 col_black = IM_COL32(0, 0, 0, style_alpha8);
            const ImU32 col_white = IM_COL32(255, 255, 255, style_alpha8);
            const ImU32 col_midgrey = IM_COL32(128, 128, 128, style_alpha8);
            const ImU32 col_hues[6 + 1] = { IM_COL32(255,0,0,style_alpha8), IM_COL32(255,255,0,style_alpha8), IM_COL32(0,255,0,style_alpha8), IM_COL32(0,255,255,style_alpha8), IM_COL32(0,0,255,style_alpha8), IM_COL32(255,0,255,style_alpha8), IM_COL32(255,0,0,style_alpha8) };

            ImVec4 hue_color_f(1, 1, 1, style.Alpha); ImGui::ColorConvertHSVtoRGB(H, ScalarT(1), ScalarT(1), hue_color_f.x, hue_color_f.y, hue_color_f.z);
            ImU32 hue_color32 = ImGui::ColorConvertFloat4ToU32(hue_color_f);
            ImU32 user_col32_striped_of_alpha = ImGui::ColorConvertFloat4ToU32(ImVec4(R, G, B, style.Alpha)); // Important: this is still including the main rendering/style alpha!!

            ImVec2 sv_cursor_pos;

            if (flags & ImGuiColorEditFlags_PickerHueWheel)
            {
                // Render Hue Wheel
                const float aeps = 0.5f / wheel_r_outer; // Half a pixel arc length in radians (2pi cancels out).
                const int segment_per_arc = ImMax(4, (int)wheel_r_outer / 12);
                for (int n = 0; n < 6; n++)
                {
                    const float a0 = (n) / 6.0f * 2.0f * IM_PI - aeps;
                    const float a1 = (n + 1.0f) / 6.0f * 2.0f * IM_PI + aeps;
                    const int vert_start_idx = draw_list->VtxBuffer.Size;
                    draw_list->PathArcTo(wheel_center, (wheel_r_inner + wheel_r_outer) * 0.5f, a0, a1, segment_per_arc);
                    draw_list->PathStroke(col_white, 0, wheel_thickness);
                    const int vert_end_idx = draw_list->VtxBuffer.Size;

                    // Paint colors over existing vertices
                    ImVec2 gradient_p0(wheel_center.x + ImCos(a0) * wheel_r_inner, wheel_center.y + ImSin(a0) * wheel_r_inner);
                    ImVec2 gradient_p1(wheel_center.x + ImCos(a1) * wheel_r_inner, wheel_center.y + ImSin(a1) * wheel_r_inner);
                    ImGui::ShadeVertsLinearColorGradientKeepAlpha(draw_list, vert_start_idx, vert_end_idx, gradient_p0, gradient_p1, col_hues[n], col_hues[n + 1]);
                }

                // Render Cursor + preview on Hue Wheel
                float cos_hue_angle = ImCos(H * 2.0f * IM_PI);
                float sin_hue_angle = ImSin(H * 2.0f * IM_PI);
                ImVec2 hue_cursor_pos(wheel_center.x + cos_hue_angle * (wheel_r_inner + wheel_r_outer) * 0.5f, wheel_center.y + sin_hue_angle * (wheel_r_inner + wheel_r_outer) * 0.5f);
                float hue_cursor_rad = value_changed_h ? wheel_thickness * 0.65f : wheel_thickness * 0.55f;
                int hue_cursor_segments = draw_list->_CalcCircleAutoSegmentCount(hue_cursor_rad); // Lock segment count so the +1 one matches others.
                draw_list->AddCircleFilled(hue_cursor_pos, hue_cursor_rad, hue_color32, hue_cursor_segments);
                draw_list->AddCircle(hue_cursor_pos, hue_cursor_rad + 1, col_midgrey, hue_cursor_segments);
                draw_list->AddCircle(hue_cursor_pos, hue_cursor_rad, col_white, hue_cursor_segments);

                // Render SV triangle (rotated according to hue)
                ImVec2 tra = wheel_center + ImRotate(triangle_pa, cos_hue_angle, sin_hue_angle);
                ImVec2 trb = wheel_center + ImRotate(triangle_pb, cos_hue_angle, sin_hue_angle);
                ImVec2 trc = wheel_center + ImRotate(triangle_pc, cos_hue_angle, sin_hue_angle);
                ImVec2 uv_white = ImGui::GetFontTexUvWhitePixel();
                draw_list->PrimReserve(3, 3);
                draw_list->PrimVtx(tra, uv_white, hue_color32);
                draw_list->PrimVtx(trb, uv_white, col_black);
                draw_list->PrimVtx(trc, uv_white, col_white);
                draw_list->AddTriangle(tra, trb, trc, col_midgrey, 1.5f);
                sv_cursor_pos = ImLerp(ImLerp(trc, tra, ImSaturate(S)), trb, ImSaturate(1 - V));
            }
            else if (flags & ImGuiColorEditFlags_PickerHueBar)
            {
                // Render SV Square
                draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), col_white, hue_color32, hue_color32, col_white);
                draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), 0, 0, col_black, col_black);
                ImGui::RenderFrameBorder(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), 0.0f);
                sv_cursor_pos.x = ImClamp(IM_ROUND(picker_pos.x + ImSaturate(S) * sv_picker_size), picker_pos.x + 2, picker_pos.x + sv_picker_size - 2); // Sneakily prevent the circle to stick out too much
                sv_cursor_pos.y = ImClamp(IM_ROUND(picker_pos.y + ImSaturate(1 - V) * sv_picker_size), picker_pos.y + 2, picker_pos.y + sv_picker_size - 2);

                // Render Hue Bar
                for (int i = 0; i < 6; ++i)
                    draw_list->AddRectFilledMultiColor(ImVec2(bar0_pos_x, picker_pos.y + i * (sv_picker_size / 6)), ImVec2(bar0_pos_x + bars_width, picker_pos.y + (i + 1) * (sv_picker_size / 6)), col_hues[i], col_hues[i], col_hues[i + 1], col_hues[i + 1]);
                float bar0_line_y = IM_ROUND(picker_pos.y + H * sv_picker_size);
                ImGui::RenderFrameBorder(ImVec2(bar0_pos_x, picker_pos.y), ImVec2(bar0_pos_x + bars_width, picker_pos.y + sv_picker_size), 0.0f);
                RenderArrowsForVerticalBar(draw_list, ImVec2(bar0_pos_x - 1, bar0_line_y), ImVec2(bars_triangles_half_sz + 1, bars_triangles_half_sz), bars_width + 2.0f, style.Alpha);
            }

            // Render cursor/preview circle (clamp S/V within 0..1 range because floating points colors may lead HSV values to be out of range)
            float sv_cursor_rad = value_changed_sv ? wheel_thickness * 0.55f : wheel_thickness * 0.40f;
            int sv_cursor_segments = draw_list->_CalcCircleAutoSegmentCount(sv_cursor_rad); // Lock segment count so the +1 one matches others.
            draw_list->AddCircleFilled(sv_cursor_pos, sv_cursor_rad, user_col32_striped_of_alpha, sv_cursor_segments);
            draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad + 1, col_midgrey, sv_cursor_segments);
            draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad, col_white, sv_cursor_segments);

            // Render alpha bar
            if constexpr (hasAlpha)
            {
                if (alpha_bar)
                {
                    float alpha = ImSaturate(outColor.W);
                    ImRect bar1_bb(bar1_pos_x, picker_pos.y, bar1_pos_x + bars_width, picker_pos.y + sv_picker_size);
                    ImGui::RenderColorRectWithAlphaCheckerboard(draw_list, bar1_bb.Min, bar1_bb.Max, 0, bar1_bb.GetWidth() / 2.0f, ImVec2(0.0f, 0.0f));
                    draw_list->AddRectFilledMultiColor(bar1_bb.Min, bar1_bb.Max, user_col32_striped_of_alpha, user_col32_striped_of_alpha, user_col32_striped_of_alpha & ~IM_COL32_A_MASK, user_col32_striped_of_alpha & ~IM_COL32_A_MASK);
                    float bar1_line_y = IM_ROUND(picker_pos.y + (1.0f - alpha) * sv_picker_size);
                    ImGui::RenderFrameBorder(bar1_bb.Min, bar1_bb.Max, 0.0f);
                    RenderArrowsForVerticalBar(draw_list, ImVec2(bar1_pos_x - 1, bar1_line_y), ImVec2(bars_triangles_half_sz + 1, bars_triangles_half_sz), bars_width + 2.0f, style.Alpha);
                }
            }

            ImGui::EndGroup();

            if (value_changed && backup_initial_col == outColor)
                value_changed = false;

            if (value_changed && g.LastItemData.ID != 0) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
                ImGui::MarkItemEdited(g.LastItemData.ID);

            if (set_current_color_edit_id)
                g.ColorEditCurrentID = 0;

            return value_changed;
        }

        // Edit colors components (each component in 0.0f..1.0f range).
        // See enum ImGuiColorEditFlags_ for available options. e.g. Only access 3 floats if ImGuiColorEditFlags_NoAlpha flag is set.
        // With typical options: Left-click on color square to open color picker. Right-click to open option menu. CTRL-Click over input fields to edit them and TAB to go to next item.
        template <typename VectorT> requires (IsVector3<VectorT> || IsVector4<VectorT>)
        bool ColorEdit(StringView id, VectorT& outColor, ImGuiColorEditFlags flags)
        {
            constexpr bool hasAlpha = IsVector4<VectorT>;
            
            ::ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems)
                return false;

            ::ImGuiContext& g = *GImGui;
            const ImGuiStyle& style = g.Style;
            const float square_sz = ImGui::GetFrameHeight();
            const char* label_display_end = ImGui::FindRenderedTextEnd(id.data());
            float w_full = ImGui::CalcItemWidth();
            g.NextItemData.ClearFlags();

            bool value_changed = false;
            bool value_changed_as_float = false;

            {
                ImGui::BeginGroup();
                ScopedImGuiId imguiScopeId(id);

                const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
                if (set_current_color_edit_id)
                    g.ColorEditCurrentID = window->IDStack.back();

                // If we're not showing any slider there's no point in doing any HSV conversions
                const ImGuiColorEditFlags flags_untouched = flags;
                if (flags & ImGuiColorEditFlags_NoInputs)
                    flags = (flags & (~ImGuiColorEditFlags_DisplayMask_)) | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions;

                // Context menu: display and modify options (before defaults are applied)
                if (!(flags & ImGuiColorEditFlags_NoOptions))
                    ImGui::ColorEditOptionsPopup(&outColor.X, flags);

                // Read stored options
                if (!(flags & ImGuiColorEditFlags_DisplayMask_))
                    flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DisplayMask_);
                if (!(flags & ImGuiColorEditFlags_DataTypeMask_))
                    flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DataTypeMask_);
                if (!(flags & ImGuiColorEditFlags_PickerMask_))
                    flags |= (g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_);
                if (!(flags & ImGuiColorEditFlags_InputMask_))
                    flags |= (g.ColorEditOptions & ImGuiColorEditFlags_InputMask_);
                flags |= (g.ColorEditOptions & ~(ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_));
                IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_DisplayMask_)); // Check that only 1 is selected
                IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));   // Check that only 1 is selected

                const float w_button = (flags & ImGuiColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
                const float w_inputs = ImMax(w_full - w_button, 1.0f);
                w_full = w_inputs + w_button;

                // Convert to the formats we need
                auto floatVector = [&]()
                {
                    if constexpr (hasAlpha)
                        return Vector4f32{ outColor.X, outColor.Y, outColor.Z, outColor.W };
                    else
                        return Vector3f32{ outColor.X, outColor.Y, outColor.Z };
                }();

                if ((flags & ImGuiColorEditFlags_InputHSV) && (flags & ImGuiColorEditFlags_DisplayRGB))
                    ImGui::ColorConvertHSVtoRGB(floatVector[0], floatVector[1], floatVector[2], floatVector[0], floatVector[1], floatVector[2]);
                else if ((flags & ImGuiColorEditFlags_InputRGB) && (flags & ImGuiColorEditFlags_DisplayHSV))
                {
                    // Hue is lost when converting from grayscale rgb (saturation=0). Restore it.
                    ImGui::ColorConvertRGBtoHSV(floatVector[0], floatVector[1], floatVector[2], floatVector[0], floatVector[1], floatVector[2]);
                    ColorEditRestoreHS(outColor, floatVector.X, floatVector.Y, floatVector.Z);
                }

                auto toRGB = [](onyxF32 val) { return static_cast<onyxU8>(std::clamp(val, 0.0f, 1.0f) * 255.0f + 0.5f); };
                auto intVector = [&]()
                {
                    if constexpr (hasAlpha)
                        return Vector4u8{ toRGB(floatVector[0]), toRGB(floatVector[1]), toRGB(floatVector[2]), toRGB(floatVector[3]) };
                    else
                        return Vector3u8{ toRGB(floatVector[0]), toRGB(floatVector[1]), toRGB(floatVector[2]) };

                }();

                const ImVec2 pos = window->DC.CursorPos;
                const float inputs_offset_x = (style.ColorButtonPosition == ImGuiDir_Left) ? w_button : 0.0f;
                window->DC.CursorPos.x = pos.x + inputs_offset_x;

                ::ImGuiWindow* picker_active_window = nullptr;
                if ((flags & (ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHSV)) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
                {
                    {
                        onyxF32 componentInputSize = 50.0f;
                        ScopedImGuiStyle styleOverride(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 0.0f));
                        ImGui::BeginHorizontal("##controls");

                        const bool isDisplayingRGB = Enums::HasAnyFlags(flags, ImGuiColorEditFlags_DisplayRGB);

                        const StringView labelX = isDisplayingRGB ? "R" : "H";
                        const StringView labelY = isDisplayingRGB ? "G" : "S";
                        const StringView labelZ = isDisplayingRGB ? "B" : "V";
                        const StringView labelW = "A";

//                        onyxF32 red, green, blue;

                        //ImGui::ColorConvertHSVtoRGB(floatVector[0], 1.0f, 0.5f, red, green, blue);

                        const onyxU32 colorX = isDisplayingRGB ? VectorControl::BACKGROUND_COLOR_X_COMPONENT :  VectorControl::BACKGROUND_COLOR_W_COMPONENT; //ColorConvertHSVtoBGR(floatVector.X, 1.0f, 1.0f);
                        const onyxU32 colorY = isDisplayingRGB ? VectorControl::BACKGROUND_COLOR_Y_COMPONENT :  VectorControl::BACKGROUND_COLOR_W_COMPONENT; //ColorConvertHSVtoBGR(floatVector.X, floatVector.Y, 1.0f);
                        const onyxU32 colorZ = isDisplayingRGB ? VectorControl::BACKGROUND_COLOR_Z_COMPONENT : VectorControl::BACKGROUND_COLOR_W_COMPONENT; //ColorConvertHSVtoBGR(floatVector.X, 1.0f, floatVector.Z);
                        const onyxU32 colorW = VectorControl::BACKGROUND_COLOR_W_COMPONENT;

                        if (flags & ImGuiColorEditFlags_Float)
                        {
                            //ImGui::BeginHorizontal("##vec3Inputs");

                            //TODO: show error tooltip or error effect on UI when clamping (red flash)
                            value_changed |= VectorControl::VectorComponentInput(labelX, floatVector.X, ImGuiDataType_Float, componentInputSize, colorX);
                            value_changed |= VectorControl::VectorComponentInput(labelY, floatVector.Y, ImGuiDataType_Float, componentInputSize, colorY);
                            value_changed |= VectorControl::VectorComponentInput(labelZ, floatVector.Z, ImGuiDataType_Float, componentInputSize, colorZ);

                            if constexpr (hasAlpha)
                            {
                                value_changed |= VectorControl::VectorComponentInput(labelW, floatVector.W, ImGuiDataType_Float, componentInputSize, colorW);
                            }
                        }
                        else
                        {
                            value_changed |= VectorControl::VectorComponentInput(labelX, intVector.X, ImGuiDataType_U8, componentInputSize, colorX);
                            value_changed |= VectorControl::VectorComponentInput(labelY, intVector.Y, ImGuiDataType_U8, componentInputSize, colorY);
                            value_changed |= VectorControl::VectorComponentInput(labelZ, intVector.Z, ImGuiDataType_U8, componentInputSize, colorZ);

                            if constexpr (hasAlpha)
                            {
                                value_changed |= VectorControl::VectorComponentInput(labelW, intVector.W, ImGuiDataType_U8, componentInputSize, colorW);
                            }
                        }
                    }

                    if (!(flags & ImGuiColorEditFlags_NoOptions))
                        ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

                    if (!(flags & ImGuiColorEditFlags_NoSmallPreview))
                    {
                        ImVec4 col_v4(outColor.X, outColor.Y, outColor.Z, 1.0f);

                        if constexpr (hasAlpha)
                        {
                            col_v4.w = outColor.W;
                        }

                        if (ImGui::ColorButton("##ColorButton", col_v4, flags))
                        {
                            if (!(flags & ImGuiColorEditFlags_NoPicker))
                            {
                                // Store current color and open a picker
                                g.ColorPickerRef = col_v4;
                                ImGui::OpenPopup("picker");
                                ImGui::SetNextWindowPos(g.LastItemData.Rect.GetBL() + ImVec2(0.0f, style.ItemSpacing.y));
                            }
                        }

                        if (!(flags & ImGuiColorEditFlags_NoOptions))
                            ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

                        if (ImGui::BeginPopup("picker", ImGuiWindowFlags_Popup))
                        {
                            if (g.CurrentWindow->BeginCount == 1)
                            {
                                picker_active_window = g.CurrentWindow;
                                if (id.data() != label_display_end)
                                {
                                    ImGui::TextEx(id.data(), label_display_end);
                                    ImGui::Spacing();
                                }
                                ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
                                ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
                                ImGui::SetNextItemWidth(square_sz * 12.0f); // Use 256 + bar sizes?
                                
                                value_changed |= ColorPicker("##picker", floatVector, picker_flags, &g.ColorPickerRef);

                                if (value_changed)
                                {
                                    outColor = floatVector;
                                }

                            }
                            ImGui::EndPopup();
                        }
                    }

                    ImGui::EndHorizontal();
                }
                else if ((flags & ImGuiColorEditFlags_DisplayHex) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
                {
                    //// RGB Hexadecimal Input
                    //char buf[64];
                    //if (alpha)
                    //    ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255), ImClamp(i[3], 0, 255));
                    //else
                    //    ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255));
                    //ImGui::SetNextItemWidth(w_inputs);
                    //if (ImGui::InputText("##Text", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_CharsUppercase))
                    //{
                    //    value_changed = true;
                    //    char* p = buf;
                    //    while (*p == '#' || ImCharIsBlankA(*p))
                    //        p++;
                    //    i[0] = i[1] = i[2] = 0;
                    //    i[3] = 0xFF; // alpha default to 255 is not parsed by scanf (e.g. inputting #FFFFFF omitting alpha)
                    //    int r;
                    //    if (alpha)
                    //        r = sscanf(p, "%02X%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2], (unsigned int*)&i[3]); // Treat at unsigned (%X is unsigned)
                    //    else
                    //        r = sscanf(p, "%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2]);
                    //    IM_UNUSED(r); // Fixes C6031: Return value ignored: 'sscanf'.
                    //}
                    if (!(flags & ImGuiColorEditFlags_NoOptions))
                        ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
                }

                if (id.data() != label_display_end && !(flags & ImGuiColorEditFlags_NoLabel))
                {
                    // Position not necessarily next to last submitted button (e.g. if style.ColorButtonPosition == ImGuiDir_Left),
                    // but we need to use SameLine() to setup baseline correctly. Might want to refactor SameLine() to simplify this.
                    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                    window->DC.CursorPos.x = pos.x + ((flags & ImGuiColorEditFlags_NoInputs) ? w_button : w_full + style.ItemInnerSpacing.x);
                    ImGui::TextEx(id.data(), label_display_end);
                }

                // Convert back
                if (value_changed && picker_active_window == NULL)
                {
                    if (!value_changed_as_float)
                    {
                        constexpr onyxU32 componentCount = hasAlpha ? 4 : 3;
                        for (onyxU32 n = 0; n < componentCount; n++)
                            floatVector[n] = intVector[n] / 255.0f;
                    }

                    if ((flags & ImGuiColorEditFlags_DisplayHSV) && (flags & ImGuiColorEditFlags_InputRGB))
                    {
                        g.ColorEditSavedHue = floatVector[0];
                        g.ColorEditSavedSat = floatVector[1];
                        ImGui::ColorConvertHSVtoRGB(floatVector[0], floatVector[1], floatVector[2], floatVector[0], floatVector[1], floatVector[2]);
                        g.ColorEditSavedID = g.ColorEditCurrentID;
                        g.ColorEditSavedColor = ImGui::ColorConvertFloat4ToU32(ImVec4(floatVector[0], floatVector[1], floatVector[2], 0));
                    }
                    if ((flags & ImGuiColorEditFlags_DisplayRGB) && (flags & ImGuiColorEditFlags_InputHSV))
                        ImGui::ColorConvertRGBtoHSV(floatVector[0], floatVector[1], floatVector[2], floatVector[0], floatVector[1], floatVector[2]);

                    outColor = floatVector;
                }

                if (set_current_color_edit_id)
                    g.ColorEditCurrentID = 0;
                //ImGui::PopID();

                ImGui::EndGroup();
            }

            // Drag and Drop Target
            // NB: The flag test is merely an optional micro-optimization, BeginDragDropTarget() does the same test.
            //if ((g.LastItemData.StatusFlags & ImGuiItemStatusFlags_HoveredRect) && !(g.LastItemData.ItemFlags & ImGuiItemFlags_ReadOnly) && !(flags & ImGuiColorEditFlags_NoDragDrop) && BeginDragDropTarget())
            //{
            //    bool accepted_drag_drop = false;
            //    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
            //    {
            //        memcpy((float*)outColor.X, payload->Data, sizeof(float) * 3); // Preserve alpha if any //-V512 //-V1086
            //        value_changed = accepted_drag_drop = true;
            //    }
            //    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
            //    {
            //        memcpy((float*)outColor.X, payload->Data, sizeof(float) * components);
            //        value_changed = accepted_drag_drop = true;
            //    }

            //    // Drag-drop payloads are always RGB
            //    if (accepted_drag_drop && (flags & ImGuiColorEditFlags_InputHSV))
            //        ImGui::ColorConvertRGBtoHSV(col[0], col[1], col[2], col[0], col[1], col[2]);
            //    ImGui::EndDragDropTarget();
            //}

            //// When picker is being actively used, use its active id so IsItemActive() will function on ColorEdit4().
            //if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window)
            //    g.LastItemData.ID = g.ActiveId;

            if (value_changed && g.LastItemData.ID != 0) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
                ImGui::MarkItemEdited(g.LastItemData.ID);

            return value_changed;
        }
    }
    bool ColorInput(StringView id, Vector3u8& rgb)
    {
        Vector3f32 rgba(static_cast<onyxF32>(rgb.X), static_cast<onyxF32>(rgb.Y), static_cast<onyxF32>(rgb.Z));
        if ( ColorEdit(id, rgba, ImGuiColorEditFlags_NoAlpha) )
        {
            rgb.X = static_cast<onyxU8>(rgba.X);
            rgb.Y = static_cast<onyxU8>(rgba.Y);
            rgb.Z = static_cast<onyxU8>(rgba.Z);
            return true;
        }

        return false;
    }

    bool ColorInput(StringView id, Vector4u8& rgba)
    {
        ONYX_UNUSED(id);
        ONYX_UNUSED(rgba);
        
        return true;
    }

    bool ColorInput(StringView id, Vector3f32& hsv)
    {
        Vector3f32 rgb(hsv);
        if (ColorEdit(id, rgb, ImGuiColorEditFlags_None))
        {
            hsv = rgb;
            return true;
        }

        return false;
    }

    bool ColorInput(StringView id, Vector4f32& hsva)
    {
        ONYX_UNUSED(id);
        ONYX_UNUSED(hsva);
        return true;
    }
}
