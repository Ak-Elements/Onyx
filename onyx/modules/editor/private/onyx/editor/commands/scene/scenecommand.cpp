#include <onyx/editor/commands/scene/scenecommand.h>

#include <onyx/assets/assethandle.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>

namespace Onyx::Editor
{
    SceneCommand::SceneCommand(StringId32 id, Assets::AssetId sceneId, GameCore::GameCoreSystem& gameCoreSystem)
        : ICommand(id)
        , m_SceneId(sceneId)
        , m_GameCoreSystem(&gameCoreSystem)
    {
    }

    GameCore::Scene& SceneCommand::GetScene() 
    {
        ONYX_ASSERT(m_GameCoreSystem != nullptr);
        Assets::AssetHandle<GameCore::Scene> scene = m_GameCoreSystem->GetScene();
        ONYX_ASSERT(scene.GetId() == m_SceneId, "Active scene does not match.");
        return *scene;
    }

    const Entity::ComponentFactory& SceneCommand::GetComponentFactory()
    {
        ONYX_ASSERT(m_GameCoreSystem != nullptr);
        return m_GameCoreSystem->GetComponentFactory();
    }
}