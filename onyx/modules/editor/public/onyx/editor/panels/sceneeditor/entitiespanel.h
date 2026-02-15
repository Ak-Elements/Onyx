#pragma once

#include <onyx/ui/imguiwindow.h>

#include <onyx/assets/assethandle.h>
#include <onyx/entity/entity.h>

namespace Onyx::Assets
{
    struct AssetId;

}

namespace Onyx::InputActions
{
    struct InputActionEvent;
}

namespace Onyx::GameCore
{
    class Scene;
    class GameCoreSystem;
}

namespace Onyx::Entity
{
    class EntityRegistry;
    enum class EntityId : onyxU32;
}

namespace Onyx::Editor
{
    class ICommandGraph;
}

namespace Onyx::Editor::SceneEditor
{
    class EntitiesPanel : public Ui::ImGuiWindow
    {
    public:
        static constexpr StringView WindowId = "EntitiesPanel";
        static constexpr StringView WindowCategory = "Panel";

        void SetCommandGraph(ICommandGraph& commandGraph) { m_CommandGraph = &commandGraph; }

    private:
        void OnOpen() override;
        void OnClose() override;

        void OnRender(Ui::ImGuiSystem& imguiSystem) override;

        void OnDeleteAction(const InputActions::InputActionEvent& deleteAction);

    private:
        String GetNewEntityName() const;

        void DeleteEntity(Entity::EntityId entity);
        void SetSelectedEntity(Entity::EntityId entity);

    private:
        Entity::EntityId m_SelectedEntity = Entity::EntityId::Invalid;

        ICommandGraph* m_CommandGraph = nullptr;
    };
}
