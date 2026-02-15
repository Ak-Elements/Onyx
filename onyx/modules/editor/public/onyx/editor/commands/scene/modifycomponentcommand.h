#pragma once

#include <onyx/editor/commands/scene/scenecommand.h>
#include <onyx/entity/entity.h>

namespace Onyx
{
    namespace Assets
    {
        struct AssetId;
    }

    namespace GameCore
    {
        class GameCoreSystem;
    }
}

namespace Onyx::Editor
{
    struct ModifyComponentCommand : public SceneCommand
    {
        ModifyComponentCommand(Entity::EntityId entity, StringId32 componentTypeId, DynamicArray<onyxU32>&& componentData, Assets::AssetId sceneId, GameCore::GameCoreSystem& gameCoreSystem);

        void Execute() override;

    private:
        DynamicArray<onyxU32> m_ComponentData;
        
        Entity::EntityId m_EntityId; 
        StringId32 m_ComponentTypeId;
    };
}