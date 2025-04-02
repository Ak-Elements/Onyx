#include <editor/rendertasks/guitask.h>

#include <onyx/graphics/bufferproperties.h>
#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/textureproperties.h>
#include <onyx/graphics/texturestorageproperties.h>
#include <onyx/graphics/shader/shadereffect.h>
#include <onyx/graphics/rendergraph/rendergraph.h>

#include <imgui.h>
#include <onyx/profiler/profiler.h>

namespace Onyx
{
    void UITask::Init(Graphics::GraphicsApi& api, Graphics::ShaderEffectHandle& /*shaderEffect*/)
    {
        ImGuiIO& io = ImGui::GetIO();

        //// Create font texture
        unsigned char* fontData;
        int texWidth, texHeight;

        io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
        onyxU32 uploadSize = texWidth * texHeight * 4 * sizeof(char);

        //// Create target image for copy
        Graphics::TextureStorageProperties storageProps;
        storageProps.m_Format = Graphics::TextureFormat::RGBA_UNORM8;
        storageProps.m_Type = Graphics::TextureType::Texture2D;
        storageProps.m_Size = { texWidth, texHeight, 1 };
        storageProps.m_MaxMipLevel = 1;
        storageProps.m_ArraySize = 0;
        storageProps.m_MSAAProperties = { 1, 1 }; // samples /quality 
        storageProps.m_CpuAccess = Graphics::CPUAccess::None;
        storageProps.m_GpuAccess = Graphics::GPUAccess::Read;
        storageProps.m_IsTexture = true;
        storageProps.m_DebugName = "ImGui Font Texture Storage";

        Graphics::TextureProperties textureProps;
        textureProps.m_Format = Graphics::TextureFormat::RGBA_UNORM8;
        textureProps.m_AllowCubeMapLoads = false;
        textureProps.m_DebugName = "ImGui Font Texture";

        Span<onyxU8> fontTexData { fontData, uploadSize };

        api.CreateTexture(m_FontImage, storageProps, textureProps, fontTexData);
        io.Fonts->TexID = m_FontImage.Texture->GetIndex();

        Graphics::BufferProperties vertexBufferProps;
        vertexBufferProps.m_Size = 400000 * sizeof(ImDrawVert);
        vertexBufferProps.m_BindFlags = static_cast<onyxU8>(Graphics::BufferType::Vertex);
        vertexBufferProps.m_CpuAccess = Graphics::CPUAccess::Write;
        vertexBufferProps.m_DebugName = "ImGui Vertices";

        Graphics::BufferProperties indexBufferProps;
        indexBufferProps.m_Size = 200000 * sizeof(ImDrawIdx);
        indexBufferProps.m_BindFlags = static_cast<onyxU8>(Graphics::BufferType::Index);
        indexBufferProps.m_CpuAccess = Graphics::CPUAccess::Write;
        indexBufferProps.m_DebugName = "ImGui Indices";

        for (onyxU8 i = 0; i < Graphics::MAX_FRAMES_IN_FLIGHT; ++i)
        {
            Graphics::BufferHandle& vertexBuffer = m_VertexBuffers[i];
            api.CreateBuffer(vertexBuffer, vertexBufferProps);
            m_VertexCounts.Add(400000);

            Graphics::BufferHandle& indexBuffer = m_IndexBuffers[i];
            api.CreateBuffer(indexBuffer, indexBufferProps);
            m_IndexCounts.Add(200000);
        }
    }

    void UITask::Shutdown(Graphics::GraphicsApi& /*api*/)
    {
        m_VertexBuffers.Clear();
        m_IndexBuffers.Clear();
    }

    void UITask::BeginFrame(const Graphics::RenderGraphContext& context)
    {
        ONYX_UNUSED(context);
    }

