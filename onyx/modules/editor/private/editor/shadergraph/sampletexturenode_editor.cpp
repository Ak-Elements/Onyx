#include <onyx/graphics/shadergraph/nodes/sampletexturenode.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/graphics/textureasset.h>

namespace Onyx::Graphics
{
    bool SampleTextureNode::OnDrawInPropertyGrid(HashMap<onyxU64, std::any>& constantPinData)
    {
        bool isModified = false;
        bool isTexturePinConnected = GetInputPin(0)->IsConnected();

        ImGui::BeginDisabled(isTexturePinConnected);
        {
            Ui::ScopedImGuiId imguiId("SampleTexture");
            if (Ui::PropertyGrid::DrawAssetSelector("Texture", TextureId, static_cast<Assets::AssetType>(TypeHash<TextureAsset>())))
            {
                isModified = true;
            }
        }
        ImGui::EndDisabled();

        return isModified || Super::OnDrawInPropertyGrid(constantPinData);
    }

    void SampleTextureNode::OnUIDrawNode()
    {
        FlexiblePinsNode<ShaderGraphNode>::OnUIDrawNode();

        ImGui::BeginVertical("TexturePreview", ImVec2(0, 0), 0.0f);
        if (Texture.IsValid() && Texture->IsLoaded())
        {
            ImGui::Image(Texture->GetTextureHandle().Texture->GetIndex(), ImVec2(96, 96));
        }
        ImGui::EndVertical();
    }
}   
