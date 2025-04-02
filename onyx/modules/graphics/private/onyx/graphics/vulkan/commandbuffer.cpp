#include <onyx/graphics/vulkan/commandbuffer.h>

#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/shader/shadereffect.h>
#include <onyx/graphics/vulkan/buffer.h>
#include <onyx/graphics/vulkan/commandpool.h>
#include <onyx/graphics/vulkan/device.h>
#include <onyx/graphics/vulkan/descriptorset.h>
#include <onyx/graphics/vulkan/renderpass.h>
#include <onyx/graphics/vulkan/framebuffer.h>
#include <onyx/graphics/vulkan/pipeline.h>
#include <onyx/graphics/vulkan/pipelinelayout.h>
#include <onyx/graphics/vulkan/texture.h>
#include <onyx/graphics/vulkan/vulkan.h>

#include "onyx/graphics/vulkan/swapchain.h"

namespace Onyx::Graphics::Vulkan
{
    VulkanCommandBuffer::VulkanCommandBuffer(const VulkanGraphicsApi& api, const CommandPool& commandPool, VkCommandBufferLevel bufferLevel, onyxU8 frameIndex, const StringView& debugName)
        : m_Api(api)
        , m_FrameIndex(frameIndex)
    {
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool.GetHandle();
        allocInfo.level = bufferLevel;
        allocInfo.commandBufferCount = 1;

        VK_CHECK_RESULT(vkAllocateCommandBuffers(api.GetDevice().GetHandle(), &allocInfo, &m_CommandBuffer))
        SetResourceName(api.GetDevice().GetHandle(), VK_OBJECT_TYPE_COMMAND_BUFFER, (onyxU64)m_CommandBuffer, debugName);
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        // don't need to free it as we free it with the command pool
    }

    VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandBuffer&&) = default;

    void VulkanCommandBuffer::Reset()
    {
        ONYX_ASSERT(m_IsRecording == false, "CommandBuffer is still recording");

        m_CurrentRenderPass = {};
        m_CurrentFrameBuffer = {};
        m_CurrentPipeline = {};

        // reset descriptors
    }

    void VulkanCommandBuffer::Begin()
    {
        ONYX_ASSERT(m_IsRecording == false, "CommandBuffer is already recording");

        VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        beginInfo.pInheritanceInfo = nullptr; // Optional

        VK_CHECK_RESULT(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo))
        m_IsRecording = true;
    }

    void VulkanCommandBuffer::BeginSingleSubmit()
    {
        ONYX_ASSERT(m_IsRecording == false, "CommandBuffer is already recording");

        VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        beginInfo.pInheritanceInfo = nullptr; // Optional

        VK_CHECK_RESULT(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo))
        m_IsRecording = true;
    }

    void VulkanCommandBuffer::End()
    {
        ONYX_ASSERT(m_IsRecording, "CommandBuffer is not recording");
        VK_CHECK_RESULT(vkEndCommandBuffer(m_CommandBuffer))
        m_IsRecording = false;
    }

    void VulkanCommandBuffer::BeginRenderPass(const RenderPassHandle& renderPassHandle, const FramebufferHandle& framebufferHandle)
    {
        m_IsRecording = true;

        if (m_CurrentRenderPass)
            EndRenderPass();

        const VulkanRenderPass& renderPass = renderPassHandle.As<VulkanRenderPass>();
        const VulkanFramebuffer& frameBuffer = framebufferHandle.As<VulkanFramebuffer>();
        const FramebufferSettings& frameBufferSettings = frameBuffer.GetSettings();
        if (m_Api.IsDynamicRenderingEnabled())
        {
            const RenderPassSettings& renderPassSettings = renderPass.GetSettings();
            DynamicArray<VkRenderingAttachmentInfoKHR> colorAttachmentsInfo;
            colorAttachmentsInfo.reserve(frameBufferSettings.m_ColorTargets.size());

            bool hasDepthAttachment = false;
            VkRenderingAttachmentInfoKHR depthAttachmentInfo{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR };

            onyxU8 colorTargetIndex = 0;
            for (const RenderPassSettings::Attachment& attachment : renderPassSettings.m_Attachments)
            {
                if (Utils::IsDepthFormat(static_cast<TextureFormat>(attachment.m_Format)))
                {
                    ONYX_ASSERT(hasDepthAttachment == false, "Only 1 depth attachment supported.");
                    hasDepthAttachment = true;

                    //synchronization2_extension_present
                    depthAttachmentInfo.imageView = frameBufferSettings.m_DepthTarget.As<VulkanTexture>().GetHandle();
                    depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                    depthAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
                    depthAttachmentInfo.loadOp = static_cast<VkAttachmentLoadOp>(attachment.m_LoadOp);
                    depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                    depthAttachmentInfo.clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f } ;
                    depthAttachmentInfo.clearValue.depthStencil.depth = 1.0f;
                    depthAttachmentInfo.clearValue.depthStencil.stencil = 0;
                }
                else
                {
                    VkRenderingAttachmentInfoKHR& colorAttachmentInfo = colorAttachmentsInfo.emplace_back();
                    colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
                    colorAttachmentInfo.imageView = frameBufferSettings.m_ColorTargets[colorTargetIndex++].As<VulkanTexture>().GetHandle();
                    colorAttachmentInfo.imageLayout = m_Api.IsSynchronization2Enabled() ? VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    colorAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
                    colorAttachmentInfo.loadOp = static_cast<VkAttachmentLoadOp>(attachment.m_LoadOp);
                    colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                    colorAttachmentInfo.clearValue.color.float32[0] = attachment.m_ClearColor[0] / 255.0f;
                    colorAttachmentInfo.clearValue.color.float32[1] = attachment.m_ClearColor[1] / 255.0f;
                    colorAttachmentInfo.clearValue.color.float32[2] = attachment.m_ClearColor[2] / 255.0f;
                    colorAttachmentInfo.clearValue.color.float32[3] = attachment.m_ClearColor[3] / 255.0f;
                }
            }

            VkRenderingInfoKHR renderingInfo{ VK_STRUCTURE_TYPE_RENDERING_INFO_KHR };
            renderingInfo.flags = /*use_secondary ? VK_RENDERING_CONTENTS_SECONDARY_COMMAND_BUFFERS_BIT_KHR : */0;
            renderingInfo.renderArea = { 0, 0, frameBufferSettings.m_Width, frameBufferSettings.m_Height };
            renderingInfo.layerCount = 1;
            renderingInfo.viewMask = 0;
            renderingInfo.colorAttachmentCount = colorTargetIndex;
            renderingInfo.pColorAttachments = colorTargetIndex > 0 ? colorAttachmentsInfo.data() : nullptr;
            renderingInfo.pDepthAttachment = hasDepthAttachment ? &depthAttachmentInfo : nullptr;
            renderingInfo.pStencilAttachment = nullptr;

            // TODO: name render pass & framebuffer
            //SetResourceName(m_Api.GetDevice().GetHandle(), VK_OBJECT_TYPE_RENDER_PASS, &renderingInfo, "Test");

            vkCmdBeginRendering(m_CommandBuffer, &renderingInfo);
        }
        else
        {
            VkRenderPassBeginInfo renderPassBegin{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
            renderPassBegin.framebuffer = frameBuffer.GetHandle();
            renderPassBegin.renderPass = renderPass.GetHandle();

            renderPassBegin.renderArea.offset = { 0, 0 };
            renderPassBegin.renderArea.extent = { frameBufferSettings.m_Width, frameBufferSettings.m_Height };

            // TODO: Add to renderpass attachment?
            onyxU32 clearValuesCount = renderPass.GetSettings().m_Attachments.size();

            DynamicArray<VkClearValue> clearValues;
            clearValues.resize(clearValuesCount);
            for (onyxU32 cvi = 0; cvi < clearValuesCount; ++cvi)
                clearValues[cvi] = VkClearValue{};

            renderPassBegin.clearValueCount = clearValuesCount;
            renderPassBegin.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(m_CommandBuffer, &renderPassBegin, /*use_secondary ? VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS : */ VK_SUBPASS_CONTENTS_INLINE);
        }

        m_CurrentRenderPass = renderPassHandle;
        m_CurrentFrameBuffer = framebufferHandle;
    }

    void VulkanCommandBuffer::EndRenderPass()
    {
        ONYX_ASSERT(m_IsRecording, "CommandBuffer needs to be recording for this action.");
        ONYX_ASSERT(m_CurrentRenderPass, "RenderPass was not started");
        
        if (m_Api.IsDynamicRenderingEnabled())
            vkCmdEndRendering(m_CommandBuffer);
        else
            vkCmdEndRenderPass(m_CommandBuffer);

        m_CurrentRenderPass.Reset();
        m_CurrentFrameBuffer.Reset();
        m_CurrentPipeline.Reset();
    }

    void VulkanCommandBuffer::BindShaderEffect(const ShaderEffectHandle& shaderEffect)
    {
        ONYX_ASSERT(m_IsRecording, "CommandBuffer needs to be recording for this action.");
        ONYX_ASSERT(m_CurrentRenderPass || shaderEffect->IsCompute(), "RenderPass was not started");

        m_CurrentShaderEffect = shaderEffect;
        BindPipeline(shaderEffect->GetPipeline());
        BindDescriptorSets();
    }

    void VulkanCommandBuffer::BindPipeline(const PipelineHandle& pipelineHandle)
    {
        const Pipeline& pipeline = pipelineHandle.As<Pipeline>();
        vkCmdBindPipeline(m_CommandBuffer, pipeline.GetBindPoint(), pipeline.GetHandle());

        m_CurrentPipeline = pipelineHandle;
    }

    void VulkanCommandBuffer::BindVertexBuffer(const BufferHandle& bufferHandle, onyxU32 binding, onyxU32 offset)
    {
        const VulkanBuffer& buffer = bufferHandle.As<VulkanBuffer>();
        VkDeviceSize offsets[] = { offset };

        // add support for parent vertex buffers?
        vkCmdBindVertexBuffers(m_CommandBuffer, binding, 1, buffer.GetHandlePtr(), offsets);
    }

    void VulkanCommandBuffer::BindVertexBuffers(const InplaceArray<BufferHandle, 8>& bufferHandles, const InplaceArray<onyxU32, 8> bufferOffsets, onyxU32 firstBinding, onyxU32 bindingCount)
    {
        // TODO: find way to pass buffers as const ref, maybe use std::reference_wrapper
        ONYX_UNUSED(bufferHandles);
        ONYX_UNUSED(bufferOffsets);
        ONYX_UNUSED(firstBinding);
        ONYX_UNUSED(bindingCount);
    }

    void VulkanCommandBuffer::BindIndexBuffer(const BufferHandle& bufferHandle, onyxU32 offset, IndexType indexType)
    {
        const VulkanBuffer& buffer = bufferHandle.As<VulkanBuffer>();
        
        // add support for parent index buffers?
        /*if (buffer->parent_buffer.index != k_invalid_index) {
            Buffer* parent_buffer = device->access_buffer(buffer->parent_buffer);
            vk_buffer = parent_buffer->vk_buffer;
            offset = buffer->global_offset;
        }*/

        VkIndexType vkIndexType = VK_INDEX_TYPE_NONE_KHR;
        switch (indexType)
        {
            case IndexType::None:
                vkIndexType = VK_INDEX_TYPE_NONE_KHR;
                break;
            case IndexType::uint8:
                vkIndexType = VK_INDEX_TYPE_UINT8_EXT;
                break;
            case IndexType::uint16:
                vkIndexType = VK_INDEX_TYPE_UINT16;
                break;
            case IndexType::uint32:
                vkIndexType = VK_INDEX_TYPE_UINT32;
                break;
        }

        vkCmdBindIndexBuffer(m_CommandBuffer, buffer.GetHandle(), offset, vkIndexType);
    }
    
    void VulkanCommandBuffer::BindPushConstants(ShaderStage stage, onyxU32 offset, onyxU32 size, void* data)
    {
        const Vulkan::PipelineLayout& pipelineLayout = m_CurrentPipeline->GetPipelineLayout();
        vkCmdPushConstants(m_CommandBuffer, pipelineLayout.GetHandle(), ToVulkanStage(stage), offset, size, data);
    }

    void VulkanCommandBuffer::Bind(const TextureHandle& texture, const String& bindingName)
    {
        ONYX_ASSERT(m_CurrentShaderEffect, "No ShaderEffect is active.");
        m_CurrentShaderEffect->Bind(texture, bindingName, m_FrameIndex);
    }

    void VulkanCommandBuffer::Bind(const BufferHandle& buffer, const String& bindingName)
    {
        ONYX_ASSERT(m_CurrentShaderEffect, "No ShaderEffect is active.");
        m_CurrentShaderEffect->Bind(buffer, bindingName, m_FrameIndex);
    }

    void VulkanCommandBuffer::BindDescriptorSets()
    {
        onyxU8 firstSet = 0;

        DynamicArray<VkDescriptorSet> vkDescriptorSets;
        if (m_Api.IsBindless())
        {
            const DescriptorSet& bindlessDescriptorSet = m_Api.GetBindlessDescriptorSet();
            vkDescriptorSets.push_back(bindlessDescriptorSet.GetHandle());
        }
       
        if (m_CurrentShaderEffect->HasDescriptorSets())
        {
            const DynamicArray<DescriptorSetHandle>& descriptorSets = m_CurrentShaderEffect->GetDescriptorSets(m_FrameIndex);

            onyxU8 lastSet = 0;
            for (const DescriptorSetHandle& descriptorSet : descriptorSets)
            {
                if (descriptorSet.IsValid() == false)
                    continue;

                if ((descriptorSet->GetSet() - 1) == lastSet)
                {
                    const DescriptorSet& vulkanDescriptorSet = descriptorSet.As<DescriptorSet>();
                    vkDescriptorSets.push_back(vulkanDescriptorSet.GetHandle());
                }
                else
                {
                    if (vkDescriptorSets.empty() == false)
                    {
                        const PipelineLayout& pipelineLayout = m_CurrentPipeline->GetPipelineLayout();
                        vkCmdBindDescriptorSets(m_CommandBuffer, m_CurrentPipeline->GetBindPoint(), pipelineLayout.GetHandle(), firstSet, static_cast<onyxU32>(vkDescriptorSets.size()), vkDescriptorSets.data(), 0, nullptr);
                    }

                    firstSet = descriptorSet->GetSet();
                    vkDescriptorSets.clear();
                }

                lastSet = descriptorSet->GetSet();
            }
        }

        // push remaining descriptor sets
        if (vkDescriptorSets.empty() == false)
        {
            const PipelineLayout& pipelineLayout = m_CurrentPipeline->GetPipelineLayout();
            vkCmdBindDescriptorSets(m_CommandBuffer, m_CurrentPipeline->GetBindPoint(), pipelineLayout.GetHandle(), firstSet, static_cast<onyxU32>(vkDescriptorSets.size()), vkDescriptorSets.data(), 0, nullptr);
        }
    }

    void VulkanCommandBuffer::SetViewport()
    {
        VkViewport vkViewport;
        vkViewport.x = 0.f;

        if (m_CurrentRenderPass)
        {
            const FramebufferSettings& frameBufferSettings = m_CurrentFrameBuffer->GetSettings();
            vkViewport.width = static_cast<onyxF32>(frameBufferSettings.m_Width);
            // Invert Y with negative height and proper offset - Vulkan has unique Clipping Y.
            vkViewport.y = static_cast<onyxF32>(frameBufferSettings.m_Height);
            vkViewport.height = -vkViewport.y;
        }
        else
        {
            const Vector2s32& swapchainExtent = m_Api.GetSwapChain().GetExtent();
            // TODO: get swap chain size
            vkViewport.width = swapchainExtent[0] * 1.f;
            // Invert Y with negative height and proper offset - Vulkan has unique Clipping Y.
            vkViewport.y = swapchainExtent[1] * 1.f;
            vkViewport.height = -swapchainExtent[1] * 1.f;
        }

        vkViewport.minDepth = 0.0f;
        vkViewport.maxDepth = 1.0f;

        vkCmdSetViewport(m_CommandBuffer, 0, 1, &vkViewport);
    }

    void VulkanCommandBuffer::SetViewport(const Viewport& viewport)
    {
        VkViewport vkViewport;

        vkViewport.x = viewport.Rect.m_Position[0] * 1.f;
        vkViewport.width = viewport.Rect.m_Size[0] * 1.f;
        // Invert Y with negative height and proper offset - Vulkan has unique Clipping Y.
        vkViewport.y = viewport.Rect.m_Size[1] * 1.f - viewport.Rect.m_Position[1];
        vkViewport.height = -viewport.Rect.m_Size[1] * 1.f;
        vkViewport.minDepth = viewport.MinDepth;
        vkViewport.maxDepth = viewport.MaxDepth;

        vkCmdSetViewport(m_CommandBuffer, 0, 1, &vkViewport);
    }

    void VulkanCommandBuffer::SetScissor()
    {
        VkRect2D vkScissor;
        vkScissor.offset.x = 0;
        vkScissor.offset.y = 0;

        const Vector2s32& swapchainExtent = m_Api.GetSwapChain().GetExtent();
        vkScissor.extent.width = swapchainExtent[0];
        vkScissor.extent.height = swapchainExtent[1];

        vkCmdSetScissor(m_CommandBuffer, 0, 1, &vkScissor);
    }

    void VulkanCommandBuffer::SetScissor(const Rect2Ds16& scissorRect)
    {
        VkRect2D vkScissor;
        vkScissor.offset.x = scissorRect.m_Position[0];
        vkScissor.offset.y = scissorRect.m_Position[1];
        vkScissor.extent.width = scissorRect.m_Size[0];
        vkScissor.extent.height = scissorRect.m_Size[1];

        vkCmdSetScissor(m_CommandBuffer, 0, 1, &vkScissor);
    }

    void VulkanCommandBuffer::ClearColor(onyxF32 red, onyxF32 green, onyxF32 blue, onyxF32 alpha, onyxU32 attachmentIndex)
    {
        ONYX_UNUSED(red);
        ONYX_UNUSED(green);
        ONYX_UNUSED(blue);
        ONYX_UNUSED(alpha);
        ONYX_UNUSED(attachmentIndex);
    }

    void VulkanCommandBuffer::ClearDepthStencil(onyxF32 depth, onyxU8 stencil)
    {
        ONYX_UNUSED(depth);
        ONYX_UNUSED(stencil);
    }

    void VulkanCommandBuffer::Draw(PrimitiveTopology /*topology*/, onyxU32 firstVertex, onyxU32 vertexCount, onyxU32 firstInstance, onyxU32 instanceCount)
    {
        PreDraw();
        vkCmdDraw(m_CommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void VulkanCommandBuffer::DrawIndexed(PrimitiveTopology /*topology*/, onyxU32 indexCount, onyxU32 instanceCount, onyxU32 firstIndex, onyxS32 vertexOffset, onyxU32 firstInstance)
    {
        PreDraw();
        vkCmdDrawIndexed(m_CommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void VulkanCommandBuffer::DrawIndirect(const BufferHandle& bufferHandle, onyxU32 drawCount, onyxU32 offset, onyxU32 stride)
    {
        PreDraw();

        const VulkanBuffer& vkBuffer = bufferHandle.As<VulkanBuffer>();

        VkDeviceSize vkOffset = offset;
        vkCmdDrawIndirect(m_CommandBuffer, vkBuffer.GetHandle(), vkOffset, drawCount, stride);
    }

    void VulkanCommandBuffer::DrawIndirectCount(const BufferHandle& argumentBufferHandle, onyxU32 argumentOffset, const BufferHandle& countBufferHandle, onyxU32 countOffset, onyxU32 maxDraws, onyxU32 stride)
    {
        PreDraw();

        const VulkanBuffer& vkArgumentBuffer = argumentBufferHandle.As<VulkanBuffer>();
        const VulkanBuffer& vkCountBuffer = countBufferHandle.As<VulkanBuffer>();

        vkCmdDrawIndirectCount(m_CommandBuffer, vkArgumentBuffer.GetHandle(), argumentOffset, vkCountBuffer.GetHandle(), countOffset, maxDraws, stride);
    }

    void VulkanCommandBuffer::DrawIndexedIndirect(const BufferHandle& bufferHandle, onyxU32 drawCount, onyxU32 offset, onyxU32 stride)
    {
        PreDraw();

        const VulkanBuffer& vkBuffer = bufferHandle.As<VulkanBuffer>();
        VkDeviceSize vkOffset = offset;

        vkCmdDrawIndexedIndirect(m_CommandBuffer, vkBuffer.GetHandle(), vkOffset, drawCount, stride);
    }

    void VulkanCommandBuffer::DrawMeshTask(onyxU32 taskCount, onyxU32 firstTask)
    {
        PreDraw();

        ONYX_ASSERT(vkCmdDrawMeshTasksNV != nullptr, "Mesh shader extension is not initialized.");
        vkCmdDrawMeshTasksNV(m_CommandBuffer, taskCount, firstTask);
    }

    void VulkanCommandBuffer::DrawMeshTaskIndirect(const BufferHandle& argumentBufferHandle, onyxU32 argumentOffset, const BufferHandle& countBufferHandle, onyxU32 countOffset, onyxU32 maxDraws, onyxU32 stride)
    {
        PreDraw();

        ONYX_ASSERT(vkCmdDrawMeshTasksIndirectCountNV != nullptr, "Mesh shader extension is not initialized.");

        const VulkanBuffer& vkArgumentBuffer = argumentBufferHandle.As<VulkanBuffer>();
        const VulkanBuffer& vkCountBuffer = countBufferHandle.As<VulkanBuffer>();

        vkCmdDrawMeshTasksIndirectCountNV(m_CommandBuffer, vkArgumentBuffer.GetHandle(), argumentOffset, vkCountBuffer.GetHandle(), countOffset, maxDraws, stride);
    }

    void VulkanCommandBuffer::Dispatch(onyxU32 groupX, onyxU32 groupY, onyxU32 groupZ)
    {
        PreDraw();

        vkCmdDispatch(m_CommandBuffer, groupX, groupY, groupZ);
    }

    void VulkanCommandBuffer::DispatchIndirect(const BufferHandle& bufferHandle, onyxU32 offset)
    {
        const VulkanBuffer& vkBuffer = bufferHandle.As<VulkanBuffer>();
        vkCmdDispatchIndirect(m_CommandBuffer, vkBuffer.GetHandle(), offset);
    }

    void VulkanCommandBuffer::Copy(const BufferHandle& source, BufferHandle& destination)
    {
        const VulkanBuffer& vkSourceBuffer = source.As<VulkanBuffer>();
        VulkanBuffer& vkDestinationBuffer = destination.As<VulkanBuffer>();

        VkBufferCopy copyRegion{};
        copyRegion.size = source->GetProperties().m_Size;
        vkCmdCopyBuffer(m_CommandBuffer, vkSourceBuffer.GetHandle(), vkDestinationBuffer.GetHandle(), 1, &copyRegion);
    }

    void VulkanCommandBuffer::GlobalBarrier(VkAccessFlagBits2 srcAccess, VkAccessFlagBits2 dstAccess)
    {
        VkMemoryBarrier2 barrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };

        barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;
        barrier.srcAccessMask = srcAccess;
        barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;
        barrier.dstAccessMask = dstAccess;

        VkDependencyInfoKHR dependency_info{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR };
        dependency_info.memoryBarrierCount = 1;
        dependency_info.pMemoryBarriers = &barrier;

        vkCmdPipelineBarrier2(m_CommandBuffer, &dependency_info);
    }

    void VulkanCommandBuffer::PreDraw()
    {
        m_CurrentShaderEffect->PreDraw(m_FrameIndex);
    }

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    void VulkanCommandBuffer::BeginDebugLabel(const StringView& label, const Vector4f& color)
    {
        if (vkCmdBeginDebugUtilsLabelEXT != nullptr)
        {
            VkDebugUtilsLabelEXT vkDebugLabel;
            vkDebugLabel.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
            vkDebugLabel.pLabelName = label.data();
            vkDebugLabel.color[0] = color[0];
            vkDebugLabel.color[1] = color[1];
            vkDebugLabel.color[2] = color[2];
            vkDebugLabel.color[3] = color[3];
            vkDebugLabel.pNext = nullptr;
            vkCmdBeginDebugUtilsLabelEXT(m_CommandBuffer, &vkDebugLabel);
        }
    }

    void VulkanCommandBuffer::EndDebugLabel()
    {
        if (vkCmdEndDebugUtilsLabelEXT != nullptr)
            vkCmdEndDebugUtilsLabelEXT(m_CommandBuffer);
    }
#endif
}
