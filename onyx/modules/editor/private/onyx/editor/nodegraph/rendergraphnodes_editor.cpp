#include <onyx/graphics/serialize/rendergraphserializer.h>

#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/ui/scopedcolor.h>
#include <onyx/ui/scopedid.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/ui/controls/button.h>

#include <imgui_stacklayout.h>

namespace Onyx::Graphics
{
    bool RenderGraphShaderNode::OnDrawInPropertyGrid(HashMap<Guid64, std::any>& constantPinData)
    {
        onyxU32 inputPinCount = GetInputPinCount();
        for (onyxU32 i = 0; i < inputPinCount; ++i)
        {
            NodeGraph::PinBase* inputPin = GetInputPin(i);

            if (inputPin->GetType() == NodeGraph::PinTypeId::Execute)
                continue;

            Ui::ScopedImGuiId id(inputPin->GetLocalIdString());

            if (i >= m_InputAttachmentInfos.size())
                m_InputAttachmentInfos.emplace_back();

            RenderGraphTextureResourceInfo& info = m_InputAttachmentInfos[i];

            if (inputPin->IsConnected())
            {
                bool isAttachment = info.Type == RenderGraphResourceType::Attachment;
                if (Ui::PropertyGrid::DrawProperty("Attachment", isAttachment))
                {
                    info.Type = isAttachment ? RenderGraphResourceType::Attachment : RenderGraphResourceType::Texture;
                }

                continue;
            }

            const Guid64 globalId = inputPin->GetGlobalId();
            if (constantPinData.contains(globalId) == false)
                constantPinData[globalId] = CreateDefaultForPin(inputPin->GetLocalId());;

            NodeGraph::PinTypeId typeId = inputPin->GetType();
            bool isTexture = typeId == static_cast<NodeGraph::PinTypeId>(TypeHash<TextureHandle>());

            Ui::PropertyGrid::DrawPropertyName(GetPinName(inputPin->GetLocalId()));
            Ui::PropertyGrid::DrawPropertyValue([&]()
                {
                    String type;
                    if (isTexture)
                    {
                        type = "Texture";
                    }
                    else
                    {
                        type = "Buffer";
                    }

                    if (ImGui::BeginCombo("##type", type.data()))
                    {
                        if (ImGui::Selectable("Texture", isTexture))
                        {
                            //RemoveInputPinAt(i);
                            //AddInputPinAt<TextureHandle>(i);
                            isTexture = true;
                            inputPin = GetInputPin(i);
                        }

                        if (ImGui::Selectable("Buffer", !isTexture))
                        {
                            //RemoveInputPinAt(i);
                            //AddInputPinAt<BufferHandle>(i);
                            isTexture = false;
                            inputPin = GetInputPin(i);
                        }

                        ImGui::EndCombo();
                    }
                });

            // TODO: Fix in 0.5.0
            if (isTexture)
            {
                String rgba;
                Vector3f32 size;

                Ui::PropertyGrid::DrawProperty("Format", rgba);
                Ui::PropertyGrid::DrawProperty("Size", size);



            }
            else
            {
                String rgba;
                String type = "Uniform";

                Ui::PropertyGrid::DrawProperty("Format", rgba);
                Ui::PropertyGrid::DrawProperty("Type", type);
            }
        }

        onyxU32 outputPinCount = GetOutputPinCount();
        for (onyxU32 i = 0; i < outputPinCount; ++i)
        {
            NodeGraph::PinBase* outputPin = GetOutputPin(i);

            Ui::ScopedImGuiId imguiScopedId(outputPin->GetLocalIdString().data());

            const Guid64 globalId = outputPin->GetGlobalId();
            if (constantPinData.contains(globalId) == false)
                constantPinData[globalId] = CreateDefaultForPin(outputPin->GetLocalId());;

            NodeGraph::PinTypeId typeId = outputPin->GetType();
            bool isTexture = typeId == static_cast<NodeGraph::PinTypeId>(TypeHash<TextureHandle>());

            if (Ui::PropertyGrid::BeginCollapsiblePropertyGroup(GetPinName(outputPin->GetLocalId())))
            {
                if (isTexture)
                {
                    if (i >= m_OutputAttachmentInfos.size())
                        m_OutputAttachmentInfos.emplace_back();

                    RenderGraphTextureResourceInfo& info = m_OutputAttachmentInfos[i];

                    bool isReference = info.Type == RenderGraphResourceType::Reference;
                    if (Ui::PropertyGrid::DrawProperty("Reference", isReference))
                    {
                        info.Type = isReference ? RenderGraphResourceType::Reference : RenderGraphResourceType::Attachment;
                    }

                    if (isReference == false)
                    {
                        Ui::PropertyGrid::DrawProperty("External", info.IsExternal);

                        //if (info.IsExternal)
                        //{
                        //    // Show combobox of available external resources?
                        //}
                        //else
                        {
                            Ui::PropertyGrid::DrawProperty("Format", info.Format);

                            bool shouldClear = (info.LoadOp == RenderPassSettings::LoadOp::Clear);
                            if (Ui::PropertyGrid::DrawProperty("Clear", shouldClear))
                            {
                                info.LoadOp = shouldClear ? RenderPassSettings::LoadOp::Clear : RenderPassSettings::LoadOp::DontCare;
                            }

                            if (shouldClear)
                            {
                                Ui::PropertyGrid::DrawColorProperty("Clear Color", info.ClearColor);
                            }

                            bool shouldLoad = (info.LoadOp == RenderPassSettings::LoadOp::Load);
                            if (Ui::PropertyGrid::DrawProperty("Load", shouldLoad))
                            {
                                info.LoadOp = shouldLoad ? RenderPassSettings::LoadOp::Load : RenderPassSettings::LoadOp::DontCare;
                            }

                            Ui::PropertyGrid::DrawProperty("Explicit Size", info.HasSize);

                            if (info.HasSize)
                            {
                                Ui::PropertyGrid::DrawProperty("Size", info.Size);
                            }
                        }
                    }
                }
                else
                {
                    String rgba;
                    String type = "Uniform";

                    Ui::PropertyGrid::DrawProperty("Format", rgba);
                    Ui::PropertyGrid::DrawProperty("Type", type);
                }

                Ui::PropertyGrid::EndPropertyGroup();
            }
        }

        return false;
    }

