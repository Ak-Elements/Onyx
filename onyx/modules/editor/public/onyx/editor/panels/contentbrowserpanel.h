#pragma once
#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/textureasset.h>
#include <onyx/filesystem/filewatcher.h>

namespace onyx::editor
{
    struct DirectoryInfo
    {
        onyxU64 Id; // hashed path
        FilePath Path;
        DynamicArray<DirectoryInfo> Items;
    };

    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel(assets::AssetSystem& assetSystem);

        void Render();

    private:
        void BuildContentDirectoryTree(const FilePath& path, DirectoryInfo& parentDirectoryInfo);
        void DrawContentDirectoryItem(const DirectoryInfo& directoryItem);

        void DrawSelectedDirectoryContent();

    private:

        DirectoryInfo m_ContentDirectoryTree;
        const DirectoryInfo* m_SelectedDirectory = nullptr;
        
        file_system::FileWatcher m_ContentDirectoryWatcher;

        assets::AssetHandle<graphics::TextureAsset> m_FolderClosedAsset;
        assets::AssetHandle<graphics::TextureAsset> m_FolderOpenAsset;
        assets::AssetHandle<graphics::TextureAsset> m_FolderSelectedClosedAsset;
        assets::AssetHandle<graphics::TextureAsset> m_FolderSelectedOpenAsset;

        bool m_IsOpen = false;
    };
}
