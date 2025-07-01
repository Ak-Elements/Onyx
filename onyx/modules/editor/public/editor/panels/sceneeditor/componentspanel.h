#pragma once

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

        void Render(GameCore::Scene& scene);

    private:
        void DrawSelectedEntityComponents(GameCore::Scene& scene);
        void DrawCreateComponentContextMenu(GameCore::Scene& scene);

    private:
        Localization::LocalizationModule* m_LocalizationModule = nullptr;
        bool m_ShowAll = false;
    };
}
