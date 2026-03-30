#pragma once

#include <onyx/ui/imguiwindow.h>

#include <onyx/assets/assetid.h>
#include <onyx/ui/controls/treeview.h>

namespace onyx::ecs {
class EntityRegistry;
class ComponentFactory;
} // namespace onyx::ecs

namespace onyx::localization {
class LocalizationModule;
}

namespace onyx::game_core {
class Scene;
class GameCoreSystem;
} // namespace onyx::game_core

namespace onyx::editor {
class ICommandGraph;
}

namespace onyx::editor::scene_editor {
class ComponentsPanel : public ui::ImGuiWindow {
  public:
    static constexpr StringView WindowId = "ComponentsPanel";
    static constexpr StringView WindowCategory = "Panel";

    void SetCommandGraph( ICommandGraph& commandGraph ) { m_CommandGraph = &commandGraph; }

  private:
    void onOpen() override;
    void onClose() override;

    void onRender( ui::ImGuiSystem& imguiSystem ) override;

    void DrawSelectedEntityComponents( ecs::EntityRegistry& registry,
                                       assets::AssetId sceneId,
                                       game_core::GameCoreSystem& gameCoreSystem,
                                       const localization::LocalizationModule& localizationModule );

    void DrawCreateComponentContextMenu( ecs::EntityRegistry& registry,
                                         assets::AssetId sceneId,
                                         game_core::GameCoreSystem& gameCoreSystem,
                                         const localization::LocalizationModule& localizationModule );

    ui::TreeItem BuildComponentTree( StringView searchString,
                                     ecs::EntityRegistry& registry,
                                     assets::AssetId sceneId,
                                     game_core::GameCoreSystem& gameCoreSystem,
                                     const localization::LocalizationModule& localizationModule ) const;

  private:
    ui::TreeItem m_ContextMenuTreeRoot;
    ICommandGraph* m_CommandGraph = nullptr;

    bool m_ShowAll = false;
};
} // namespace onyx::editor::scene_editor
