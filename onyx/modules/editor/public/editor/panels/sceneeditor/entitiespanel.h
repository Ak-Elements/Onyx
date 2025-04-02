#pragma once

#include <onyx/entity/entityregistry.h>

namespace Onyx::GameCore
{
    class Scene;
}

namespace Onyx::Entity
{
    enum class EntityId : onyxU32;
}

namespace Onyx::Editor::SceneEditor
{
    class SceneEditorWindow;

    class EntitiesPanel
    {
    public:
        void Render(GameCore::Scene& scene);

    private:
        String GetNewEntityName() const;

        void DeleteEntity(Entity::EntityId entity);
        void SetSelectedEntity(Entity::EntityId entity);
    private:
        Entity::EntityId selectedEntity;
    };
}
