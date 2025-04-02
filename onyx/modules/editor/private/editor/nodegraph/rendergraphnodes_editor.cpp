#include <onyx/graphics/serialize/rendergraphserializer.h>

#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/nodefactory.h>
#include <onyx/ui/imguistyle.h>
#include <onyx/ui/propertygrid.h>
#include <imgui_stacklayout.h>

namespace Onyx::Graphics
{
    bool RenderGraphShaderNode::OnDrawInPropertyGrid(HashMap<onyxU64, std::any>& constantPinData)
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
                if (Ui::PropertyGrid::DrawBoolProperty("Attachment", isAttachment))
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

            if (isTexture)
            {
                String rgba;
                Vector3f size;

                Ui::PropertyGrid::DrawStringProperty("Format", rgba);
                Ui::PropertyGrid::DrawVector3Property("Size", size);



            }
            else
            {
                String rgba;
                String type = "Uniform";

                Ui::PropertyGrid::DrawStringProperty("Format", rgba);
                Ui::PropertyGrid::DrawStringProperty("Type", type);
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
                    Vector3f size;

                    if (i >= m_OutputAttachmentInfos.size())
                        m_OutputAttachmentInfos.emplace_back();

                    RenderGraphTextureResourceInfo& info = m_OutputAttachmentInfos[i];

                    bool isReference = info.Type == RenderGraphResourceType::Reference;
                    if (Ui::PropertyGrid::DrawBoolProperty("Reference", isReference))
                    {
                        info.Type = isReference ? RenderGraphResourceType::Reference : RenderGraphResourceType::Attachment;
                    }

                    if (isReference == false)
                    {
                        Ui::PropertyGrid::DrawBoolProperty("External", info.IsExternal);

                        //if (info.IsExternal)
                        //{
                        //    // Show combobox of available external resources?
                        //}
                        //else
                        {
                            Ui::PropertyGrid::DrawEnumProperty("Format", info.Format);

                            bool shouldClear = (info.LoadOp == RenderPassSettings::LoadOp::Clear);
                            if (Ui::PropertyGrid::DrawBoolProperty("Clear", shouldClear))
                            {
                                info.LoadOp = shouldClear ? RenderPassSettings::LoadOp::Clear : RenderPassSettings::LoadOp::DontCare;
                            }

                            if (shouldClear)
                            {
                                Ui::PropertyGrid::DrawColorProperty("Clear Color", info.ClearColor);
                            }

                            bool shouldLoad = (info.LoadOp == RenderPassSettings::LoadOp::Load);
                            if (Ui::PropertyGrid::DrawBoolProperty("Load", shouldLoad))
                            {
                                info.LoadOp = shouldLoad ? RenderPassSettings::LoadOp::Load : RenderPassSettings::LoadOp::DontCare;
                            }

                            Ui::PropertyGrid::DrawBoolProperty("Explicit Size", info.HasSize);

                            if (info.HasSize)
                            {
                                Ui::PropertyGrid::DrawVector3Property("Size", info.Size);
                            }
                        }
                    }
                }
                else
                {
                    String rgba;
                    String type = "Uniform";

                    Ui::PropertyGrid::DrawStringProperty("Format", rgba);
                    Ui::PropertyGrid::DrawStringProperty("Type", type);
                }

                Ui::PropertyGrid::EndPropertyGroup();
            }
        }

        return false;
    }

    bool RenderGraphFixedShaderNode::OnDrawInPropertyGrid(HashMap<onyxU64, std::any>& constantPinData)
    {
        if (Ui::PropertyGrid::DrawStringProperty("Shader", m_ShaderPath))
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
                if (ImGui::Button("+"))
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
                    Ui::PropertyGrid::DrawBoolProperty("Enabled", blendState.IsBlendEnabled);
                    Ui::PropertyGrid::DrawEnumProperty("Color Mask", blendState.ColorWriteMask);

                    if (Ui::PropertyGrid::BeginCollapsiblePropertyGroup("Color"))
                    {
                        Ui::PropertyGrid::DrawEnumProperty("Source", blendState.SourceColor);
                        Ui::PropertyGrid::DrawEnumProperty("Destination", blendState.DestinationColor);
                        Ui::PropertyGrid::DrawEnumProperty("Operation", blendState.ColorOperation);

                        Ui::PropertyGrid::EndPropertyGroup();
                    }

                    if (Ui::PropertyGrid::BeginCollapsiblePropertyGroup("Alpha"))
                    {
                        Ui::PropertyGrid::DrawEnumProperty("Source", blendState.SourceAlpha);
                        Ui::PropertyGrid::DrawEnumProperty("Destination", blendState.DestinationAlpha);
                        Ui::PropertyGrid::DrawEnumProperty("Operation", blendState.AlphaOperation);

                        Ui::PropertyGrid::EndPropertyGroup();
                    }

                    Ui::PropertyGrid::EndPropertyGroup();
                }

            }

            Ui::PropertyGrid::EndPropertyGroup();
        }

        if (Ui::PropertyGrid::BeginCollapsiblePropertyGroup("Rasterization"))
        {
            Ui::PropertyGrid::DrawBoolProperty("Front facing", m_PipelineProperties.Rasterization.IsFrontFacing);
            Ui::PropertyGrid::DrawEnumProperty("Fill Mode", m_PipelineProperties.Rasterization.FillMode);
            Ui::PropertyGrid::DrawEnumProperty("Cull Mode", m_PipelineProperties.Rasterization.CullMode);

            Ui::PropertyGrid::EndPropertyGroup();
        }

        if (Ui::PropertyGrid::BeginCollapsiblePropertyGroup("Depth Stencil"))
        {
            bool isDepthEnabled = m_PipelineProperties.DepthStencil.IsDepthEnabled;
            Ui::PropertyGrid::DrawBoolProperty("Enabled", isDepthEnabled);
            m_PipelineProperties.DepthStencil.IsDepthEnabled = isDepthEnabled;

            bool isDepthWriteEnabled = m_PipelineProperties.DepthStencil.IsDepthWriteEnabled;
            Ui::PropertyGrid::DrawBoolProperty("Write", isDepthWriteEnabled);
            m_PipelineProperties.DepthStencil.IsDepthWriteEnabled = isDepthWriteEnabled;

            bool isStencilEnabled = m_PipelineProperties.DepthStencil.IsStencilEnabled;
            Ui::PropertyGrid::DrawBoolProperty("Stencil", isStencilEnabled);
            m_PipelineProperties.DepthStencil.IsStencilEnabled = isStencilEnabled;

            Ui::PropertyGrid::DrawEnumProperty("Compare", m_PipelineProperties.DepthStencil.Compare);

            //if (Ui::PropertyGrid::BeginCollapsiblePropertyGroup("Front Operation"))
            //{
            //    //m_PipelineProperties.DepthStencil.Front
            //}

            Ui::PropertyGrid::EndPropertyGroup();
        }

        return RenderGraphShaderNode::OnDrawInPropertyGrid(constantPinData);
    }
}