    bool RenderGraphFixedShaderNode::OnDrawInPropertyGrid(HashMap<Guid64, std::any>& constantPinData)
    {
        if (Ui::PropertyGrid::DrawAssetSelector("Shader", m_PipelineProperties.Shader, static_cast<Assets::AssetType>(Shader::TypeId.GetId())))
        {
            // TODO: remove shaderPath and change m_Shader to be an asset
            // shaderHandle.isLoaded?
            // adjust pins
            // register on load callback to adjust pins
            return true;
        }

        const onyxU8 blendStatesCount = m_PipelineProperties.BlendStates.size();
        const onyxU8 capacity = m_PipelineProperties.BlendStates.capacity();

        auto AddBlendFunctor = [&]()
            {
                Ui::ScopedImGuiStyle style
                {
                     { ImGuiStyleVar_FrameBorderSize, 0.0f },
                };
                Ui::ScopedImGuiColor color
                {
                    { ImGuiCol_Button, 0x30000000 },
                };

                const bool canAddBlendState = blendStatesCount != capacity;
                bool shouldOpen = false;
                ImGui::BeginDisabled(canAddBlendState == false);
                ImGui::Spring();
                if (Ui::Button("+"))
                {
                    BlendState& state = m_PipelineProperties.BlendStates.Emplace();
                    state.IsBlendEnabled = true;
                    shouldOpen = true;
                }
                ImGui::EndDisabled();
                return shouldOpen;
            };

        if (Ui::PropertyGrid::BeginCollapsiblePropertyGroup("Blend", AddBlendFunctor))
        {
            for (onyxS8 i = 0; i < blendStatesCount; ++i)
            {
                BlendState& blendState = m_PipelineProperties.BlendStates[i];

                if (Ui::PropertyGrid::BeginCollapsiblePropertyGroup(Format::Format("{}", i), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    Ui::PropertyGrid::DrawProperty("Enabled", blendState.IsBlendEnabled);
                    Ui::PropertyGrid::DrawProperty("Color Mask", blendState.ColorWriteMask);

                    if (Ui::PropertyGrid::BeginCollapsiblePropertyGroup("Color"))
                    {
                        Ui::PropertyGrid::DrawProperty("Source", blendState.SourceColor);
                        Ui::PropertyGrid::DrawProperty("Destination", blendState.DestinationColor);
                        Ui::PropertyGrid::DrawProperty("Operation", blendState.ColorOperation);

                        Ui::PropertyGrid::EndPropertyGroup();
                    }

                    if (Ui::PropertyGrid::BeginCollapsiblePropertyGroup("Alpha"))
                    {
                        Ui::PropertyGrid::DrawProperty("Source", blendState.SourceAlpha);
                        Ui::PropertyGrid::DrawProperty("Destination", blendState.DestinationAlpha);
                        Ui::PropertyGrid::DrawProperty("Operation", blendState.AlphaOperation);

                        Ui::PropertyGrid::EndPropertyGroup();
                    }

                    Ui::PropertyGrid::EndPropertyGroup();
                }

            }

            Ui::PropertyGrid::EndPropertyGroup();
        }

        if (Ui::PropertyGrid::BeginCollapsiblePropertyGroup("Rasterization"))
        {
            Ui::PropertyGrid::DrawProperty("Front facing", m_PipelineProperties.Rasterization.IsFrontFacing);
            Ui::PropertyGrid::DrawProperty("Fill Mode", m_PipelineProperties.Rasterization.FillMode);
            Ui::PropertyGrid::DrawProperty("Cull Mode", m_PipelineProperties.Rasterization.CullMode);

            Ui::PropertyGrid::EndPropertyGroup();
        }

        if (Ui::PropertyGrid::BeginCollapsiblePropertyGroup("Depth Stencil"))
        {
            bool isDepthEnabled = m_PipelineProperties.DepthStencil.IsDepthEnabled;
            Ui::PropertyGrid::DrawProperty("Enabled", isDepthEnabled);
            m_PipelineProperties.DepthStencil.IsDepthEnabled = isDepthEnabled;

            bool isDepthWriteEnabled = m_PipelineProperties.DepthStencil.IsDepthWriteEnabled;
            Ui::PropertyGrid::DrawProperty("Write", isDepthWriteEnabled);
            m_PipelineProperties.DepthStencil.IsDepthWriteEnabled = isDepthWriteEnabled;

            bool isStencilEnabled = m_PipelineProperties.DepthStencil.IsStencilEnabled;
            Ui::PropertyGrid::DrawProperty("Stencil", isStencilEnabled);
            m_PipelineProperties.DepthStencil.IsStencilEnabled = isStencilEnabled;

            Ui::PropertyGrid::DrawProperty("Compare", m_PipelineProperties.DepthStencil.Compare);

            //if (Ui::PropertyGrid::BeginCollapsiblePropertyGroup("Front Operation"))
            //{
            //    //m_PipelineProperties.DepthStencil.Front
            //}

            Ui::PropertyGrid::EndPropertyGroup();
        }

        return RenderGraphShaderNode::OnDrawInPropertyGrid(constantPinData);
    }
}
