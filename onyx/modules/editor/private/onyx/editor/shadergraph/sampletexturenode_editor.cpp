#include <onyx/graphics/shadergraph/nodes/sampletexturenode.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/graphics/textureasset.h>
#include <onyx/ui/scopedid.h>

#include <imgui_internal.h>
#include <imgui_node_editor.h>

namespace onyx::graphics::shader_graph_nodes
{
   //bool SampleTextureNode::OnDrawInPropertyGrid(HashMap<Guid64, std::any>& constantPinData)
   //{
   //    bool isModified = false;
   //    bool isTexturePinConnected = GetInputPin(0)->IsConnected();

   //    ImGui::BeginDisabled(isTexturePinConnected);
   //    {
   //        ui::ScopedImGuiId imguiId("SampleTexture");
   //        if (ui::property_grid::DrawAssetSelector("Texture", TextureId, static_cast<assets::AssetType>(TypeHash<TextureAsset>())))
   //        {
   //            isModified = true;
   //        }
   //    }
   //    ImGui::EndDisabled();

   //    return isModified || Super::OnDrawInPropertyGrid(constantPinData);
   //}

   //void SampleTextureNode::OnUIDrawNode()
   //{
   //    FlexiblePinsNode<ShaderGraphNode>::OnUIDrawNode();

   //    ImGui::BeginVertical("TexturePreview", ImVec2(0, 0), 0.0f);
   //    if (Texture.IsValid() && Texture->IsLoaded())
   //    {
   //        ImGui::Image(Texture->GetTextureHandle().Texture->GetIndex(), ImVec2(96, 96));
   //    }
   //    ImGui::EndVertical();
   //}

}