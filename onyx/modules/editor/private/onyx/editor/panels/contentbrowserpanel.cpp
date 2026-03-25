#include <onyx/editor/panels/contentbrowserpanel.h>

#include <onyx/editor/assets/importer/textureimporter.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stacklayout.h>
#include <onyx/ui/controls/button.h>

namespace onyx::editor {
ContentBrowserPanel::ContentBrowserPanel( assets::AssetSystem& assetSystem )
    : m_ContentDirectoryWatcher( "", true ) {
    BuildContentDirectoryTree( "", m_ContentDirectoryTree );

    assets::AssetId closedId(
        file_system::Path::GetFullPath( "textures/editor/icons/contentbrowser/folder_closed.png" ) );
    assetSystem.getAsset( closedId, m_FolderClosedAsset );

    assets::AssetId openId( file_system::Path::GetFullPath( "textures/editor/icons/contentbrowser/folder_open.png" ) );
    assetSystem.getAsset( openId, m_FolderOpenAsset );

    assets::AssetId closedSelectedId(
        file_system::Path::GetFullPath( "textures/editor/icons/contentbrowser/folder_closed_selected.png" ) );
    assetSystem.getAsset( closedSelectedId, m_FolderSelectedClosedAsset );

    assets::AssetId openSelectedId(
        file_system::Path::GetFullPath( "textures/editor/icons/contentbrowser/folder_open_selected.png" ) );
    assetSystem.getAsset( openSelectedId, m_FolderSelectedOpenAsset );
}

void ContentBrowserPanel::Render() {
    ImGui::SetNextWindowSize( ImVec2( 800, 600 ), ImGuiCond_FirstUseEver );
    ImGui::Begin( "Content Browser", &m_IsOpen );

    if ( m_FolderClosedAsset->isLoaded() )
        ImGui::ImageButton( "folder", m_FolderClosedAsset->GetTextureHandle().Texture->GetIndex(), ImVec2( 64, 64 ) );

    if ( m_FolderOpenAsset->isLoaded() )
        ImGui::ImageButton( "folder", m_FolderOpenAsset->GetTextureHandle().Texture->GetIndex(), ImVec2( 64, 64 ) );

    if ( m_FolderSelectedClosedAsset->isLoaded() )
        ImGui::ImageButton( "folder",
                            m_FolderSelectedClosedAsset->GetTextureHandle().Texture->GetIndex(),
                            ImVec2( 64, 64 ) );

    if ( m_FolderSelectedOpenAsset->isLoaded() )
        ImGui::ImageButton( "folder",
                            m_FolderSelectedOpenAsset->GetTextureHandle().Texture->GetIndex(),
                            ImVec2( 64, 64 ) );

    ImGui::Columns( 2 );

    for ( const auto& contentDirectoryItem : m_ContentDirectoryTree.Items ) {
        DrawContentDirectoryItem( contentDirectoryItem );
    }

    ImGui::NextColumn();

    DrawSelectedDirectoryContent();

    ImGui::EndColumns();

    if ( ImGui::BeginPopupContextWindow( "Context" ) ) {
        if ( ImGui::BeginMenu( "Create New..." ) ) {
            if ( ImGui::MenuItem( "Material" ) ) {
                ImGui::CloseCurrentPopup();
            }

            if ( ImGui::MenuItem( "Scene" ) ) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndMenu();
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}

void ContentBrowserPanel::BuildContentDirectoryTree( const FilePath& path, DirectoryInfo& parentDirectoryInfo ) {
    using directory_iterator = std::filesystem::directory_iterator;

    for ( const std::filesystem::directory_entry& entry : directory_iterator( path ) ) {
        DirectoryInfo& childItem = parentDirectoryInfo.Items.emplace_back();
        childItem.Id = hash::fnV1aHash< uint64_t >( entry.path().string() );
        childItem.Path = entry.path();

        if ( entry.is_directory() ) {
            BuildContentDirectoryTree( entry.path(), childItem );
        }
    }
}

void ContentBrowserPanel::DrawContentDirectoryItem( const DirectoryInfo& directoryItem ) {
    if ( directoryItem.Items.empty() )
        return;

    if ( ImGui::TreeNodeEx( directoryItem.Path.filename().string().data() ) ) {
        if ( ImGui::IsItemToggledOpen() ) {
            m_SelectedDirectory = &directoryItem;
        }

        for ( const DirectoryInfo& childItem : directoryItem.Items )
            DrawContentDirectoryItem( childItem );

        ImGui::TreePop();
    }
}

void ContentBrowserPanel::DrawSelectedDirectoryContent() {
    if ( m_SelectedDirectory == nullptr )
        return;

    // TODO: Hacked together to get materials quickly done

    ImGui::BeginHorizontal( "##selectedDirContent" );
    for ( const DirectoryInfo& childItem : m_SelectedDirectory->Items ) {
        if ( ui::Button( childItem.Path.stem().string().c_str() ) ) {
            if ( childItem.Path.extension().string() == ".png" ) {
                assets::AssetMetaData metaData;
                TextureImporter importer;
                importer.Import( childItem.Path, metaData );
            }
        }
    }
    ImGui::EndHorizontal();
}
} // namespace onyx::editor
