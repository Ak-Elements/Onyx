#include <onyx/graphics/serialize/rendergraphserializer.h>

#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/ui/controls/button.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/ui/scopedcolor.h>
#include <onyx/ui/scopedid.h>

#include <imgui_stacklayout.h>

namespace onyx::graphics {
bool RenderGraphShaderNode::OnDrawInPropertyGrid( HashMap< Guid64, std::any >& constantPinData ) {
    uint32_t inputPinCount = GetInputPinCount();
    for ( uint32_t i = 0; i < inputPinCount; ++i ) {
        node_graph::PinBase* inputPin = GetInputPin( i );

        if ( inputPin->GetType() == node_graph::PinTypeId::Execute )
            continue;

        ui::ScopedImGuiId id( inputPin->GetLocalIdString() );

        if ( i >= m_InputAttachmentInfos.size() )
            m_InputAttachmentInfos.emplace_back();

        RenderGraphTextureResourceInfo& info = m_InputAttachmentInfos[ i ];

        if ( inputPin->IsConnected() ) {
            bool isAttachment = info.Type == RenderGraphResourceType::Attachment;
            if ( ui::property_grid::drawProperty( "Attachment", isAttachment ) ) {
                info.Type = isAttachment ? RenderGraphResourceType::Attachment : RenderGraphResourceType::Texture;
            }

            continue;
        }

        const Guid64 globalId = inputPin->GetGlobalId();
        if ( constantPinData.contains( globalId ) == false )
            constantPinData[ globalId ] = CreateDefaultForPin( inputPin->GetLocalId() );
        ;

        node_graph::PinTypeId typeId = inputPin->GetType();
        bool isTexture = typeId == static_cast< node_graph::PinTypeId >( TypeHash< rhi::TextureHandle >() );

        ui::property_grid::drawPropertyName( GetPinName( inputPin->GetLocalId() ) );
        ui::property_grid::drawPropertyValue( [ & ]() {
            String type;
            if ( isTexture ) {
                type = "Texture";
            } else {
                type = "Buffer";
            }

            if ( ImGui::BeginCombo( "##type", type.data() ) ) {
                if ( ImGui::Selectable( "Texture", isTexture ) ) {
                    // RemoveInputPinAt(i);
                    // AddInputPinAt<TextureHandle>(i);
                    isTexture = true;
                    inputPin = GetInputPin( i );
                }

                if ( ImGui::Selectable( "Buffer", !isTexture ) ) {
                    // RemoveInputPinAt(i);
                    // AddInputPinAt<BufferHandle>(i);
                    isTexture = false;
                    inputPin = GetInputPin( i );
                }

                ImGui::EndCombo();
            }
        } );

        // TODO: Fix in 0.5.0
        if ( isTexture ) {
            String rgba;
            Vector3f32 size;

            ui::property_grid::drawProperty( "Format", rgba );
            ui::property_grid::drawProperty( "Size", size );

        } else {
            String rgba;
            String type = "Uniform";

            ui::property_grid::drawProperty( "Format", rgba );
            ui::property_grid::drawProperty( "Type", type );
        }
    }

    uint32_t outputPinCount = GetOutputPinCount();
    for ( uint32_t i = 0; i < outputPinCount; ++i ) {
        node_graph::PinBase* outputPin = GetOutputPin( i );

        ui::ScopedImGuiId imguiScopedId( outputPin->GetLocalIdString().data() );

        const Guid64 globalId = outputPin->GetGlobalId();
        if ( constantPinData.contains( globalId ) == false )
            constantPinData[ globalId ] = CreateDefaultForPin( outputPin->GetLocalId() );
        ;

        node_graph::PinTypeId typeId = outputPin->GetType();
        bool isTexture = typeId == static_cast< node_graph::PinTypeId >( TypeHash< rhi::TextureHandle >() );

        if ( ui::property_grid::beginCollapsiblePropertyGroup( GetPinName( outputPin->GetLocalId() ) ) ) {
            if ( isTexture ) {
                if ( i >= m_OutputAttachmentInfos.size() )
                    m_OutputAttachmentInfos.emplace_back();

                RenderGraphTextureResourceInfo& info = m_OutputAttachmentInfos[ i ];

                bool isReference = info.Type == RenderGraphResourceType::Reference;
                if ( ui::property_grid::drawProperty( "Reference", isReference ) ) {
                    info.Type = isReference ? RenderGraphResourceType::Reference : RenderGraphResourceType::Attachment;
                }

                if ( isReference == false ) {
                    ui::property_grid::drawProperty( "External", info.IsExternal );

                    // if (info.IsExternal)
                    //{
                    //     // Show combobox of available external resources?
                    // }
                    // else
                    {
                        ui::property_grid::drawProperty( "Format", info.Format );

                        bool shouldClear = ( info.LoadOp == rhi::RenderPassSettings::LoadOp::Clear );
                        if ( ui::property_grid::drawProperty( "Clear", shouldClear ) ) {
                            info.LoadOp = shouldClear ? rhi::RenderPassSettings::LoadOp::Clear
                                                      : rhi::RenderPassSettings::LoadOp::DontCare;
                        }

                        if ( shouldClear ) {
                            ui::property_grid::drawColorProperty( "Clear Color", info.ClearColor );
                        }

                        bool shouldLoad = ( info.LoadOp == rhi::RenderPassSettings::LoadOp::Load );
                        if ( ui::property_grid::drawProperty( "Load", shouldLoad ) ) {
                            info.LoadOp = shouldLoad ? rhi::RenderPassSettings::LoadOp::Load
                                                     : rhi::RenderPassSettings::LoadOp::DontCare;
                        }

                        ui::property_grid::drawProperty( "Explicit Size", info.HasSize );

                        if ( info.HasSize ) {
                            ui::property_grid::drawProperty( "Size", info.Size );
                        }
                    }
                }
            } else {
                String rgba;
                String type = "Uniform";

                ui::property_grid::drawProperty( "Format", rgba );
                ui::property_grid::drawProperty( "Type", type );
            }

            ui::property_grid::endPropertyGroup();
        }
    }

    return false;
}

bool RenderGraphFixedShaderNode::OnDrawInPropertyGrid( HashMap< Guid64, std::any >& constantPinData ) {
    if ( ui::property_grid::drawAssetSelector( "Shader",
                                               m_PipelineProperties.Shader,
                                               static_cast< assets::AssetType >( rhi::Shader::TypeId.getId() ) ) ) {
        // TODO: remove shaderPath and change m_Shader to be an asset
        // shaderHandle.isLoaded?
        // adjust pins
        // register on load callback to adjust pins
        return true;
    }

    const uint8_t blendStatesCount = m_PipelineProperties.BlendStates.size();
    const uint8_t capacity = m_PipelineProperties.BlendStates.capacity();

    auto AddBlendFunctor = [ & ]() {
        ui::ScopedImGuiStyle style{
            { ImGuiStyleVar_FrameBorderSize, 0.0f },
        };
        ui::ScopedImGuiColor color{
            { ImGuiCol_Button, 0x30000000 },
        };

        const bool canAddBlendState = blendStatesCount != capacity;
        bool shouldOpen = false;
        ImGui::BeginDisabled( canAddBlendState == false );
        ImGui::Spring();
        if ( ui::Button( "+" ) ) {
            rhi::BlendState& state = m_PipelineProperties.BlendStates.emplace();
            state.IsBlendEnabled = true;
            shouldOpen = true;
        }
        ImGui::EndDisabled();
        return shouldOpen;
    };

    if ( ui::property_grid::beginCollapsiblePropertyGroup( "Blend", AddBlendFunctor ) ) {
        for ( int8_t i = 0; i < blendStatesCount; ++i ) {
            rhi::BlendState& blendState = m_PipelineProperties.BlendStates[ i ];

            if ( ui::property_grid::beginCollapsiblePropertyGroup( format::format( "{}", i ),
                                                                   ImGuiTreeNodeFlags_DefaultOpen ) ) {
                ui::property_grid::drawProperty( "Enabled", blendState.IsBlendEnabled );
                ui::property_grid::drawProperty( "Color Mask", blendState.ColorWriteMask );

                if ( ui::property_grid::beginCollapsiblePropertyGroup( "Color" ) ) {
                    ui::property_grid::drawProperty( "Source", blendState.SourceColor );
                    ui::property_grid::drawProperty( "Destination", blendState.DestinationColor );
                    ui::property_grid::drawProperty( "Operation", blendState.ColorOperation );

                    ui::property_grid::endPropertyGroup();
                }

                if ( ui::property_grid::beginCollapsiblePropertyGroup( "Alpha" ) ) {
                    ui::property_grid::drawProperty( "Source", blendState.SourceAlpha );
                    ui::property_grid::drawProperty( "Destination", blendState.DestinationAlpha );
                    ui::property_grid::drawProperty( "Operation", blendState.AlphaOperation );

                    ui::property_grid::endPropertyGroup();
                }

                ui::property_grid::endPropertyGroup();
            }
        }

        ui::property_grid::endPropertyGroup();
    }

    if ( ui::property_grid::beginCollapsiblePropertyGroup( "Rasterization" ) ) {
        ui::property_grid::drawProperty( "Front facing", m_PipelineProperties.Rasterization.IsFrontFacing );
        ui::property_grid::drawProperty( "Fill Mode", m_PipelineProperties.Rasterization.FillMode );
        ui::property_grid::drawProperty( "Cull Mode", m_PipelineProperties.Rasterization.CullMode );

        ui::property_grid::endPropertyGroup();
    }

    if ( ui::property_grid::beginCollapsiblePropertyGroup( "Depth Stencil" ) ) {
        bool isDepthEnabled = m_PipelineProperties.DepthStencil.IsDepthEnabled;
        ui::property_grid::drawProperty( "Enabled", isDepthEnabled );
        m_PipelineProperties.DepthStencil.IsDepthEnabled = isDepthEnabled;

        bool isDepthWriteEnabled = m_PipelineProperties.DepthStencil.IsDepthWriteEnabled;
        ui::property_grid::drawProperty( "Write", isDepthWriteEnabled );
        m_PipelineProperties.DepthStencil.IsDepthWriteEnabled = isDepthWriteEnabled;

        bool isStencilEnabled = m_PipelineProperties.DepthStencil.IsStencilEnabled;
        ui::property_grid::drawProperty( "Stencil", isStencilEnabled );
        m_PipelineProperties.DepthStencil.IsStencilEnabled = isStencilEnabled;

        ui::property_grid::drawProperty( "Compare", m_PipelineProperties.DepthStencil.Compare );

        // if (ui::property_grid::BeginCollapsiblePropertyGroup("Front Operation"))
        //{
        //     //m_PipelineProperties.DepthStencil.Front
        // }

        ui::property_grid::endPropertyGroup();
    }

    return RenderGraphShaderNode::OnDrawInPropertyGrid( constantPinData );
}
} // namespace onyx::graphics
