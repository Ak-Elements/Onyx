#include <editor/panels/contentbrowserpanel.h>

#include <editor/assets/importer/textureimporter.h>

#include <imgui.h>
#include <imgui_stacklayout.h>
#include <imgui_internal.h>

namespace Onyx::Editor
{
    ContentBrowserPanel::ContentBrowserPanel(Assets::AssetSystem& assetSystem)
        : m_ContentDirectoryWatcher("", true)
    {
        BuildContentDirectoryTree("", m_ContentDirectoryTree);

        Assets::AssetId closedId = static_cast<Assets::AssetId>(Hash::FNV1aHash64(FileSystem::Path::GetFullPath("textures/editor/icons/contentbrowser/folder_closed.png")));
        assetSystem.GetAsset(closedId, m_FolderClosedAsset);

        Assets::AssetId openId = static_cast<Assets::AssetId>(Hash::FNV1aHash64(FileSystem::Path::GetFullPath("textures/editor/icons/contentbrowser/folder_open.png")));
        assetSystem.GetAsset(openId, m_FolderOpenAsset);

        Assets::AssetId closedSelectedId = static_cast<Assets::AssetId>(Hash::FNV1aHash64(FileSystem::Path::GetFullPath("textures/editor/icons/contentbrowser/folder_closed_selected.png")));
        assetSystem.GetAsset(closedSelectedId, m_FolderSelectedClosedAsset);

        Assets::AssetId openSelectedId = static_cast<Assets::AssetId>(Hash::FNV1aHash64(FileSystem::Path::GetFullPath("textures/editor/icons/contentbrowser/folder_open_selected.png")));
        assetSystem.GetAsset(openSelectedId, m_FolderSelectedOpenAsset);
    }

    void ContentBrowserPanel::Render()
    {
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        ImGui::Begin("Content Browser", &m_IsOpen);

        if (m_FolderClosedAsset->IsLoaded())
            ImGui::ImageButton("folder", m_FolderClosedAsset->GetTextureHandle().Texture->GetIndex(), ImVec2(64,64));

        if (m_FolderOpenAsset->IsLoaded())
            ImGui::ImageButton("folder", m_FolderOpenAsset->GetTextureHandle().Texture->GetIndex(), ImVec2(64, 64));

        if (m_FolderSelectedClosedAsset->IsLoaded())
            ImGui::ImageButton("folder", m_FolderSelectedClosedAsset->GetTextureHandle().Texture->GetIndex(), ImVec2(64, 64));

        if (m_FolderSelectedOpenAsset->IsLoaded())
            ImGui::ImageButton("folder", m_FolderSelectedOpenAsset->GetTextureHandle().Texture->GetIndex(), ImVec2(64, 64));

        ImGui::Columns(2);

        for (const auto& contentDirectoryItem : m_ContentDirectoryTree.Items)
        {
            DrawContentDirectoryItem(contentDirectoryItem);
        }

        ImGui::NextColumn();

        DrawSelectedDirectoryContent();

        ImGui::EndColumns();

        if (ImGui::BeginPopupContextWindow("Context"))
        {
            if (ImGui::BeginMenu("Create New..."))
            {
                if (ImGui::MenuItem("Material"))
                {
                    ImGui::CloseCurrentPopup();
                }
                
                if (ImGui::MenuItem("Scene"))
                {
                    ImGui::CloseCurrentPopup();
                }

               ImGui::EndMenu();
            }

            ImGui::EndPopup();
        }


        ImGui::End();
    }

    void ContentBrowserPanel::BuildContentDirectoryTree(const FileSystem::Filepath& path, DirectoryInfo& parentDirectoryInfo)
    {
        using directory_iterator = std::filesystem::directory_iterator;

        for (const std::filesystem::directory_entry& entry : directory_iterator(path))
        {
            DirectoryInfo& childItem = parentDirectoryInfo.Items.emplace_back();
            childItem.Id = Hash::FNV1aHash64(entry.path().string());
            childItem.Path = entry.path();

            if (entry.is_directory())
            {
                BuildContentDirectoryTree(entry.path(), childItem);
            }
        }
    }

    void ContentBrowserPanel::DrawContentDirectoryItem(const DirectoryInfo& directoryItem)
    {
        if (directoryItem.Items.empty())
            return;

        if (ImGui::TreeNodeEx(directoryItem.Path.filename().string().data()))
        {
            if (ImGui::IsItemToggledOpen())
            {
                m_SelectedDirectory = &directoryItem;
            }

            for (const DirectoryInfo& childItem : directoryItem.Items)
                DrawContentDirectoryItem(childItem);

            ImGui::TreePop();
        }

    }

    void ContentBrowserPanel::DrawSelectedDirectoryContent()
    {
        if (m_SelectedDirectory == nullptr)
            return;

        // TODO: Hacked together to get materials quickly done

        ImGui::BeginHorizontal("##selectedDirContent");
        for (const DirectoryInfo& childItem : m_SelectedDirectory->Items)
        {
            if (ImGui::Button(childItem.Path.stem().string().c_str()))
            {
                if (childItem.Path.extension().string() == ".png")
                {
                    Assets::AssetMetaData metaData;
                    TextureImporter importer;
                    importer.Import(childItem.Path, metaData);
                }
            }
        }
        ImGui::EndHorizontal();
    }
}
