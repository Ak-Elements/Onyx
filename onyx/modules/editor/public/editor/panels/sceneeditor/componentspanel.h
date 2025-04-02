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
        void Render(GameCore::Scene& scene);

    private:
        void DrawSelectedEntityComponents();
        void DrawCreateComponentContextMenu();

    private:
        HashMap<onyxU32, String> m_Components;

        bool m_ShowAll = false;
    };
}
