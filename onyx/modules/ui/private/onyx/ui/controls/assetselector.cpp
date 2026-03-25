#include <onyx/ui/controls/assetselector.h>
#include <onyx/ui/scopedstyle.h>

#if ONYX_IS_EDITOR

#define IMGUI_DEFINE_MATH_OPERATORS
#include <onyx/assets/assetsystem.h>

#include <onyx/ui/controls/button.h>
#include <onyx/ui/controls/combobox.h>
#include <onyx/ui/layout/imguilayout.h>
#include <onyx/ui/widgets.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <onyx/localization/localization.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/localization/localizedstring.h>
#include <onyx/ui/imguisystem.h>
#include <onyx/ui/ui_localization.h>

namespace onyx::ui {
namespace {
bool loc_HasFocus = true;
String loc_SearchString;
} // namespace

bool AssetSelector( const assets::AssetSystem& assetSystem, assets::AssetType assetType, assets::AssetId& outAssetId ) {
    bool hasModified = false;
    DynamicArray< assets::AssetMetaData > availableAssets;

    String assetName;
    if ( outAssetId.isValid() ) {
        Optional< const assets::AssetMetaData* > assetMetaOptional = assetSystem.tryGetAssetMeta( outAssetId );
        const assets::AssetMetaData* assetMeta = assetMetaOptional.value_or( nullptr );
        if ( assetMeta )
            assetName = assetMeta->getName();
        else {
            assetName = onyx::localization::generic::None.Get();
        }
    } else {
        assetName = onyx::localization::generic::None.Get();
    }

    layout::BeginHorizontal( "#AssetControl" );

    ScopedImGuiStyle styleOverride{ ImGuiStyleVar_FrameBorderSize, 1.0f };
    if ( BeginCombobox( "##selector", assetName ) ) {
        DrawSearchBar( loc_SearchString, onyx::localization::generic::Search.Get(), loc_HasFocus );
        StringView searchString = trim( loc_SearchString );

        if ( availableAssets.empty() ) {
            availableAssets = assetSystem.getAvailableAssets( assetType );
        }

        for ( const assets::AssetMetaData& assetMeta : availableAssets ) {
            if ( ( searchString.empty() == false ) &&
                 ignoreCaseFind( assetMeta.getName(), searchString ) == String::npos ) {
                continue;
            }

            bool isSelected = assetMeta.Id == outAssetId;
            if ( Selectable( assetMeta.getName(), isSelected, true ) ) {
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
    ImVec2 buttonSize( 16.0f, 16.0f );
    ImGuiID browseButtonId = ImGui::GetID( "BrowseButton" );

    // ImVec2 currentPos = ImGui::GetCursorScreenPos();
    // const float32 currentHeight = ImGui::GetFrameHeight();
    // currentPos.y += currentHeight * 0.5f - buttonSize.y * 0.5f;
    ImRect bb( ImGui::GetCursorScreenPos() + style.FramePadding,
               ImGui::GetCursorScreenPos() + buttonSize + style.FramePadding );
    ButtonState state = ui::ButtonBehavior( browseButtonId, bb );

    if ( state == ButtonState::Pressed ) {
        ImGui::OpenPopup( "###AssetBrowser" );
    }
    ImGui::ItemAdd( bb, browseButtonId );

    if ( ( state == ButtonState::Held ) || ( state == ButtonState::Hovered ) ) {
        uint32_t buttonBackgroundColor = state == ButtonState::Held ? ImGui::GetColorU32( ImGuiCol_ButtonActive )
                                                                    : ImGui::GetColorU32( ImGuiCol_ButtonHovered );
        ImGui::GetForegroundDrawList()->AddRectFilled( bb.Min - style.FramePadding,
                                                       bb.Max + style.FramePadding,
                                                       buttonBackgroundColor );
    }

    // TODO: Move to style
    // TODO: Find held & hovered color
    uint32_t folderColor = 0xFF666666;
    uint32_t lidColor = state == ButtonState::Held      ? 0xFFFF7929
                        : state == ButtonState::Hovered ? 0xFFFF7929
                                                        : 0xFF888888;
    DrawFolderIcon( ImGui::GetForegroundDrawList(), style.FramePadding, buttonSize.x, 1.0f, folderColor, lidColor );

    ImGui::SetNextWindowSize( ImVec2( 800, 600 ), ImGuiCond_Appearing );
    bool isOpen = true;
    StringView modalName = format::format( "{}###AssetBrowser", ui::localization::asset_selector::modal::Title );
    if ( ImGui::BeginPopupModal( modalName.data(), &isOpen ) ) {
        if ( availableAssets.empty() ) {
            availableAssets = assetSystem.getAvailableAssets( assetType );
        }

        ImGui::BeginTable( "##assetlist",
                           2,
                           ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable |
                               ImGuiTableFlags_Reorderable );
        ImGui::TableSetupColumn( ::onyx::localization::generic::File.Get().data(), ImGuiTableColumnFlags_None, 0.7f );
        ImGui::TableSetupColumn( ::onyx::localization::generic::Type.Get().data(), ImGuiTableColumnFlags_None, 0.3f );
        ImGui::TableHeadersRow();

        for ( const assets::AssetMetaData& assetMeta : availableAssets ) {
            if ( assetMeta.Type != assetType )
                continue;

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex( 0 );
            bool isSelected = assetMeta.Id == outAssetId;
            if ( ImGui::Selectable( assetMeta.getName().data(), isSelected, ImGuiSelectableFlags_SpanAllColumns ) ) {
                hasModified = outAssetId != assetMeta.Id;
                outAssetId = assetMeta.Id;
                loc_HasFocus = true;
                loc_SearchString.clear();
                ImGui::CloseCurrentPopup();
                break;
            }

            ImGui::TableSetColumnIndex( 1 );
            ImGui::TextEx( "type" );
        }

        ImGui::EndTable();
        ImGui::EndPopup();
    }

    layout::EndHorizontal();

    return hasModified;
}
} // namespace onyx::ui
#endif