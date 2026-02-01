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
        void Render(const Entity::ComponentFactory& componentFactory, GameCore::Scene& scene, Localization::LocalizationModule& localizationModule);

    private:
        void DrawSelectedEntityComponents(const Entity::ComponentFactory& componentFactory, GameCore::Scene& scene, const Localization::LocalizationModule& localizationModule);
        void DrawCreateComponentContextMenu(const Entity::ComponentFactory& componentFactory, GameCore::Scene& scene, const Localization::LocalizationModule& localizationModule);

        Ui::TreeItem BuildComponentTree(const Entity::ComponentFactory& componentFactor, GameCore::Scene& scene, const Localization::LocalizationModule& localizationModule, StringView searchString) const;

    private:
        Ui::TreeItem m_ContextMenuTreeRoot;
        bool m_ShowAll = false;
    };
}
