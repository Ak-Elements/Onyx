#pragma once
#include <onyx/localization/localizedstring.h>

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
        Localization::LocalizedString m_ShowAllLabel;
        Localization::LocalizedString m_SearchHintLabel;

        Localization::LocalizationModule* m_LocalizationModule;

        bool m_ShowAll = false;
    };
}
