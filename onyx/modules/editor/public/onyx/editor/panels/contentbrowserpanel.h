#pragma once
#include <onyx/ui/imguiwindow.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/filesystem/filewatcher.h>
#include <onyx/graphics/textureasset.h>

namespace onyx::editor {
struct DirectoryInfo {
    uint64_t Id = 0;
    FilePath Path;
    std::string Label;
    std::string Extension;
    std::vector< DirectoryInfo > Items;
};

struct DirectoryTree {
    std::vector< DirectoryInfo > Items;
};

class ContentBrowserPanel : public ui::ImGuiWindow {
  public:
    static constexpr StringView WindowId = "ContentBrowser";
    static constexpr StringView WindowCategory = "Panel";

    ContentBrowserPanel();

    // void setCommandGraph( ICommandGraph& commandGraph ) { m_commandGraph = &commandGraph; }

  private:
    void onOpen() override;
    void onClose() override;

    void onRender( ui::ImGuiSystem& imguiSystem ) override;

    // void onDeleteAction( const input_actions::InputActionEvent& deleteAction );

  private:
    void buildContentDirectoryTree( const FilePath& path, DirectoryInfo& parentDirectoryInfo );

    void drawToolbar();
    void drawBreadcrumbs();
    void drawContextMenu();
    void drawContentDirectoryItem( const DirectoryInfo& directoryItem );
    void drawRightPanel();
    void drawContentTile( const DirectoryInfo& item, ImVec2 tileSize );

    bool findPathToDirectory( const DirectoryInfo& current,
                              const DirectoryInfo* target,
                              std::vector< const DirectoryInfo* >& chain ) const;
    void collectMatchingItems( const DirectoryInfo& directory,
                               const std::string& lowerFilter,
                               std::vector< const DirectoryInfo* >& results ) const;

    void navigateTo( const DirectoryInfo* directory );

    std::string truncateLabel( const std::string& label, float maxWidth ) const;

  private:
    file_system::FileWatcher m_contentDirectoryWatcher;

    DirectoryTree m_contentDirectoryTree;
    const DirectoryInfo* m_selectedDirectory = nullptr;
    const DirectoryInfo* m_selectedAsset = nullptr;

    HashSet< uint64_t > m_openDirectories;

    float m_tileSize = 80.0f;
    String m_search;

    assets::AssetHandle< graphics::TextureAsset > m_folderClosedAsset;
    assets::AssetHandle< graphics::TextureAsset > m_folderOpenAsset;
    assets::AssetHandle< graphics::TextureAsset > m_folderSelectedClosedAsset;
    assets::AssetHandle< graphics::TextureAsset > m_folderSelectedOpenAsset;
    assets::AssetHandle< graphics::TextureAsset > m_fileGenericAsset;
};
} // namespace onyx::editor
