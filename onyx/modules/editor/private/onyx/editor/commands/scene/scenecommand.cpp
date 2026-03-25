#include <onyx/editor/commands/scene/scenecommand.h>

#include <onyx/assets/assethandle.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>

namespace onyx::editor {
SceneCommand::SceneCommand( StringId32 id, assets::AssetId sceneId, game_core::GameCoreSystem& gameCoreSystem )
    : ICommand( id )
    , m_SceneId( sceneId )
    , m_GameCoreSystem( &gameCoreSystem ) {}

game_core::Scene& SceneCommand::GetScene() {
    ONYX_ASSERT( m_GameCoreSystem != nullptr );
    assets::AssetHandle< game_core::Scene > scene = m_GameCoreSystem->GetScene();
    ONYX_ASSERT( scene.getId() == m_SceneId, "Active scene does not match." );
    return *scene;
}

const ecs::ComponentFactory& SceneCommand::GetComponentFactory() {
    ONYX_ASSERT( m_GameCoreSystem != nullptr );
    return m_GameCoreSystem->GetComponentFactory();
}
} // namespace onyx::editor