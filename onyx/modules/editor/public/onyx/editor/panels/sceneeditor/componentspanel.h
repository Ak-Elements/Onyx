#pragma once
#include <onyx/ui/controls/treeview.h>

namespace Onyx::Entity
{
    class ComponentFactory;
}

namespace Onyx::Localization
{
    class LocalizationModule;
}

namespace Onyx::GameCore
{
    class Scene;
}

namespace Onyx::Editor::SceneEditor
{
    class SceneEditorWindow;

    class ComponentsPanel
    {
    public:
        ComponentsPanel(Localization::LocalizationModule& localizationModule);

        void Render(const Entity::ComponentFactory& componentFactory, GameCore::Scene& scene);

    private:
        void DrawSelectedEntityComponents(const Entity::ComponentFactory& componentFactory, GameCore::Scene& scene);
        void DrawCreateComponentContextMenu(const Entity::ComponentFactory& componentFactory, GameCore::Scene& scene);

        Ui::TreeItem BuildComponentTree(const Entity::ComponentFactory& componentFactor, GameCore::Scene& scene, StringView searchString) const;

    private:
        Localization::LocalizationModule* m_LocalizationModule = nullptr;

        Ui::TreeItem m_ContextMenuTreeRoot;
        bool m_ShowAll = false;
    };
}
