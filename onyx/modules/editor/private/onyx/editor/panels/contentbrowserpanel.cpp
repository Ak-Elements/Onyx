#include <onyx/editor/panels/contentbrowserpanel.h>

#include <onyx/editor/assets/importer/textureimporter.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stacklayout.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/ui/controls/button.h>
#include <onyx/ui/scopedcolor.h>
#include <onyx/ui/scopedid.h>
#include <onyx/ui/scopedstyle.h>
#include <onyx/ui/widgets.h>

namespace onyx::editor {

ContentBrowserPanel::ContentBrowserPanel()
    : m_contentDirectoryWatcher( "", true ) {}

void ContentBrowserPanel::onOpen() {
    for( auto&& [ id, mountPoint ] : file_system::path::getMountPoints() ) {
        if( file_system::path::TmpMountPointId == id )
            continue;

        auto& mountPointEntry = m_contentDirectoryTree.Items.emplace_back();
        mountPointEntry.Id = hash::fnV1aHash< uint64_t >( mountPoint.Prefix );
        mountPointEntry.Path = mountPoint.Path;
        mountPointEntry.Label = mountPoint.Prefix.substr( 0, mountPoint.Prefix.size() - 2 );
        buildContentDirectoryTree( mountPoint.Path, mountPointEntry );
    }

    assets::AssetSystem& assetSystem = getEngineSystem< assets::AssetSystem >();
    assetSystem.getAsset( "engine:/textures/editor/icons/contentbrowser/folder_closed.png", m_folderClosedAsset );
    assetSystem.getAsset( "engine:/textures/editor/icons/contentbrowser/folder_open.png", m_folderOpenAsset );
    assetSystem.getAsset( "engine:/textures/editor/icons/contentbrowser/folder_closed_selected.png",
                          m_folderSelectedClosedAsset );
    assetSystem.getAsset( "engine:/textures/editor/icons/contentbrowser/folder_open_selected.png",
                          m_folderSelectedOpenAsset );
    assetSystem.getAsset( "engine:/textures/editor/icons/contentbrowser/folder_closed.png", m_fileGenericAsset );
}

void ContentBrowserPanel::onClose() {}

void ContentBrowserPanel::onRender( ui::ImGuiSystem& imguiSystem ) {
    drawToolbar();

    ImGui::Separator();

    ImGui::Columns( 2, "ContentBrowserColumns", true );
    ImGui::SetColumnWidth( 0, 220.0f );

    for( const auto& item : m_contentDirectoryTree.Items )
        drawContentDirectoryItem( item );

    ImGui::NextColumn();

    drawRightPanel();

    ImGui::EndColumns();

    drawContextMenu();
}

void ContentBrowserPanel::buildContentDirectoryTree( const FilePath& path, DirectoryInfo& parentDirectoryInfo ) {
    using DirectoryIterator = std::filesystem::directory_iterator;

    for( const std::filesystem::directory_entry& entry : DirectoryIterator( path ) ) {
        DirectoryInfo& child = parentDirectoryInfo.Items.emplace_back();
        child.Id = hash::fnV1aHash< uint64_t >( entry.path().string() );
        child.Path = entry.path();
        child.Label = entry.path().filename().string();
        child.Extension = entry.path().extension().string();

        if( entry.is_directory() )
            buildContentDirectoryTree( entry.path(), child );
    }
}

void ContentBrowserPanel::drawToolbar() {
    // ui::ScopedImGuiStyle style{ { ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) } };
    ImGui::BeginHorizontal( "##top", ImVec2( ImGui::GetContentRegionAvail().x, 0 ), 1.0f );
    drawBreadcrumbs();

    ImGui::Spring( 1.0f );

    auto& localization = getEngineSystem< localization::LocalizationModule >();

    static HashSet< uint32_t > filteredTypes;
    DynamicArray< StringView > assetTypes;
    for( const StringId32& assetType : getEngineSystem< assets::AssetSystem >().getAssetTypes() ) {
        assetTypes.emplace_back( localization.GetLocalized( localization::LocalizationId( assetType ) ).Get() );
    }

    float lineHeight = ImGui::GetFrameHeight();
    float itemSpacing = ImGui::GetStyle().FramePadding.y;
    ui::iconButton( "filter", ImVec2( lineHeight, lineHeight ), [ & ]( ImDrawList* drawList, ui::ButtonState state ) {
        bool isHeldOrPressed = state == ui::ButtonState::Held || state == ui::ButtonState::Pressed;
        uint32_t iconColor = filteredTypes.empty() || ( isHeldOrPressed ) ? ImGui::GetColorU32( ImGuiCol_Text, 0.6f )
                                                                          : ImGui::GetColorU32( ImGuiCol_ButtonActive );
        ui::drawFilledFilterIcon( drawList,
                                  ImVec2( itemSpacing, itemSpacing ),
                                  lineHeight - ( itemSpacing * 2.0f ),
                                  1.0f,
                                  iconColor );
    } );

    if( ImGui::BeginPopupContextItem( "F", ImGuiPopupFlags_MouseButtonLeft ) ) {
        ui::drawMultiSelect( "filter", assetTypes, filteredTypes );

        if( ImGui::IsWindowFocused() == false && ImGui::IsKeyPressed( ImGuiKey_Escape ) )
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    bool focus = false;
    ImGui::SetNextItemWidth( 180.0f );
    ui::drawSearchBar( m_search, "Search...", focus );

    ImGui::SetNextItemWidth( 80.0f );
    ImGui::SliderFloat( "##TileSize", &m_tileSize, 48.0f, 128.0f, "%.0f" );

    ImGui::EndHorizontal();
}

void ContentBrowserPanel::drawBreadcrumbs() {
    if( m_selectedDirectory == nullptr ) {
        return;
    }

    ui::ScopedImGuiStyle style{ { ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) } };
    std::vector< const DirectoryInfo* > chain;
    for( const DirectoryInfo& root : m_contentDirectoryTree.Items ) {
        if( findPathToDirectory( root, m_selectedDirectory, chain ) )
            break;
    }

    for( size_t i = 0; i < chain.size(); ++i ) {
        if( i > 0 ) {
            ImGui::TextDisabled( "/" );
        }

        ui::ScopedImGuiColor buttonColors{ { ImGuiCol_Button, 0 } };
        ui::ScopedImGuiId id( static_cast< uint32_t >( chain[ i ]->Id ) );
        if( ImGui::SmallButton( chain[ i ]->Label.c_str() ) )
            navigateTo( chain[ i ] );
    }
}

bool ContentBrowserPanel::findPathToDirectory( const DirectoryInfo& current,
                                               const DirectoryInfo* target,
                                               std::vector< const DirectoryInfo* >& chain ) const {
    if( &current == target ) {
        chain.push_back( &current );
        return true;
    }

    for( const DirectoryInfo& child : current.Items ) {
        if( findPathToDirectory( child, target, chain ) ) {
            chain.insert( chain.begin(), &current );
            return true;
        }
    }

    return false;
}

void ContentBrowserPanel::drawContentDirectoryItem( const DirectoryInfo& directoryItem ) {
    if( !std::filesystem::is_directory( directoryItem.Path ) )
        return;

    const bool isSelected = ( m_selectedDirectory == &directoryItem );
    const bool isOpen = m_openDirectories.contains( directoryItem.Id );
    const bool hasChildren = std::ranges::any_of( directoryItem.Items, []( const DirectoryInfo& item ) {
        return std::filesystem::is_directory( item.Path );
    } );

    auto* iconAsset = isSelected ? ( isOpen ? &m_folderSelectedOpenAsset : &m_folderSelectedClosedAsset )
                                 : ( isOpen ? &m_folderOpenAsset : &m_folderClosedAsset );

    ImGui::PushID( static_cast< int >( directoryItem.Id ) );

    auto pixelSnap = []( ImVec2 v ) -> ImVec2 { return { IM_FLOOR( v.x ), IM_FLOOR( v.y ) }; };

    const float rowHeight = IM_FLOOR( ImGui::GetTextLineHeight() );
    const ImVec2 iconSize( 16.0f, 16.0f );
    const float rowPadding = IM_FLOOR( ( rowHeight - iconSize.y ) * 0.5f );
    const float fullWidth = ImGui::GetContentRegionAvail().x;
    const ImVec2 rowSize( std::max( 1.0f, fullWidth ), rowHeight );

    const ImVec2 rowMin = pixelSnap( ImGui::GetCursorScreenPos() );

    bool clicked = ImGui::InvisibleButton( "##row", rowSize );

    ImDrawList* dl = ImGui::GetWindowDrawList();
    const ImVec2 rowMax( rowMin.x + rowSize.x, rowMin.y + rowSize.y );

    if( isSelected )
        dl->AddRectFilled( rowMin, rowMax, ImGui::GetColorU32( ImGuiCol_HeaderActive ) );
    else if( ImGui::IsItemHovered() )
        dl->AddRectFilled( rowMin, rowMax, ImGui::GetColorU32( ImGuiCol_HeaderHovered ) );

    const ImVec2 iconMin = pixelSnap( { rowMin.x, rowMin.y + rowPadding } );
    const ImVec2 iconMax = pixelSnap( { iconMin.x + iconSize.x, iconMin.y + iconSize.y } );

    if( iconAsset && iconAsset->isLoaded() )
        dl->AddImage( ( *iconAsset )->getTextureHandle().Texture->GetIndex(), iconMin, iconMax );
    else
        dl->AddText( iconMin, ImGui::GetColorU32( ImGuiCol_Text ), isOpen ? "v " : "> " );

    const ImVec2 labelPos( iconMax.x + ImGui::GetStyle().ItemSpacing.x, rowMin.y );
    dl->AddText( labelPos, ImGui::GetColorU32( ImGuiCol_Text ), directoryItem.Label.data() );

    if( clicked ) {
        if( hasChildren ) {
            if( isOpen )
                m_openDirectories.erase( directoryItem.Id );
            else
                m_openDirectories.insert( directoryItem.Id );
        }
        navigateTo( &directoryItem );
    }

    if( isOpen && hasChildren ) {
        ImGui::Indent( 8.0f );
        for( const DirectoryInfo& child : directoryItem.Items )
            drawContentDirectoryItem( child );
        ImGui::Unindent( 8.0f );
    }

    ImGui::PopID();
}

void ContentBrowserPanel::collectMatchingItems( const DirectoryInfo& directory,
                                                const std::string& lowerFilter,
                                                std::vector< const DirectoryInfo* >& results ) const {
    for( const DirectoryInfo& child : directory.Items ) {
        if( toLower( child.Label ).find( lowerFilter ) != std::string::npos )
            results.push_back( &child );

        if( std::filesystem::is_directory( child.Path ) )
            collectMatchingItems( child, lowerFilter, results );
    }
}

void ContentBrowserPanel::navigateTo( const DirectoryInfo* directory ) {
    if( m_selectedDirectory == directory )
        return;

    m_selectedDirectory = directory;
}

void ContentBrowserPanel::drawRightPanel() {
    if( m_selectedDirectory == nullptr )
        return;

    const float panelWidth = ImGui::GetContentRegionAvail().x;
    const float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
    const ImVec2 tileSize( m_tileSize, m_tileSize + ImGui::GetTextLineHeightWithSpacing() );
    const int maxColumns = std::max( 1, static_cast< int >( panelWidth / ( m_tileSize + itemSpacing ) ) );

    const std::string lowerFilter = toLower( StringView( m_search ) );
    const bool isFiltering = !lowerFilter.empty();

    std::vector< const DirectoryInfo* > itemsToShow;
    if( isFiltering ) {
        for( const DirectoryInfo& root : m_contentDirectoryTree.Items )
            collectMatchingItems( root, lowerFilter, itemsToShow );
    }

    const auto& items = isFiltering ? itemsToShow : [ & ]() -> std::vector< const DirectoryInfo* > {
        std::vector< const DirectoryInfo* > direct;
        for( const DirectoryInfo& child : m_selectedDirectory->Items )
            direct.push_back( &child );
        return direct;
    }();

    int column = 0;
    for( const DirectoryInfo* item : items ) {
        if( column > 0 && column < maxColumns )
            ImGui::SameLine();
        else
            column = 0;

        drawContentTile( *item, tileSize );
        ++column;
    }
}

void ContentBrowserPanel::drawContentTile( const DirectoryInfo& item, ImVec2 tileSize ) {
    const bool isDirectory = std::filesystem::is_directory( item.Path );
    const bool isSelected = ( m_selectedAsset == &item );

    ImGui::PushID( static_cast< int >( item.Id ) );
    ImGui::BeginGroup();

    const float imageSize = tileSize.x - 8.0f;

    ImGui::PushStyleColor( ImGuiCol_Button, isSelected ? ImGui::GetColorU32( ImGuiCol_HeaderActive ) : 0x00 );
    ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImGui::GetColorU32( ImGuiCol_HeaderHovered ) );

