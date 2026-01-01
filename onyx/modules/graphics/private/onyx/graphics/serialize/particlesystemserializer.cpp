#include <onyx/graphics/serialize/particlesystemserializer.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/graphicssystem.h>
#include <onyx/graphics/particles/particlesystem.h>
#include <onyx/graphics/serialize/shadergraphserializer.h>
#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>

namespace Onyx::Graphics
{
    bool ParticleSystemSerializer::Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& /*meta*/, Serializer& serializer, IEngine& engine) const
    {
        ONYX_UNUSED(engine);
        //Assets::AssetSystem& assetSystem = engine.GetSystem<Assets::AssetSystem>();
        const ParticleSystem& particleSystem = asset.As<ParticleSystem>();

        //bool queuedSave = assetSystem.SaveAsset(particleSystem.m_EmitShader);
        //queuedSave |= assetSystem.SaveAsset(particleSystem.m_UpdateShader);

        serializer.Write<"emitShader">(particleSystem.m_EmitShader->GetId());
        serializer.Write<"updateShader">(particleSystem.m_UpdateShader->GetId());

        return true;
    }

    bool ParticleSystemSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& /*meta*/, const Deserializer& deserializer, IEngine& engine) const
    {
        Assets::AssetSystem& assetSystem = engine.GetSystem<Assets::AssetSystem>();

        ParticleSystem& particleSystem = asset.As<ParticleSystem>();

        Assets::AssetId emitShaderAssetId;
        deserializer.Read<"emitShader">(emitShaderAssetId);
        Assets::AssetId updateShaderAssetId;
        deserializer.Read<"updateShader">(updateShaderAssetId);

        assetSystem.GetAsset(emitShaderAssetId, particleSystem.m_EmitShader);
        assetSystem.GetAsset(updateShaderAssetId, particleSystem.m_UpdateShader);

        return true;
    }

}
