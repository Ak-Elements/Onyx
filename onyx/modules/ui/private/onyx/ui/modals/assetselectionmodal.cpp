#include <onyx/application/ui/modals/assetselectionmodal.h>
#include <onyx/assets/assetsystem.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>
#include <imgui_internal.h>
#include <onyx/application/ui/widgets.h>

namespace Onyx::Application::AssetSelectionControl
{
    namespace Internal
    {
        bool loc_HasFocus = true;
        String loc_SearchString;
    }

    bool Render(const Assets::AssetSystem& assetSystem, Assets::AssetType assetType, Assets::AssetId& outAssetId)
    {
        bool hasModified = false;
        DynamicArray<Assets::AssetMetaData> availableAssets;

        String assetName = "None";
        if (outAssetId.IsValid())
        {
            const Assets::AssetMetaData& assetMeta = assetSystem.GetAssetMeta(outAssetId);
            assetName = assetMeta.Name;
        }

        ImGui::BeginHorizontal("#AssetControl");
        if (ImGui::BeginCombo("##selector", assetName.data()))
        {
            Gui::DrawSearchBar(Internal::loc_SearchString, "Asset", Internal::loc_HasFocus);
            StringView searchString = Trim(Internal::loc_SearchString);

            if (availableAssets.empty())
            {
                availableAssets = assetSystem.GetAvailableAssets(assetType);
            }

            for (const Assets::AssetMetaData& assetMeta : availableAssets)
            {
                if (assetMeta.Type != assetType)
                {
                    continue;
                }

                if ((searchString.empty() == false) && assetMeta.Name.find(searchString) == String::npos)
                {
                    continue;
                }

                bool isSelected = assetMeta.Id == outAssetId;
                if (ImGui::Selectable(assetMeta.Name.data(), isSelected))
                {
                    hasModified = outAssetId != assetMeta.Id;
                    outAssetId = assetMeta.Id;
                    Internal::loc_HasFocus = true;
                    Internal::loc_SearchString.clear();
                    break;
                }

                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        const ImGuiStyle& style = ImGui::GetStyle();
        ImVec2 buttonSize(16.0f, 16.0f);
        bool hovered, held;
        ImGuiID browseButtonId = ImGui::GetID("BrowseButton");

        //ImVec2 currentPos = ImGui::GetCursorScreenPos();
        //const onyxF32 currentHeight = ImGui::GetFrameHeight();
        //currentPos.y += currentHeight * 0.5f - buttonSize.y * 0.5f;
        ImRect bb(ImGui::GetCursorScreenPos() + style.FramePadding, ImGui::GetCursorScreenPos() + buttonSize + style.FramePadding);
        if (ImGui::ButtonBehavior(bb, browseButtonId, &hovered, &held))
        {
            ImGui::OpenPopup("AssetBrowser");
        }
        ImGui::ItemAdd(bb, browseButtonId);

        if (hovered || held)
        {
            onyxU32 buttonBackgroundColor = held ? ImGui::GetColorU32(ImGuiCol_ButtonActive) : ImGui::GetColorU32(ImGuiCol_ButtonHovered);
            ImGui::GetForegroundDrawList()->AddRectFilled(bb.Min - style.FramePadding, bb.Max + style.FramePadding, buttonBackgroundColor);
        }

        // TODO: Move to style
        // TODO: Find held & hovered color
        onyxU32 folderColor = 0xFF666666;
        onyxU32 lidColor = held ? 0xFFFF7929 : hovered ? 0xFFFF7929 : 0xFF888888;
        Gui::DrawFolderIcon(ImGui::GetForegroundDrawList(), style.FramePadding, buttonSize.x, 1.0f, folderColor, lidColor);
       
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_Appearing);
        if (ImGui::BeginPopupModal("AssetBrowser", nullptr))
        {
            if (availableAssets.empty())
            {
                availableAssets = assetSystem.GetAvailableAssets(assetType);
            }

            ImGui::BeginTable("##assetlist", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable);
            ImGui::TableSetupColumn("File Name", ImGuiTableColumnFlags_None, 0.7f);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None, 0.3f);
            ImGui::TableHeadersRow();

            for (const Assets::AssetMetaData& assetMeta : availableAssets)
            {
                if (assetMeta.Type != assetType)
                    continue;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                bool isSelected = assetMeta.Id == outAssetId;
                if (ImGui::Selectable(assetMeta.Name.data(), isSelected, ImGuiSelectableFlags_SpanAllColumns))
                {
                    hasModified = outAssetId != assetMeta.Id;
                    outAssetId = assetMeta.Id;
                    Internal::loc_HasFocus = true;
                    Internal::loc_SearchString.clear();
                    ImGui::CloseCurrentPopup();
                    break;
                }

                ImGui::TableSetColumnIndex(1);
                ImGui::TextEx("type");
            }

            ImGui::EndTable();
            ImGui::EndPopup();
        }

        ImGui::EndHorizontal();

        return hasModified;
    }
}
