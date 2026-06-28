#include <onyx/gamecore/systems/lightingsystem.h>

#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/components/graphics/directionallightcomponent.gen.h>
#include <onyx/gamecore/components/graphics/pointlightcomponent.gen.h>
#include <onyx/gamecore/components/graphics/spotlightcomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/rhi/framecontext.h>

#include <onyx/gamecore/scene/scene.h>

namespace onyx::game_core::lighting {
namespace directional_lights {
using LightAccess = ecs::Access::Read< DirectionalLightComponent, TransformComponent >;
using LightsQuery = LightAccess::AsQuery;

void system( LightsQuery lightEntities, rhi::FrameContext& frameContext ) {
    // Directional lights
    uint32_t directionalLightIndex = 0;
    for( auto lightEntity : lightEntities ) {
        auto&& [ lightComponent, transformComponent ] = LightAccess::AsEntity( lightEntities, lightEntity );

        rhi::DirectionalLight& light = frameContext.Lighting.DirectionalLights[ directionalLightIndex++ ];
        light.Color = lightComponent.Color;
        light.Intensity = lightComponent.Intensity;
        light.ShadowAmount = lightComponent.ShadowAmount;
        light.IsShadowCasting = lightComponent.IsShadowCasting;
        light.Direction = transformComponent.Rotation.toMatrix3() * -Vector3f32::zUnit();
    }

    frameContext.Lighting.DirectionalLightsCount = directionalLightIndex;
}
} // namespace directional_lights

namespace point_lights {
using LightAccess = ecs::Access::Read< PointLightComponent, TransformComponent >;
using LightsQuery = LightAccess::AsQuery;

void system( LightsQuery lightEntities, rhi::FrameContext& frameContext ) {
    uint32_t pointLightIndex = 0;
    for( ecs::EntityId lightEntity : lightEntities ) {
        auto&& [ lightComponent, transformComponent ] = LightAccess::AsEntity( lightEntities, lightEntity );

        rhi::PointLight& light = frameContext.Lighting.PointLights[ pointLightIndex++ ];
        light.Position = transformComponent.Translation;
        light.Color = lightComponent.Color;
        light.Intensity = lightComponent.Intensity;
        light.Radius = lightComponent.Radius;
        light.IsEnabled = lightComponent.IsEnabled;
        light.IsShadowCasting = lightComponent.IsShadowCasting;

        auto viewSpacePos = frameContext.ViewConstants.ViewMatrix * Vector4f32( light.Position, 1.0f );
        std::ignore /*auto distance*/ = ( Vector3f32( viewSpacePos ) - frameContext.ViewConstants.CameraPosition )
                                            .length();
    }

    frameContext.Lighting.PointLightsCount = pointLightIndex;
}
} // namespace point_lights

namespace spot_lights {
using LightAccess = ecs::Access::Read< SpotLightComponent, TransformComponent >;
using LightsQuery = LightAccess::AsQuery;

void system( LightsQuery lightEntities, rhi::FrameContext& frameContext ) {
    uint32_t spotLightIndex = 0;
    for( ecs::EntityId lightEntity : lightEntities ) {
        auto&& [ lightComponent, transformComponent ] = LightAccess::AsEntity( lightEntities, lightEntity );

        rhi::SpotLight& light = frameContext.Lighting.SpotLights[ spotLightIndex++ ];
        light.Position = transformComponent.Translation;
        light.Direction = transformComponent.Rotation.toMatrix3() * -Vector3f32::zUnit();
        light.Color = lightComponent.Color;
        light.Intensity = lightComponent.Intensity;
        light.Falloff = lightComponent.Falloff;
        light.Angle = lightComponent.Angle;
        light.AngleAttenuation = lightComponent.AngleAttenuation;
        light.Range = lightComponent.Range;
        light.IsShadowCasting = lightComponent.IsShadowCasting;

        frameContext.Lighting.SpotLightsCount = spotLightIndex;
    }
}
} // namespace spot_lights

void registerSystems( ecs::EcsBuilder& ecsBuilder ) {
    ecsBuilder.registerSystem( directional_lights::system );
    ecsBuilder.registerSystem( point_lights::system );
    ecsBuilder.registerSystem( spot_lights::system );
}
} // namespace onyx::game_core::lighting
