#pragma once

#include <onyx/entity/entityregistry.h>
#include <onyx/localization/localizedstring.h>

namespace Onyx::Localization
{
    class LocalizationModule;
}

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
        EntitiesPanel(Localization::LocalizationModule& localizationModule);

        void Render(GameCore::Scene& scene);

    private:
        String GetNewEntityName() const;

        void DeleteEntity(GameCore::Scene& scene, Entity::EntityId entity);
        void SetSelectedEntity(GameCore::Scene& scene, Entity::EntityId entity);
    private:
        Localization::LocalizedString m_NameColumnLabel;
        Localization::LocalizedString m_VisibilityColumnLabel;
        Localization::LocalizedString m_DuplicateEntityLabel;
        Localization::LocalizedString m_CreateEntityLabel;
        Localization::LocalizedString m_DeleteEntityLabel;

        Entity::EntityId selectedEntity;
    };
}