    bool doubleClicked = false;
    bool singleClicked = false;

    if( isDirectory ) {
        const bool iconReady = m_folderClosedAsset.isLoaded();
        if( iconReady ) {
            singleClicked = ImGui::ImageButton( "##tile",
                                                m_folderClosedAsset->getTextureHandle().Texture->GetIndex(),
                                                ImVec2( imageSize, imageSize ) );
        } else {
            singleClicked = ImGui::Button( "##tile", ImVec2( imageSize, imageSize ) );
        }
    } else {
        const bool iconReady = m_fileGenericAsset.isLoaded();
        if( iconReady ) {
            // singleClicked = ImGui::ImageButton( "##tile",
            //                                     m_fileGenericAsset->GetTextureHandle().Texture->GetIndex(),
            //                                     ImVec2( imageSize, imageSize ) );
            ui::drawFileIcon( ImGui::GetWindowDrawList(), ImVec2( 0, 0 ), imageSize, 1.0f, Color{ 255, 0, 0, 255 } );
            ImGui::Dummy( ImVec2( imageSize, imageSize ) );
        } else {
            singleClicked = ImGui::Button( "##tile", ImVec2( imageSize, imageSize ) );
        }
    }

    if( ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
        doubleClicked = true;

    ImGui::PopStyleColor( 2 );

    const float labelWidth = tileSize.x;
    ImGui::SetNextItemWidth( labelWidth );

    const std::string& label = item.Label;
    const float textWidth = ImGui::CalcTextSize( label.c_str() ).x;
    if( textWidth > labelWidth ) {
        ImGui::TextUnformatted( truncateLabel( label, labelWidth ).c_str() );
        if( ImGui::IsItemHovered() )
            ImGui::SetTooltip( "%s", label.c_str() );
    } else {
        const float offset = ( labelWidth - textWidth ) * 0.5f;
        ImGui::SetCursorPosX( ImGui::GetCursorPosX() + offset );
        ImGui::TextUnformatted( label.c_str() );
    }

    ImGui::EndGroup();
    ImGui::PopID();

    if( singleClicked && !doubleClicked )
        m_selectedAsset = &item;

    if( doubleClicked ) {
        if( isDirectory )
            navigateTo( &item );
        else {
            // Handle asset click
        }
    }
}

void ContentBrowserPanel::drawContextMenu() {
    if( ImGui::BeginPopupContextWindow( "ContentBrowserContext",
                                        ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems ) ) {
        if( ImGui::BeginMenu( "Create New..." ) ) {
            if( ImGui::MenuItem( "Material" ) )
                ImGui::CloseCurrentPopup();

            if( ImGui::MenuItem( "Scene" ) )
                ImGui::CloseCurrentPopup();

            ImGui::EndMenu();
        }

        ImGui::EndPopup();
    }
}

std::string ContentBrowserPanel::truncateLabel( const std::string& label, float maxWidth ) const {
    const std::string ellipsis = "...";
    const float ellipsisWidth = ImGui::CalcTextSize( ellipsis.c_str() ).x;

    std::string truncated = label;
    while( !truncated.empty() && ImGui::CalcTextSize( truncated.c_str() ).x + ellipsisWidth > maxWidth )
        truncated.pop_back();

    return truncated + ellipsis;
}

} // namespace onyx::editor
