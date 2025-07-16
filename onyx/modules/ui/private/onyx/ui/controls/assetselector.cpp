#include <onyx/ui/controls/assetselector.h>

#if ONYX_IS_EDITOR

#define IMGUI_DEFINE_MATH_OPERATORS
#include <onyx/assets/assetsystem.h>

#include <onyx/ui/widgets.h>
#include <onyx/ui/controls/button.h>
#include <onyx/ui/controls/combobox.h>
#include <onyx/ui/layout/imguilayout.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <onyx/localization/localization.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/localization/localizedstring.h>
#include <onyx/ui/imguisystem.h>
#include <onyx/ui/ui_localization.h>

namespace Onyx::Ui
{
    namespace
    {
        bool loc_HasFocus = true;
        String loc_SearchString;
    }

    bool AssetSelector(const Assets::AssetSystem& assetSystem, Assets::AssetType assetType, Assets::AssetId& outAssetId)
    {
        bool hasModified = false;
        DynamicArray<Assets::AssetMetaData> availableAssets;

        String assetName;
        if (outAssetId.IsValid())
        {
            const Assets::AssetMetaData& assetMeta = assetSystem.GetAssetMeta(outAssetId);
            assetName = assetMeta.GetName();
        }
        else
        {
            assetName = Localization::Generic::None.Get();
        }

        Layout::BeginHorizontal("#AssetControl");
        if (BeginCombobox("##selector", assetName))
        {
            DrawSearchBar(loc_SearchString, Localization::Generic::Search.Get(), loc_HasFocus);
            StringView searchString = Trim(loc_SearchString);

            if (availableAssets.empty())
            {
                availableAssets = assetSystem.GetAvailableAssets(assetType);
            }

            for (const Assets::AssetMetaData& assetMeta : availableAssets)
            {
                if ((searchString.empty() == false) && IgnoreCaseFind(assetMeta.GetName(), searchString) == String::npos)
                {
                    continue;
                }

                bool isSelected = assetMeta.Id == outAssetId;
                if (Selectable(assetMeta.GetName(), isSelected, true))
                {
                    hasModified = outAssetId != assetMeta.Id;
                    outAssetId = assetMeta.Id;
                    loc_HasFocus = true;
                    loc_SearchString.clear();
                    break;
                }
            }

            EndCombobox();
        }

        const ImGuiStyle& style = ImGui::GetStyle();
        ImVec2 buttonSize(16.0f, 16.0f);
        ImGuiID browseButtonId = ImGui::GetID("BrowseButton");

        //ImVec2 currentPos = ImGui::GetCursorScreenPos();
        //const onyxF32 currentHeight = ImGui::GetFrameHeight();
        //currentPos.y += currentHeight * 0.5f - buttonSize.y * 0.5f;
        ImRect bb(ImGui::GetCursorScreenPos() + style.FramePadding, ImGui::GetCursorScreenPos() + buttonSize + style.FramePadding);
        ButtonState state = Ui::ButtonBehavior(browseButtonId, bb);

        if (state == ButtonState::Pressed)
        {
            ImGui::OpenPopup("###AssetBrowser");
        }
        ImGui::ItemAdd(bb, browseButtonId);

        if ((state == ButtonState::Held) || (state == ButtonState::Hovered))
        {
            onyxU32 buttonBackgroundColor = state == ButtonState::Held ? ImGui::GetColorU32(ImGuiCol_ButtonActive) : ImGui::GetColorU32(ImGuiCol_ButtonHovered);
            ImGui::GetForegroundDrawList()->AddRectFilled(bb.Min - style.FramePadding, bb.Max + style.FramePadding, buttonBackgroundColor);
        }

        // TODO: Move to style
        // TODO: Find held & hovered color
        onyxU32 folderColor = 0xFF666666;
        onyxU32 lidColor = state == ButtonState::Held ? 0xFFFF7929 : state == ButtonState::Hovered ? 0xFFFF7929 : 0xFF888888;
        DrawFolderIcon(ImGui::GetForegroundDrawList(), style.FramePadding, buttonSize.x, 1.0f, folderColor, lidColor);

        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_Appearing);
        bool isOpen = true;
        StringView modalName = Format::Format("{}###AssetBrowser", Localization::Ui::AssetSelector::Modal::Title);
        if (ImGui::BeginPopupModal(modalName.data(), &isOpen))
        {
            if (availableAssets.empty())
            {
                availableAssets = assetSystem.GetAvailableAssets(assetType);
            }

            ImGui::BeginTable("##assetlist", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable);
            ImGui::TableSetupColumn(Localization::Generic::File.Get().data(), ImGuiTableColumnFlags_None, 0.7f);
            ImGui::TableSetupColumn(Localization::Generic::Type.Get().data(), ImGuiTableColumnFlags_None, 0.3f);
            ImGui::TableHeadersRow();

            for (const Assets::AssetMetaData& assetMeta : availableAssets)
            {
                if (assetMeta.Type != assetType)
                    continue;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                bool isSelected = assetMeta.Id == outAssetId;
                if (ImGui::Selectable(assetMeta.GetName().data(), isSelected, ImGuiSelectableFlags_SpanAllColumns))
                {
                    hasModified = outAssetId != assetMeta.Id;
                    outAssetId = assetMeta.Id;
                    loc_HasFocus = true;
                    loc_SearchString.clear();
                    ImGui::CloseCurrentPopup();
                    break;
                }

                ImGui::TableSetColumnIndex(1);
                ImGui::TextEx("type");
            }

            ImGui::EndTable();
            ImGui::EndPopup();
        }

        Layout::EndHorizontal();

        return hasModified;
    }
}
#endif