#pragma once
#include <onyx/graphics/graphicshandles.h>

namespace Onyx::GameCore
{
    class Scene;
}

namespace Onyx::Editor::SceneEditor
{
    class SceneEditorWindow;

    class TerrainPanel
    {
    public:
        TerrainPanel(Graphics::GraphicsApi& graphicsApi);

        void SetSceneViewPanelId(onyxU32 panelId) {
            m_SceneViewPanelId = panelId;
            m_SceneViewPanelId = 0;
        }

        void Render(GameCore::Scene& scene);

    private:
        Graphics::GraphicsApi& m_GraphicsApi;

        onyxU32 m_SceneViewPanelId = 0;

        Graphics::ShaderHandle m_EditTerrainShader;
        Graphics::ShaderEffectHandle m_EditTerrainShaderEffect;

        Graphics::ShaderHandle m_GenerateMeshShader;
        Graphics::ShaderEffectHandle m_GenerateMeshShaderEffect;
    };
}
