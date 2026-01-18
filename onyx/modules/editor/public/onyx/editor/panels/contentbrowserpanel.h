#pragma once
#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/textureasset.h>
#include <onyx/filesystem/filewatcher.h>

namespace Onyx::Editor
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
        ContentBrowserPanel(Assets::AssetSystem& assetSystem);

        void Render();

    private:
        void BuildContentDirectoryTree(const FilePath& path, DirectoryInfo& parentDirectoryInfo);
        void DrawContentDirectoryItem(const DirectoryInfo& directoryItem);

        void DrawSelectedDirectoryContent();

    private:

        DirectoryInfo m_ContentDirectoryTree;
        const DirectoryInfo* m_SelectedDirectory = nullptr;
        
        FileSystem::FileWatcher m_ContentDirectoryWatcher;

        Assets::AssetHandle<Graphics::TextureAsset> m_FolderClosedAsset;
        Assets::AssetHandle<Graphics::TextureAsset> m_FolderOpenAsset;
        Assets::AssetHandle<Graphics::TextureAsset> m_FolderSelectedClosedAsset;
        Assets::AssetHandle<Graphics::TextureAsset> m_FolderSelectedOpenAsset;

        bool m_IsOpen = false;
    };
}
