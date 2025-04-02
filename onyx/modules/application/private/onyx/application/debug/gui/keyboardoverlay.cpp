#include <onyx/application/debug/gui/keyboardoverlay.h>

#if ONYX_UI_MODULE && ONYX_USE_IMGUI

#include <onyx/input/inputsystem.h>
#include <onyx/filesystem/onyxfile.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>
#include <imgui_internal.h>

namespace Onyx::Application::Debug
{
    KeyboardOverlay::KeyboardOverlay()
    {
        FileSystem::OnyxFile file{ "debug/keyboard/iso_105_us_international.json" };
        FileSystem::JsonValue jsonDoc = file.LoadJson();

        m_KeyboardLayout.reserve(6); // typical keyboards have 6 rows

        Vector2f offset;
        Vector2f offset2; // optional
        Vector2f size{ 1.0f, 1.0f };
        Vector2f size2{ 0.0f, 0.0f }; // optional

        for (const auto& rowData : jsonDoc.Json)
        {
            DynamicArray<KeyData>& keyboardRow = m_KeyboardLayout.emplace_back();
            for (const auto& columnValue : rowData)
            {
                if (columnValue.is_object() == false)
                {
                    ONYX_LOG_WARNING("Skipping element in keyboard json");
                    continue;
                }
                
                // is key data
                if (columnValue.contains("label"))
                {
                    const String& label = columnValue["label"].get<String>();
                    Input::Key key = Enums::ToEnum<Input::Key>(columnValue["keyCode"].get<onyxU16>());
                    keyboardRow.emplace_back(offset, offset2, size, size2, key, label);

                    offset.Set(0.0f, 0.0f);
                    offset2.Set(0.0f, 0.0f);
                    size.Set(1.0f, 1.0f);
                    size2.Set(0.0f, 0.0f);
                }
                else // is meta data
                {
                    if (columnValue.contains("x"))
                        offset[0] = columnValue["x"].get<float>();
                    if (columnValue.contains("x2"))
                        offset2[0] = columnValue["x2"].get<float>();

                    if (columnValue.contains("y"))
                        offset[1] = columnValue["y"].get<float>();
                    if (columnValue.contains("y2"))
                        offset2[1] = columnValue["y2"].get<float>();

                    if (columnValue.contains("w"))
                        size[0] = columnValue["w"].get<float>();
                    if (columnValue.contains("w2"))
                        size2[0] = columnValue["w2"].get<float>();

                    if (columnValue.contains("h"))
                        size[1] = columnValue["h"].get<float>();
                    if (columnValue.contains("h2"))
                        size2[1] = columnValue["h2"].get<float>();
                }
                
            }
        }
    }

    void KeyboardOverlay::DrawImGui(const Input::InputSystem& inputSystem)
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking;

        // TODO: This should probably be rewritten to use ItemAdd from Imgui to auto-size the window
        ImGui::SetNextWindowPos(ImVec2{ 300, 800 }, ImGuiCond_FirstUseEver);

        onyxF32 aspectRatio = 23.0f / static_cast<onyxF32>(m_KeyboardLayout.size() + 1);
        auto aspect_ratio_constraint = [](ImGuiSizeCallbackData* data)
        {
            float aspectRatio = *static_cast<onyxF32*>(data->UserData);
            data->DesiredSize.y = (float)(int)(data->DesiredSize.x / aspectRatio);
        };

        ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX), aspect_ratio_constraint, &aspectRatio);   // Aspect ratio
        ImGui::Begin("Keyboard", nullptr, windowFlags);
        ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());

        static const ImVec4 defaultColor{ 0.7f, 0.7f, 0.7f, 1.0f };
        static const ImVec4 pressedColor{ 0.9f, 0.2f, 0.2f, 1.0f };
        onyxU32 defaultColorU32 = ImGui::GetColorU32(defaultColor);
        onyxU32 pressedColorU32 = ImGui::GetColorU32(pressedColor);
        auto drawList = ImGui::GetWindowDrawList();
        const onyxF32 renderSize = ImGui::GetContentRegionAvail().x;
        float size = renderSize / 23.0f;
        const ImVec2& initialPosition = ImGui::GetCursorScreenPos();
        Vector2f cursorPos = { initialPosition.x, initialPosition.y };
        float spacing = 1.0f;

        ImRect keyBounds;
        for (const DynamicArray<KeyData>& keyboardRow : m_KeyboardLayout)
        {
            onyxU32 numButtonsInRow = static_cast<onyxU32>(keyboardRow.size());
            for (onyxU32 i = 0; i < numButtonsInRow; ++i)
            {
                const KeyData& key = keyboardRow[i];
                const Vector2f& rectSize = key.m_Size * size;

                Vector2f tempCursorPos = cursorPos;

                onyxU32 color = inputSystem.IsButtonDown(key.m_Key) ? pressedColorU32 : defaultColorU32;

                tempCursorPos += (key.m_Offset * size);
                keyBounds = ImRect({tempCursorPos[0], tempCursorPos[1]}, { tempCursorPos[0] + rectSize[0] - spacing, tempCursorPos[1] + rectSize[1] - spacing });
                drawList->AddRectFilled(keyBounds.Min, keyBounds.Max, color);

                if (key.m_Size2.IsZero() == false)
                {
                    const Vector2f& rectSize2 = key.m_Size2 * size;
                    Vector2f tmpCursorPos2 = tempCursorPos + (key.m_Offset2 * size);
                    keyBounds = ImRect({ tmpCursorPos2[0], tmpCursorPos2[1] }, { tmpCursorPos2[0] + rectSize2[0] - spacing, tmpCursorPos2[1] + rectSize2[1] - spacing });
                    drawList->AddRectFilled(keyBounds.Min, keyBounds.Max, color);
                }
               
                cursorPos = cursorPos + (key.m_Offset * size);
                cursorPos[0] += rectSize[0];
            }

            cursorPos[0] = initialPosition.x;
            cursorPos[1] += size;
        }

       // ImVec2 windowSize = ImVec2(initialPosition.x + renderSize, cursorPos[1]) - initialPosition;
        //ImGui::Dummy(windowSize);
        ImGui::End();
    }
}
#endif