#include <onyx/gamecore/systems/lightingsystem.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/entity/entitycomponentsystem.h>

#include <onyx/gamecore/components/graphics/lightcomponents.h>
#include <onyx/gamecore/components/transformcomponent.h>

#include <onyx/gamecore/scene/scene.h>
#include <onyx/graphics/graphicsapi.h>

namespace Onyx::GameCore::Lighting
{
    namespace DirectionalLights
    {
        using EntityQuery = Entity::EntityQuery<const DirectionalLightComponent, const TransformComponent>;

        void system(EntityQuery entities, Graphics::FrameContext& frameContext)
        {
            // Directional lights
            onyxU32 directionalLightIndex = 0;
            auto lightEntities = entities.GetView();
            for (Entity::EntityId lightEntity : lightEntities)
            {
                auto tuple = lightEntities.get<const DirectionalLightComponent, const TransformComponent>(lightEntity);
                auto&& [lightComponent, transformComponent] = tuple;
                Graphics::DirectionalLight& light = frameContext.Lighting.DirectionalLights[directionalLightIndex++];
                light = lightComponent.Light;
                light.Direction = transformComponent.GetRotationMatrix() * -Vector3f::Z_Unit();
            }

            frameContext.Lighting.DirectionalLightsCount = directionalLightIndex;
        }
    }

    namespace PointLights
    {
        using EntityQuery = Entity::EntityQuery<const PointLightComponent, const TransformComponent>;
        void system(EntityQuery entities, Graphics::FrameContext& frameContext)
        {
            onyxU32 pointLightIndex = 0;
            auto pointLightEntities = entities.GetView(/*entt::get<TransformComponent>*/);
            for (Entity::EntityId lightEntity : pointLightEntities)
            {
                const TransformComponent& transformComponent = pointLightEntities.get<TransformComponent>(lightEntity);
                const PointLightComponent& lightComponent = pointLightEntities.get<PointLightComponent>(lightEntity);

                Graphics::PointLight& light = frameContext.Lighting.PointLights[pointLightIndex++];
                light = lightComponent.Light;
                light.Position = transformComponent.GetTranslation();

                auto viewSpacePos = frameContext.ViewConstants.ViewMatrix * Vector4f(light.Position, 1.0f);
                auto distance = (Vector3f(viewSpacePos) - frameContext.ViewConstants.CameraPosition).Length();

                ONYX_UNUSED(viewSpacePos);
                ONYX_UNUSED(distance);
            }

            frameContext.Lighting.PointLightsCount = pointLightIndex;
        }
    }

    namespace SpotLights
    {
        using EntityQuery = Entity::EntityQuery<const SpotLightComponent, const TransformComponent>;
        void system(EntityQuery entities, Graphics::FrameContext& frameContext)
        {
            {
                onyxU32 spotLightIndex = 0;
                auto spotLightEntities = entities.GetView();
                for (Entity::EntityId lightEntity : spotLightEntities)
                {
                    const TransformComponent& transformComponent = spotLightEntities.get<TransformComponent>(lightEntity);
                    const SpotLightComponent& lightComponent = spotLightEntities.get<SpotLightComponent>(lightEntity);

                    Graphics::SpotLight& light = frameContext.Lighting.SpotLights[spotLightIndex++];
                    light = lightComponent.Light;
                    light.Position = transformComponent.GetTranslation();
                    light.Direction = transformComponent.GetRotationMatrix() * -Vector3f::Z_Unit();
                }

                frameContext.Lighting.SpotLightsCount = spotLightIndex;
            }
        }
    }

    void registerSystems(Entity::EntityComponentSystemsGraph& ecsGraph)
    {
        ecsGraph.Register(DirectionalLights::system);
        ecsGraph.Register(PointLights::system);
        ecsGraph.Register(SpotLights::system);
    }
}
