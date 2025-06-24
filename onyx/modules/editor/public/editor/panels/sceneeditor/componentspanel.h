#pragma once

namespace Onyx::Entity
{
    class ComponentFactory;
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
        void Render(const Entity::ComponentFactory& componentFactory, GameCore::Scene& scene);

    private:
        void DrawSelectedEntityComponents(const Entity::ComponentFactory& componentFactory, GameCore::Scene& scene);
        void DrawCreateComponentContextMenu(const Entity::ComponentFactory& componentFactory, GameCore::Scene& scene);

    private:
        bool m_ShowAll = false;
    };
}
