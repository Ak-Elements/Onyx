#include <onyx/editor/commands/scene/scenecommand.h>

#include <onyx/assets/assethandle.h>

#include <onyx/gamecore/gamecoresystem.h>
#include <onyx/gamecore/scene/scene.h>

namespace onyx::editor {
SceneCommand::SceneCommand( StringId32 id, assets::AssetId sceneId, game_core::GameCoreSystem& gameCoreSystem )
    : ICommand( id )
    , m_sceneId( sceneId )
    , m_gameCoreSystem( &gameCoreSystem ) {}

game_core::Scene& SceneCommand::getScene() {
    ONYX_ASSERT( m_gameCoreSystem != nullptr );
    assets::AssetHandle< game_core::Scene > scene = m_gameCoreSystem->getScene();
    ONYX_ASSERT( scene.getId() == m_sceneId, "Active scene does not match." );
    return *scene;
}

const ecs::ComponentFactory& SceneCommand::getComponentFactory() {
    ONYX_ASSERT( m_gameCoreSystem != nullptr );
    return m_gameCoreSystem->getComponentFactory();
}
} // namespace onyx::editor
