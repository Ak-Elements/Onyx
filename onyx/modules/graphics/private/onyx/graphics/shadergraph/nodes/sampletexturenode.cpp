#include <onyx/graphics/shadergraph/nodes/sampletexturenode.h>

#include <onyx/assets/assetsystem.h>

#include <onyx/nodegraph/executioncontext.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/graphics/textureasset.h>
#include <onyx/graphics/shadergraph/shadergraph.h>

namespace onyx::graphics::shader_graph_nodes
{
    SampleTextureNode::SampleTextureNode()
    {
        AddInPin<TextureInPin>();
        AddInPin<UVInPin>();

        AddOutPin<RGBOutPin>();
        AddOutPin<RGBAOutPin>();
        AddOutPin<RedOutPin>();
        AddOutPin<GreenOutPin>();
        AddOutPin<BlueOutPin>();
        AddOutPin<AlphaOutPin>();
    }

    SampleTextureNode::~SampleTextureNode() = default;

    void SampleTextureNode::OnUpdate(node_graph::ExecutionContext& context) const
    {
        const TextureInPin& inputPin = static_cast<const TextureInPin&>(*GetInputPin(0));
        if ((inputPin.IsConnected() == false) && Texture.IsValid() && Texture->IsLoaded())
        {
            ShaderGraphTextures& textures = context.Get<ShaderGraphTextures>();
            textures.AddTexture(Texture->GetTextureHandle());
        }
    }

    bool SampleTextureNode::OnSerialize(Serializer& serializer) const
    {
        if (Texture.HasAssetId())
        {
            // TODO: Add asset id serializer
            serializer.Write<"sampleTextureId">(Texture.GetId().Get());
        }
        
        return FlexiblePinsNode::OnSerialize(serializer);
    }

    bool SampleTextureNode::OnDeserialize(const Deserializer& deserializer)
    {
        onyxU64 assetId;
        if (deserializer.Read<"sampleTextureId">(assetId))
        {
            Texture.SetId(assetId);
        }

        return FlexiblePinsNode::OnDeserialize(deserializer);
    }

