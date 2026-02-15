#pragma once

#include <onyx/ui/imguiwindow.h>

#include <onyx/assets/assetid.h>
#include <onyx/ui/controls/treeview.h>

namespace Onyx::Entity
{
    class EntityRegistry;
    class ComponentFactory;
}

namespace Onyx::Localization
{
    class LocalizationModule;
}

namespace Onyx::GameCore
{
    class Scene;
    class GameCoreSystem;
}

namespace Onyx::Editor
{
    class ICommandGraph;
}

namespace Onyx::Editor::SceneEditor
{
    class ComponentsPanel : public Ui::ImGuiWindow
    {
    public:
        static constexpr StringView WindowId = "ComponentsPanel";
        static constexpr StringView WindowCategory = "Panel";

        void SetCommandGraph(ICommandGraph& commandGraph) { m_CommandGraph = &commandGraph; }
        
    private:
        void OnOpen() override;
        void OnClose() override;

        void OnRender(Ui::ImGuiSystem& imguiSystem) override;
        
        void DrawSelectedEntityComponents(Entity::EntityRegistry& registry,
            Assets::AssetId sceneId, 
            GameCore::GameCoreSystem& gameCoreSystem,
            const Localization::LocalizationModule& localizationModule);

        void DrawCreateComponentContextMenu(Entity::EntityRegistry& registry,
            Assets::AssetId sceneId,
            GameCore::GameCoreSystem& gameCoreSystem, 
            const Localization::LocalizationModule& localizationModule);

        Ui::TreeItem BuildComponentTree(StringView searchString, Entity::EntityRegistry& registry, Assets::AssetId sceneId, GameCore::GameCoreSystem& gameCoreSystem, const Localization::LocalizationModule& localizationModule) const;

    private:
        Ui::TreeItem m_ContextMenuTreeRoot;
        ICommandGraph* m_CommandGraph = nullptr;

        bool m_ShowAll = false;
    };
}
