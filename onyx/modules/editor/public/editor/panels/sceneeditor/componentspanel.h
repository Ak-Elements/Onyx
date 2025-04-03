#pragma once

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
        ComponentsPanel();
        void Render(GameCore::Scene& scene);

    private:
        void DrawSelectedEntityComponents(GameCore::Scene& scene);
        void DrawCreateComponentContextMenu(GameCore::Scene& scene);

    private:
        HashMap<onyxU32, String> m_Components;

        bool m_ShowAll = false;
    };
}