    void SampleTextureNode::DoGenerateShader(const node_graph::ExecutionContext& context, rhi::ShaderGenerator& generator) const
    {
        if (generator.GetStage() != rhi::ShaderStage::Fragment)
            return;

        const TextureInPin& inputPin = static_cast<const TextureInPin&>(*GetInputPin(0));

        onyxS32 textureIndex;
        //const ShaderGraphTextures& shaderGraphTextures = context.Get<ShaderGraphTextures>();
        if (inputPin.IsConnected())
        {
            textureIndex = generator.GetTextureIndex(inputPin.GetLinkedPinGlobalId());
        }
        else
        {
            // TODO: we need to get the texture from the asset here to store it similar to OnUpdate and avoid adding duplicates
            //shaderGraphTextures.GetTextureEntryIndex
            textureIndex = generator.AddTexture(Texture.GetId().Get());
        }

        if (textureIndex == INVALID_INDEX_32)
        {
            ONYX_LOG_WARNING("Missing texture for texture sample node ({:x})", GetId().Get());
            // TODO: return bool
        }

        // Texture input
        const onyxU64 texturePinGlobalId = inputPin.GetGlobalId();
        const String textureSampleVariable = format::Format("sampledTexture_{:x}", texturePinGlobalId);

        // UV Input
        const UVInPin& uvInputPin = static_cast<const UVInPin&>(*GetInputPin(1));
        const String textureCoordsString = uvInputPin.IsConnected() ? format::Format("pin_{:x}", uvInputPin.GetLinkedPinGlobalId().Get()) : rhi::ShaderGenerator::GenerateShaderValue(context.GetPinData<UVInPin>());

        // Sampling code
        String textureSampleCode = format::Format("vec4 {} = texture(BindlessTextures[nonuniformEXT(TextureIndices[{}])], {}); \n", textureSampleVariable, textureIndex, textureCoordsString);

        // Outputs
        bool isAnyOutPinConnected = false;
        Optional<const RGBOutPin*> rgbOutputPin = GetOutputPinByLocalId<RGBOutPin>();
        if (rgbOutputPin && context.IsPinConnected<RGBOutPin>())
        {
            isAnyOutPinConnected = true;
            textureSampleCode += format::Format("vec3 pin_{:x} = {}.xyz; // rgb \n", rgbOutputPin.value()->GetGlobalId().Get(), textureSampleVariable);
        }

        Optional<const RGBAOutPin*> rgbaOutputPin = GetOutputPinByLocalId<RGBAOutPin>();
        if (rgbaOutputPin && context.IsPinConnected<RGBAOutPin>())
        {
            isAnyOutPinConnected = true;
            textureSampleCode += format::Format("vec4 pin_{:x} = {}.xyzw; // rgba \n", rgbaOutputPin.value()->GetGlobalId().Get(), textureSampleVariable);
        }

        Optional<const RedOutPin*> redOutputPin = GetOutputPinByLocalId<RedOutPin>();
        if (redOutputPin && context.IsPinConnected<RedOutPin>())
        {
            isAnyOutPinConnected = true;
            textureSampleCode += format::Format("float pin_{:x} = {}.x; // red \n", redOutputPin.value()->GetGlobalId().Get(), textureSampleVariable);
        }

        Optional<const GreenOutPin*> greenOutputPin = GetOutputPinByLocalId<GreenOutPin>();
        if (greenOutputPin && context.IsPinConnected<GreenOutPin>())
        {
            isAnyOutPinConnected = true;
            textureSampleCode += format::Format("float pin_{:x} = {}.y; // green \n", greenOutputPin.value()->GetGlobalId().Get(), textureSampleVariable);
        }

        Optional<const BlueOutPin*> blueOutputPin = GetOutputPinByLocalId<BlueOutPin>();
        if (blueOutputPin && context.IsPinConnected<BlueOutPin>())
        {
            isAnyOutPinConnected = true;
            textureSampleCode += format::Format("float pin_{:x} = {}.z; // blue \n", blueOutputPin.value()->GetGlobalId().Get(), textureSampleVariable);
        }

        Optional<const AlphaOutPin*> alphaOutputPin = GetOutputPinByLocalId<AlphaOutPin>();
        if (alphaOutputPin && context.IsPinConnected<AlphaOutPin>())
        {
            isAnyOutPinConnected = true;
            textureSampleCode += format::Format("float pin_{:x} = {}.w; // alpha \n", alphaOutputPin.value()->GetGlobalId().Get(), textureSampleVariable);
        }

        if (isAnyOutPinConnected)
        {
            generator.AppendCode(textureSampleCode);
        }
    }

    void SampleTextureNode::OnChanged(assets::AssetSystem& assetSystem)
    {
        if (Texture.HasAssetId())
        {
            if (Texture.IsValid() == false)
            {
                assetSystem.GetAsset(Texture.GetId(), Texture);
            }
        }
        else
        {
            Texture.Reset();
        }
    }

#if ONYX_IS_EDITOR
    StringView SampleTextureNode::GetPinName(StringId32 pinId) const
    {
        switch (pinId)
        {
            case TextureInPin::LocalId: return "Texture";
            case UVInPin::LocalId: return "UVs";
            case RGBOutPin::LocalId: return "RGB";
            case RGBAOutPin::LocalId: return "RGBA";
            case RedOutPin::LocalId: return "R";
            case GreenOutPin::LocalId: return "G";
            case BlueOutPin::LocalId: return "B";
            case AlphaOutPin::LocalId: return "A";
        }

        ONYX_ASSERT(false, "Invalid pin id");
        return "";
    }

    node_graph::PinVisibility SampleTextureNode::DoGetPinVisibility(StringId32 localPinId) const
    {
        switch (localPinId)
        {
            case TextureInPin::LocalId: return node_graph::PinVisibility::InNode;
            default: return node_graph::PinVisibility::Default;
        }
    }
#endif
    
}