    void UITask::PreRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer)
    {
        ONYX_UNUSED(commandBuffer);

        ImGui::Render();

        ImDrawData* imDrawData = ImGui::GetDrawData();

        if ((!imDrawData) || (imDrawData->CmdListsCount == 0))
        {
            return;
        }

        onyxS32 fb_width = numeric_cast<onyxS32>(imDrawData->DisplaySize.x * imDrawData->FramebufferScale.x);
        onyxS32 fb_height = numeric_cast<onyxS32>(imDrawData->DisplaySize.y * imDrawData->FramebufferScale.y);
        if (fb_width <= 0 || fb_height <= 0)
            return;

        // Note: Alignment is done inside buffer creation
        onyxU32 vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
        onyxU32 indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

        // Update buffers only if vertex or index count has been changed compared to current buffer size
        if ((vertexBufferSize == 0) || (indexBufferSize == 0))
        {
            return /*false*/;
        }

        // Vertex buffer
        const onyxU8 frameIndex = context.FrameContext.FrameIndex;
        Graphics::BufferHandle& vertexBuffer = m_VertexBuffers[frameIndex];
        if ((vertexBuffer.IsValid() == false) || (vertexBuffer->IsValid() == false) || (m_VertexCounts[frameIndex] < imDrawData->TotalVtxCount))
        {
			Graphics::BufferProperties vertexBufferProps = vertexBuffer->GetProperties();
            vertexBufferProps.m_Size = vertexBufferSize;

            context.FrameContext.Api->CreateBuffer(vertexBuffer, vertexBufferProps);
            m_VertexCounts[frameIndex] = imDrawData->TotalVtxCount;
        }

        // Index buffer
        Graphics::BufferHandle& indexBuffer = m_IndexBuffers[frameIndex];
		if ((indexBuffer.IsValid() == false) || (indexBuffer->IsValid() == false) || (m_IndexCounts[frameIndex] < imDrawData->TotalIdxCount))
        {
			Graphics::BufferProperties indexBufferProps = indexBuffer->GetProperties();
            indexBufferProps.m_Size = indexBufferSize;

            context.FrameContext.Api->CreateBuffer(indexBuffer, indexBufferProps);
            m_IndexCounts[frameIndex] = imDrawData->TotalIdxCount;
        }

        // Upload data
        onyxS32 vertexCopyOffset = 0;
        onyxS32 indexCopyOffset = 0;

        for (int n = 0; n < imDrawData->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = imDrawData->CmdLists[n];
            vertexBuffer->SetData(vertexCopyOffset, cmd_list->VtxBuffer.Data,
                                    cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
            indexBuffer->SetData(indexCopyOffset, cmd_list->IdxBuffer.Data,
                                   cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

            vertexCopyOffset += cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
            indexCopyOffset += cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
        }
    }

    void UITask::Render(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer)
    {
        ONYX_UNUSED(context);

        ImDrawData* imDrawData = ImGui::GetDrawData();

        onyxS32 fb_width = numeric_cast<onyxS32>(imDrawData->DisplaySize.x * imDrawData->FramebufferScale.x);
        onyxS32 fb_height = numeric_cast<onyxS32>(imDrawData->DisplaySize.y * imDrawData->FramebufferScale.y);
        if (fb_width <= 0 || fb_height <= 0)
            return;

        if ((!imDrawData) || (imDrawData->CmdListsCount == 0))
        {
            return;
        }

        onyxS32 vertexOffset = 0;
        onyxS32 indexOffset = 0;

        const onyxU8 frameIndex = context.FrameContext.FrameIndex;
        const Graphics::BufferHandle& vertexBuffer = m_VertexBuffers[frameIndex];
        const Graphics::BufferHandle& indexBuffer = m_IndexBuffers[frameIndex];
        commandBuffer.BindVertexBuffer(vertexBuffer, 0, 0);
        commandBuffer.BindIndexBuffer(indexBuffer, 0, Graphics::IndexType::uint16);

        struct PushConstants
        {
            Vector2f scale;
            Vector2f translate;
        } constants;

        constants.scale = { 2.0f / imDrawData->DisplaySize.x, -2.0f / imDrawData->DisplaySize.y };
        constants.translate = { -1.0f - imDrawData->DisplayPos.x * constants.scale[0], 1.0f - imDrawData->DisplayPos.y * constants.scale[1] };
        commandBuffer.BindPushConstants(Graphics::ShaderStage::Vertex, 0, sizeof(PushConstants), &constants);
        ONYX_UNUSED(constants);
        /*Graphics::Viewport viewport;
        viewport.Rect.m_Position = { 0,0 };
        viewport.Rect.m_Size = { static_cast<onyxS16>(fb_width), static_cast<onyxS16>(fb_height) };
        viewport.MaxDepth = 1.0f;
        commandBuffer.SetViewport(viewport);*/
 
        // Will project scissor/clipping rectangles into framebuffer space
        ImVec2 clip_off = imDrawData->DisplayPos; // (0,0) unless using multi-viewports
        ImVec2 clip_scale = imDrawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

        for (onyxS32 i = 0; i < imDrawData->CmdListsCount; i++)
        {
            const ImDrawList* cmd_list = imDrawData->CmdLists[i];
            for (onyxS32 j = 0; j < cmd_list->CmdBuffer.Size; j++)
            {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
                if (pcmd->UserCallback != nullptr)
                {
                    // User callback, registered via ImDrawList::AddCallback()
                    // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                    //if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    //    commandBuffer.BindPushConstants(Graphics::ShaderStage::Vertex, 0, sizeof(PushConstants), &constants);
                    //else
                    pcmd->UserCallback(cmd_list, pcmd);
                }
                else
                {
                    ImTextureID textureId = pcmd->TextureId;

                    ImVec4 clip_rect;
                    clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                    clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                    clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                    clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

                    if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
                    {
                        // Negative offsets are illegal for vkCmdSetScissor
                        if (clip_rect.x < 0.0f)
                            clip_rect.x = 0.0f;
                        if (clip_rect.y < 0.0f)
                            clip_rect.y = 0.0f;

                        // Apply scissor/clipping rectangle
                        Graphics::Rect2Ds16 scissor;
                        scissor.m_Position[0] = (onyxS16)(clip_rect.x);
                        scissor.m_Position[1] = (onyxS16)(clip_rect.y);
                        scissor.m_Size[0] = (onyxS16)(std::abs(clip_rect.z - clip_rect.x));
                        scissor.m_Size[1] = (onyxS16)(std::abs(clip_rect.w - clip_rect.y));
                        //vkCmdSetScissor(command_buffer, 0, 1, &scissor);

                        commandBuffer.SetScissor(scissor);
                        commandBuffer.DrawIndexed(Graphics::PrimitiveTopology::Triangle, pcmd->ElemCount, 1,
                            pcmd->IdxOffset + indexOffset, pcmd->VtxOffset + vertexOffset, numeric_cast<onyxU32>(textureId));
                    }
                }
            }

            vertexOffset += cmd_list->VtxBuffer.Size;
            indexOffset += cmd_list->IdxBuffer.Size;

            Graphics::Rect2Ds16 scissor;
            scissor.m_Position = { 0, 0 };
            scissor.m_Size = { (onyxS16)fb_width, (onyxS16)fb_height };
            commandBuffer.SetScissor(scissor);
        }
    }

    void UITask::PostRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer)
    {
        ONYX_UNUSED(context);
        ONYX_UNUSED(commandBuffer);
    }

    void UITask::EndFrame(const Graphics::RenderGraphContext& context)
    {
        ONYX_UNUSED(context);
    }

    UIRenderGraphNode::UIRenderGraphNode()
    {
        m_ShaderPath = "engine:/shaders/imgui.oshader";

        AddInPin<InPin>();
        AddOutPin<OutPin>();
    }

    void UIRenderGraphNode::OnInit(Graphics::GraphicsApi& api, RenderGraphResourceCache& /*resourceCache*/)
    {
        /*Graphics::RenderGraphResourceInfo& input = m_Inputs.emplace_back();
        input.Id = Hash::FNV1aHash32("final");
        input.Name = "final";
        input.Type = Graphics::RenderGraphResourceType::Reference;*/

        //Graphics::RenderGraphResource& outputResource = resourceCache[GetOutputPin(0)->GetGlobalId()];

       /* Graphics::RenderGraphResourceInfo& output = m_Outputs.emplace_back();
        output.Id = Graphics::SWAPCHAIN_RESOURCE_ID;
        output.Name = "swapchain";
        output.Type = Graphics::RenderGraphResourceType::Attachment;

        Graphics::RenderGraphResource& resource = resourceCache[output.Id];
        resource.Info = output;
        resource.IsExternal = true;

        Graphics::RenderGraphTextureResourceInfo textureInfo;
        textureInfo.Format = Graphics::TextureFormat::BGRA_UNORM8;
        textureInfo.HasSize = false;
        textureInfo.LoadOp = Graphics::RenderPassSettings::LoadOp::Clear;
        resource.Properties = textureInfo;*/

        // texture and vertex buffers

        /*Graphics::BlendState& blendState = m_PipelineProperties.BlendStates.Emplace();
        blendState.IsBlendEnabled = true;
        blendState.SourceColor = Graphics::Blend::SrcAlpha;
        blendState.DestinationColor = Graphics::Blend::OneMinusSrcAlpha;*/
        
        m_Task.Init(api, m_ShaderEffect);
    }

    void UIRenderGraphNode::OnBeginFrame(const Graphics::RenderGraphContext& context)
    {
        ONYX_PROFILE_FUNCTION;

        // TODO: Fix
        // We set the output resource to the swapchain resource here
        Graphics::RenderGraphResource& outputResource = context.Graph.GetResource(GetOutputPin(0)->GetGlobalId());
        outputResource.Handle = context.Graph.GetResource(Graphics::SWAPCHAIN_RESOURCE_ID).Handle;

        m_Task.BeginFrame(context);
    }

    void UIRenderGraphNode::OnPreRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;
        m_Task.PreRender(context, commandBuffer);
    }

    void UIRenderGraphNode::OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;
        m_Task.Render(context, commandBuffer);
    }

    void UIRenderGraphNode::OnPostRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer)
    {
        ONYX_PROFILE_FUNCTION;
        m_Task.PostRender(context, commandBuffer);
    }

    void UIRenderGraphNode::OnEndFrame(const Graphics::RenderGraphContext& context)
    {
        ONYX_PROFILE_FUNCTION;
        m_Task.EndFrame(context);
    }

    // UI node graph
}
