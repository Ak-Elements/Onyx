#pragma once
#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/textureasset.h>
#include <onyx/filesystem/filewatcher.h>

namespace Onyx::Editor
{
    struct DirectoryInfo
    {
        onyxU64 Id; // hashed path
        FileSystem::Filepath Path;
        DynamicArray<DirectoryInfo> Items;
    };

    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel(Assets::AssetSystem& assetSystem);

        void Render();

    private:
        void BuildContentDirectoryTree(const FileSystem::Filepath& path, DirectoryInfo& parentDirectoryInfo);
        void DrawContentDirectoryItem(const DirectoryInfo& directoryItem);

        void DrawSelectedDirectoryContent();

    private:

        DirectoryInfo m_ContentDirectoryTree;
        const DirectoryInfo* m_SelectedDirectory = nullptr;
        
        FileSystem::FileWatcher m_ContentDirectoryWatcher;

        Reference<Graphics::TextureAsset> m_FolderClosedAsset;
        Reference<Graphics::TextureAsset> m_FolderOpenAsset;
        Reference<Graphics::TextureAsset> m_FolderSelectedClosedAsset;
        Reference<Graphics::TextureAsset> m_FolderSelectedOpenAsset;

        bool m_IsOpen = false;
    };
}
