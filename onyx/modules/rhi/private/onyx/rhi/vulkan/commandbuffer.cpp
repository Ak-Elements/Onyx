#include <onyx/rhi/vulkan/commandbuffer.h>

#include <onyx/rhi/shader/shaderinstance.h>
#include <onyx/rhi/vulkan/buffer.h>
#include <onyx/rhi/vulkan/commandpool.h>
#include <onyx/rhi/vulkan/descriptorset.h>
#include <onyx/rhi/vulkan/device.h>
#include <onyx/rhi/vulkan/framebuffer.h>
#include <onyx/rhi/vulkan/pipeline.h>
#include <onyx/rhi/vulkan/pipelinelayout.h>
#include <onyx/rhi/vulkan/renderpass.h>
#include <onyx/rhi/vulkan/texture.h>
#include <onyx/rhi/vulkan/vulkan.h>

#include <onyx/rhi/vulkan/swapchain.h>

#include <onyx/geometry/rect2.h>

namespace onyx::rhi::vulkan {
VulkanCommandBuffer::VulkanCommandBuffer( const VulkanGraphicsApi& api,
                                          const CommandPool& commandPool,
                                          VkCommandBufferLevel bufferLevel,
                                          uint8_t frameIndex,
                                          StringView debugName )
    : m_Api( api )
    , m_FrameIndex( frameIndex ) {
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool.GetHandle();
    allocInfo.level = bufferLevel;
    allocInfo.commandBufferCount = 1;

    VK_CHECK_RESULT( vkAllocateCommandBuffers( api.getDevice().GetHandle(), &allocInfo, &m_CommandBuffer ) )
    SetResourceName( api.getDevice().GetHandle(), VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64_t)m_CommandBuffer, debugName );
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
    // don't need to free it as we free it with the command pool
}

VulkanCommandBuffer::VulkanCommandBuffer( VulkanCommandBuffer&& ) = default;

void VulkanCommandBuffer::reset() {
    ONYX_ASSERT( m_IsRecording == false, "CommandBuffer is still recording" );

    m_CurrentRenderPass = {};
    m_CurrentFrameBuffer = {};
    m_CurrentShaderEffect.reset();
    // reset descriptors
}

void VulkanCommandBuffer::begin() {
    ONYX_ASSERT( m_IsRecording == false, "CommandBuffer is already recording" );

    VkCommandBufferBeginInfo beginInfo;
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo = nullptr; // Optional
    beginInfo.pNext = nullptr;

    VK_CHECK_RESULT( vkBeginCommandBuffer( m_CommandBuffer, &beginInfo ) )
    m_IsRecording = true;
}

void VulkanCommandBuffer::beginSingleSubmit() {
    ONYX_ASSERT( m_IsRecording == false, "CommandBuffer is already recording" );

    VkCommandBufferBeginInfo beginInfo;
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    ;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = nullptr; // Optional
    beginInfo.pNext = nullptr;

    VK_CHECK_RESULT( vkBeginCommandBuffer( m_CommandBuffer, &beginInfo ) )
    m_IsRecording = true;
}

void VulkanCommandBuffer::end() {
    ONYX_ASSERT( m_IsRecording, "CommandBuffer is not recording" );
    VK_CHECK_RESULT( vkEndCommandBuffer( m_CommandBuffer ) )
    m_IsRecording = false;
}

void VulkanCommandBuffer::beginRenderPass( const RenderPassHandle& renderPassHandle,
                                           const FramebufferHandle& framebufferHandle ) {
    m_IsRecording = true;

    if( m_CurrentRenderPass )
        endRenderPass();

    const VulkanRenderPass& renderPass = renderPassHandle.as< VulkanRenderPass >();
    const VulkanFramebuffer& frameBuffer = framebufferHandle.as< VulkanFramebuffer >();
    const FramebufferSettings& frameBufferSettings = frameBuffer.GetSettings();
    if( m_Api.isDynamicRenderingEnabled() ) {
        const RenderPassSettings& renderPassSettings = renderPass.GetSettings();
        DynamicArray< VkRenderingAttachmentInfoKHR > colorAttachmentsInfo;
        colorAttachmentsInfo.reserve( frameBufferSettings.m_ColorTargets.size() );

        bool hasDepthAttachment = false;
        VkRenderingAttachmentInfoKHR depthAttachmentInfo{};
        depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        depthAttachmentInfo.pNext = nullptr;

        uint8_t colorTargetIndex = 0;
        for( const RenderPassSettings::Attachment& attachment : renderPassSettings.m_Attachments ) {
            if( Utils::IsDepthFormat( static_cast< TextureFormat >( attachment.m_Format ) ) ) {
                ONYX_ASSERT( hasDepthAttachment == false, "Only 1 depth attachment supported." );
                hasDepthAttachment = true;

                // synchronization2_extension_present
                depthAttachmentInfo.imageView = frameBufferSettings.m_DepthTarget.as< VulkanTexture >().GetHandle();
                depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                depthAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
                depthAttachmentInfo.loadOp = static_cast< VkAttachmentLoadOp >( attachment.m_LoadOp );
                depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                depthAttachmentInfo.clearValue.color.float32[ 0 ] = 0.0f;
                depthAttachmentInfo.clearValue.color.float32[ 1 ] = 0.0f;
                depthAttachmentInfo.clearValue.color.float32[ 2 ] = 0.0f;
                depthAttachmentInfo.clearValue.color.float32[ 3 ] = 1.0f;
                depthAttachmentInfo.clearValue.depthStencil.depth = 1.0f;
                depthAttachmentInfo.clearValue.depthStencil.stencil = 0;
            } else {
                VkRenderingAttachmentInfoKHR& colorAttachmentInfo = colorAttachmentsInfo.emplace_back();
                colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
                colorAttachmentInfo.imageView = frameBufferSettings.m_ColorTargets[ colorTargetIndex++ ]
                                                    .as< VulkanTexture >()
                                                    .GetHandle();
                colorAttachmentInfo.imageLayout = m_Api.isSynchronization2Enabled()
                                                      ? VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR
                                                      : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                colorAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
                colorAttachmentInfo.loadOp = static_cast< VkAttachmentLoadOp >( attachment.m_LoadOp );
                colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                colorAttachmentInfo.clearValue.color.float32[ 0 ] = attachment.m_ClearColor[ 0 ] / 255.0f;
                colorAttachmentInfo.clearValue.color.float32[ 1 ] = attachment.m_ClearColor[ 1 ] / 255.0f;
                colorAttachmentInfo.clearValue.color.float32[ 2 ] = attachment.m_ClearColor[ 2 ] / 255.0f;
                colorAttachmentInfo.clearValue.color.float32[ 3 ] = attachment.m_ClearColor[ 3 ] / 255.0f;
            }
        }

        VkRenderingInfoKHR renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
        renderingInfo.flags = /*use_secondary ? VK_RENDERING_CONTENTS_SECONDARY_COMMAND_BUFFERS_BIT_KHR : */ 0;
        renderingInfo.renderArea.offset = VkOffset2D{ 0, 0 };
        renderingInfo.renderArea.extent = VkExtent2D{ frameBufferSettings.m_Width, frameBufferSettings.m_Height };
        renderingInfo.layerCount = 1;
        renderingInfo.viewMask = 0;
        renderingInfo.colorAttachmentCount = colorTargetIndex;
        renderingInfo.pColorAttachments = colorTargetIndex > 0 ? colorAttachmentsInfo.data() : nullptr;
        renderingInfo.pDepthAttachment = hasDepthAttachment ? &depthAttachmentInfo : nullptr;
        renderingInfo.pStencilAttachment = nullptr;
        renderingInfo.pNext = nullptr;

        // TODO: name render pass & framebuffer
        // SetResourceName(m_Api.GetDevice().GetHandle(), VK_OBJECT_TYPE_RENDER_PASS, &renderingInfo, "Test");

        vkCmdBeginRendering( m_CommandBuffer, &renderingInfo );
    } else {
        VkRenderPassBeginInfo renderPassBegin{};
        renderPassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBegin.framebuffer = frameBuffer.GetHandle();
        renderPassBegin.renderPass = renderPass.GetHandle();
        renderPassBegin.renderArea.offset = { 0, 0 };
        renderPassBegin.renderArea.extent = { frameBufferSettings.m_Width, frameBufferSettings.m_Height };
        renderPassBegin.pNext = nullptr;

        // TODO: Add to renderpass attachment?
        uint32_t clearValuesCount = renderPass.GetSettings().m_Attachments.size();

        DynamicArray< VkClearValue > clearValues;
        clearValues.resize( clearValuesCount );
        for( uint32_t cvi = 0; cvi < clearValuesCount; ++cvi )
            clearValues[ cvi ] = VkClearValue{};

        renderPassBegin.clearValueCount = clearValuesCount;
        renderPassBegin.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(
            m_CommandBuffer,
            &renderPassBegin,
            /*use_secondary ? VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS : */ VK_SUBPASS_CONTENTS_INLINE );
    }

    m_CurrentRenderPass = renderPassHandle;
    m_CurrentFrameBuffer = framebufferHandle;
}

void VulkanCommandBuffer::endRenderPass() {
    ONYX_ASSERT( m_IsRecording, "CommandBuffer needs to be recording for this action." );
    ONYX_ASSERT( m_CurrentRenderPass, "RenderPass was not started" );

    if( m_Api.isDynamicRenderingEnabled() )
        vkCmdEndRendering( m_CommandBuffer );
    else
        vkCmdEndRenderPass( m_CommandBuffer );

    m_CurrentRenderPass.reset();
    m_CurrentFrameBuffer.reset();
}

void VulkanCommandBuffer::bindShaderEffect( const ShaderInstanceHandle& shader ) {
    ONYX_ASSERT( m_IsRecording, "CommandBuffer needs to be recording for this action." );
    ONYX_ASSERT( m_CurrentRenderPass || shader->IsCompute(), "RenderPass was not started" );

    m_CurrentShaderEffect = shader;

    const Pipeline& pipeline = shader->GetPipeline().as< Pipeline >();
    VkPipeline vkPipeline = pipeline.GetHandle();
    VkPipelineLayout vkLayout = pipeline.GetPipelineLayout().GetHandle();
    VkPipelineBindPoint vkBindPoint = pipeline.GetBindPoint();

    vkCmdBindPipeline( m_CommandBuffer, vkBindPoint, vkPipeline );
    BindDescriptorSets( vkLayout, vkBindPoint );
}

void VulkanCommandBuffer::bindVertexBuffer( const BufferHandle& bufferHandle, uint32_t binding, uint32_t offset ) {
    const VulkanBuffer& buffer = bufferHandle.Buffer.as< VulkanBuffer >();
    VkDeviceSize offsets[] = { offset };

    // add support for parent vertex buffers?
    vkCmdBindVertexBuffers( m_CommandBuffer, binding, 1, buffer.GetHandlePtr(), offsets );
}

void VulkanCommandBuffer::bindVertexBuffers( [[maybe_unused]] const InplaceArray< BufferHandle, 8 >& bufferHandles,
                                             [[maybe_unused]] const InplaceArray< uint32_t, 8 > bufferOffsets,
                                             [[maybe_unused]] uint32_t firstBinding,
                                             [[maybe_unused]] uint32_t bindingCount ) {
    // TODO: find way to pass buffers as const ref, maybe use std::reference_wrapper
}

void VulkanCommandBuffer::bindIndexBuffer( const BufferHandle& bufferHandle, uint32_t offset, IndexType indexType ) {
    const VulkanBuffer& buffer = bufferHandle.Buffer.as< VulkanBuffer >();

    // add support for parent index buffers?
    /*if (buffer->parent_buffer.index != k_invalid_index) {
        Buffer* parent_buffer = device->access_buffer(buffer->parent_buffer);
        vk_buffer = parent_buffer->vk_buffer;
        offset = buffer->global_offset;
    }*/

    VkIndexType vkIndexType = VK_INDEX_TYPE_NONE_KHR;
    switch( indexType ) {
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

    vkCmdBindIndexBuffer( m_CommandBuffer, buffer.GetHandle(), offset, vkIndexType );
}

void VulkanCommandBuffer::bindPushConstants( ShaderStage stage, uint32_t offset, uint32_t size, const void* data ) {
    vkCmdPushConstants( m_CommandBuffer, GetPipelineLayout(), ToVulkanStage( stage ), offset, size, data );
}

void VulkanCommandBuffer::beginConditionalRendering( const BufferHandle& conditionalBuffer, uint32_t offset ) {
    ONYX_ASSERT( g_vkCmdBeginConditionalRenderingExt != nullptr );

    const VulkanBuffer& vkBuffer = conditionalBuffer.Buffer.as< VulkanBuffer >();

    VkConditionalRenderingBeginInfoEXT conditionalRenderingInfo{};
    conditionalRenderingInfo.sType = VK_STRUCTURE_TYPE_CONDITIONAL_RENDERING_BEGIN_INFO_EXT;
    conditionalRenderingInfo.buffer = vkBuffer.GetHandle();
    conditionalRenderingInfo.flags = 0;
    conditionalRenderingInfo.offset = offset;

    g_vkCmdBeginConditionalRenderingExt( m_CommandBuffer, &conditionalRenderingInfo );
}

void VulkanCommandBuffer::endConditionalRendering() {
    ONYX_ASSERT( g_vkCmdEndConditionalRenderingExt != nullptr );
    g_vkCmdEndConditionalRenderingExt( m_CommandBuffer );
}

VkPipelineLayout VulkanCommandBuffer::GetPipelineLayout() const {
    ONYX_ASSERT( m_CurrentShaderEffect.isValid() );
    return m_CurrentShaderEffect->GetPipeline().as< Pipeline >().GetPipelineLayout().GetHandle();
}

void VulkanCommandBuffer::bind( const TextureHandle& texture, const String& bindingName ) {
    ONYX_ASSERT( m_CurrentShaderEffect, "No ShaderEffect is active." );
    m_CurrentShaderEffect->Bind( texture, bindingName, m_FrameIndex );
}

void VulkanCommandBuffer::bind( const BufferHandle& buffer, const String& bindingName ) {
    ONYX_ASSERT( m_CurrentShaderEffect, "No ShaderEffect is active." );
    m_CurrentShaderEffect->Bind( buffer, bindingName, m_FrameIndex );
}

void VulkanCommandBuffer::barrier( BufferHandle& buffer, Context newContext, Access newAccess ) {
    buffer.Buffer->Barrier( *this, newContext, newAccess, buffer.Alias );
}

void VulkanCommandBuffer::transitionLayout( TextureHandle& texture,
                                            Context newContext,
                                            Access newAccess,
                                            ImageLayout newLayout ) {
    texture.Storage->TransitionLayout( *this, newContext, newAccess, newLayout );
}

void VulkanCommandBuffer::BindDescriptorSets( VkPipelineLayout pipelineLayout, VkPipelineBindPoint bindingPoint ) {
    uint8_t firstSet = 0;

    DynamicArray< VkDescriptorSet > vkDescriptorSets;
    if( m_Api.isBindless() ) {
        const DescriptorSet& bindlessDescriptorSet = m_Api.getBindlessDescriptorSet();
        vkDescriptorSets.push_back( bindlessDescriptorSet.GetHandle() );
    }

    if( m_CurrentShaderEffect->HasDescriptorSets() ) {
        const DynamicArray< DescriptorSetHandle >& descriptorSets = m_CurrentShaderEffect->GetDescriptorSets(
            m_FrameIndex );

        uint8_t lastSet = 0;
        for( const DescriptorSetHandle& descriptorSet : descriptorSets ) {
            if( descriptorSet.isValid() == false )
                continue;

            if( ( descriptorSet->GetSet() - 1 ) == lastSet ) {
                const DescriptorSet& vulkanDescriptorSet = descriptorSet.as< DescriptorSet >();
                vkDescriptorSets.push_back( vulkanDescriptorSet.GetHandle() );
            } else {
                if( vkDescriptorSets.empty() == false ) {
                    vkCmdBindDescriptorSets( m_CommandBuffer,
                                             bindingPoint,
                                             pipelineLayout,
                                             firstSet,
                                             static_cast< uint32_t >( vkDescriptorSets.size() ),
                                             vkDescriptorSets.data(),
                                             0,
                                             nullptr );
                }

                firstSet = descriptorSet->GetSet();
                vkDescriptorSets.clear();
            }

            lastSet = descriptorSet->GetSet();
        }
    }

    // push remaining descriptor sets
    if( vkDescriptorSets.empty() == false ) {
        vkCmdBindDescriptorSets( m_CommandBuffer,
                                 bindingPoint,
                                 pipelineLayout,
                                 firstSet,
                                 static_cast< uint32_t >( vkDescriptorSets.size() ),
                                 vkDescriptorSets.data(),
                                 0,
                                 nullptr );
    }
}

void VulkanCommandBuffer::setViewport() {
    VkViewport vkViewport;
    vkViewport.x = 0.f;

    if( m_CurrentRenderPass ) {
        const FramebufferSettings& frameBufferSettings = m_CurrentFrameBuffer->GetSettings();
        vkViewport.width = static_cast< float32 >( frameBufferSettings.m_Width );
        // Invert Y with negative height and proper offset - Vulkan has unique Clipping Y.
        vkViewport.y = static_cast< float32 >( frameBufferSettings.m_Height );
        vkViewport.height = -vkViewport.y;
    } else {
        const Vector2s32& swapchainExtent = m_Api.getSwapChain().GetExtent();
        // TODO: get swap chain size
        vkViewport.width = swapchainExtent[ 0 ] * 1.f;
        // Invert Y with negative height and proper offset - Vulkan has unique Clipping Y.
        vkViewport.y = swapchainExtent[ 1 ] * 1.f;
        vkViewport.height = -swapchainExtent[ 1 ] * 1.f;
    }

    vkViewport.minDepth = 0.0f;
    vkViewport.maxDepth = 1.0f;

    vkCmdSetViewport( m_CommandBuffer, 0, 1, &vkViewport );
}

void VulkanCommandBuffer::setViewport( const Viewport& viewport ) {
    VkViewport vkViewport;

    vkViewport.x = numericCast< float32 >( viewport.X );
    vkViewport.width = numericCast< float32 >( viewport.Width );
    // Invert Y with negative height and proper offset - Vulkan has unique Clipping Y.
    vkViewport.y = numericCast< float32 >( viewport.Height - viewport.Y );
    vkViewport.height = numericCast< float32 >( -viewport.Height );
    vkViewport.minDepth = viewport.MinDepth;
    vkViewport.maxDepth = viewport.MaxDepth;

    vkCmdSetViewport( m_CommandBuffer, 0, 1, &vkViewport );
}

void VulkanCommandBuffer::setScissor() {
    VkRect2D vkScissor;
    vkScissor.offset.x = 0;
    vkScissor.offset.y = 0;

    const Vector2s32& swapchainExtent = m_Api.getSwapChain().GetExtent();
    vkScissor.extent.width = swapchainExtent[ 0 ];
    vkScissor.extent.height = swapchainExtent[ 1 ];

    vkCmdSetScissor( m_CommandBuffer, 0, 1, &vkScissor );
}

void VulkanCommandBuffer::setScissor( Rect2s16 scissorRect ) {
    VkRect2D vkScissor;

    vkScissor.offset.x = scissorRect.Position[ 0 ];
    vkScissor.offset.y = scissorRect.Position[ 1 ];
    vkScissor.extent.width = scissorRect.Extents[ 0 ];
    vkScissor.extent.height = scissorRect.Extents[ 1 ];

    vkCmdSetScissor( m_CommandBuffer, 0, 1, &vkScissor );
}

void VulkanCommandBuffer::clearColor( [[maybe_unused]] float32 red,
                                      [[maybe_unused]] float32 green,
                                      [[maybe_unused]] float32 blue,
                                      [[maybe_unused]] float32 alpha,
                                      [[maybe_unused]] uint32_t attachmentIndex ) {
}

void VulkanCommandBuffer::clearDepthStencil( [[maybe_unused]] float32 depth, [[maybe_unused]] uint8_t stencil ) {
}

void VulkanCommandBuffer::draw( PrimitiveTopology /*topology*/,
                                uint32_t firstVertex,
                                uint32_t vertexCount,
                                uint32_t firstInstance,
                                uint32_t instanceCount ) {
    PreDraw();
    vkCmdDraw( m_CommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance );
}

void VulkanCommandBuffer::drawIndexed( PrimitiveTopology /*topology*/,
                                       uint32_t indexCount,
                                       uint32_t instanceCount,
                                       uint32_t firstIndex,
                                       int32_t vertexOffset,
                                       uint32_t firstInstance ) {
    PreDraw();
    vkCmdDrawIndexed( m_CommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance );
}

void VulkanCommandBuffer::drawIndirect( const BufferHandle& bufferHandle,
                                        uint32_t drawCount,
                                        uint32_t offset,
                                        uint32_t stride ) {
    PreDraw();

    const VulkanBuffer& vkBuffer = bufferHandle.Buffer.as< VulkanBuffer >();

    VkDeviceSize vkOffset = offset;
    vkCmdDrawIndirect( m_CommandBuffer, vkBuffer.GetHandle(), vkOffset, drawCount, stride );
}

void VulkanCommandBuffer::drawIndirectCount( const BufferHandle& argumentBufferHandle,
                                             uint32_t argumentOffset,
                                             const BufferHandle& countBufferHandle,
                                             uint32_t countOffset,
                                             uint32_t maxDraws,
                                             uint32_t stride ) {
    PreDraw();

    const VulkanBuffer& vkArgumentBuffer = argumentBufferHandle.Buffer.as< VulkanBuffer >();
    const VulkanBuffer& vkCountBuffer = countBufferHandle.Buffer.as< VulkanBuffer >();

    vkCmdDrawIndirectCount( m_CommandBuffer,
                            vkArgumentBuffer.GetHandle(),
                            argumentOffset,
                            vkCountBuffer.GetHandle(),
                            countOffset,
                            maxDraws,
                            stride );
}

void VulkanCommandBuffer::drawIndexedIndirect( const BufferHandle& bufferHandle,
                                               uint32_t drawCount,
                                               uint32_t offset,
                                               uint32_t stride ) {
    PreDraw();

    const VulkanBuffer& vkBuffer = bufferHandle.Buffer.as< VulkanBuffer >();
    VkDeviceSize vkOffset = offset;

    vkCmdDrawIndexedIndirect( m_CommandBuffer, vkBuffer.GetHandle(), vkOffset, drawCount, stride );
}

void VulkanCommandBuffer::drawMeshTask( uint32_t groupX, uint32_t groupY, uint32_t groupZ ) {
    PreDraw();

    ONYX_ASSERT( g_vkCmdDrawMeshTasks != nullptr, "Mesh shader extension is not initialized." );
    g_vkCmdDrawMeshTasks( m_CommandBuffer, groupX, groupY, groupZ );
}

void VulkanCommandBuffer::drawMeshTaskIndirect( const BufferHandle& argumentBufferHandle,
                                                uint32_t argumentOffset,
                                                const BufferHandle& countBufferHandle,
                                                uint32_t countOffset,
                                                uint32_t maxDraws,
                                                uint32_t stride ) {
    PreDraw();

    ONYX_ASSERT( g_vkCmdDrawMeshTasksIndirectCount != nullptr, "Mesh shader extension is not initialized." );

    const VulkanBuffer& vkArgumentBuffer = argumentBufferHandle.Buffer.as< VulkanBuffer >();
    const VulkanBuffer& vkCountBuffer = countBufferHandle.Buffer.as< VulkanBuffer >();

    g_vkCmdDrawMeshTasksIndirectCount( m_CommandBuffer,
                                       vkArgumentBuffer.GetHandle(),
                                       argumentOffset,
                                       vkCountBuffer.GetHandle(),
                                       countOffset,
                                       maxDraws,
                                       stride );
}

void VulkanCommandBuffer::dispatch( uint32_t groupX, uint32_t groupY, uint32_t groupZ ) {
    PreDraw();

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    // BeginDebugLabel(m_CurrentShaderEffect->GetPipeline()->GetProperties().m_DebugName, Vector4f32{ 1.0f } );
#endif

    vkCmdDispatch( m_CommandBuffer, groupX, groupY, groupZ );

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    // EndDebugLabel();
#endif
}

void VulkanCommandBuffer::dispatchIndirect( const BufferHandle& bufferHandle ) {
    dispatchIndirect( bufferHandle, 0 );
}

void VulkanCommandBuffer::dispatchIndirect( const BufferHandle& bufferHandle, uint32_t offset ) {
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    // BeginDebugLabel(m_CurrentPipeline->GetProperties().m_DebugName, Vector4f32{ 1.0f });
#endif

    const VulkanBuffer& vkBuffer = bufferHandle.Buffer.as< VulkanBuffer >();
    vkCmdDispatchIndirect( m_CommandBuffer, vkBuffer.GetHandle(), bufferHandle.GetOffset() + offset );

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    // EndDebugLabel();
#endif
}

void VulkanCommandBuffer::copy( const BufferHandle& source, BufferHandle& destination ) {
    const VulkanBuffer& vkSourceBuffer = source.Buffer.as< VulkanBuffer >();
    VulkanBuffer& vkDestinationBuffer = destination.Buffer.as< VulkanBuffer >();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = source.Buffer->GetAliasOffset( source.Alias );
    copyRegion.size = source.Buffer->GetAliasSize( source.Alias );
    copyRegion.dstOffset = 0;
    vkCmdCopyBuffer( m_CommandBuffer, vkSourceBuffer.GetHandle(), vkDestinationBuffer.GetHandle(), 1, &copyRegion );
}

void VulkanCommandBuffer::globalBarrier( VkAccessFlagBits2 srcAccess, VkAccessFlagBits2 dstAccess ) {
    VkMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;
    barrier.srcAccessMask = srcAccess;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;
    barrier.dstAccessMask = dstAccess;
    barrier.pNext = nullptr;

    VkDependencyInfoKHR dependency_info{};
    dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
    dependency_info.memoryBarrierCount = 1;
    dependency_info.pMemoryBarriers = &barrier;
    dependency_info.pNext = nullptr;

    vkCmdPipelineBarrier2( m_CommandBuffer, &dependency_info );
}

void VulkanCommandBuffer::globalBarrier( VkAccessFlagBits2 srcAccess,
                                         VkPipelineStageFlags2 srcStage,
                                         VkAccessFlagBits2 dstAccess,
                                         VkPipelineStageFlags2 dstStage ) {
    VkMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
    barrier.srcStageMask = srcStage;
    barrier.srcAccessMask = srcAccess;
    barrier.dstStageMask = dstStage;
    barrier.dstAccessMask = dstAccess;
    barrier.pNext = nullptr;

    VkDependencyInfoKHR dependency_info{};
    dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
    dependency_info.memoryBarrierCount = 1;
    dependency_info.pMemoryBarriers = &barrier;
    dependency_info.pNext = nullptr;

    vkCmdPipelineBarrier2( m_CommandBuffer, &dependency_info );
}

void VulkanCommandBuffer::PreDraw() {
    m_CurrentShaderEffect->PreDraw( m_FrameIndex );
}

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
void VulkanCommandBuffer::beginDebugLabel( StringView label, const Vector4f32& color ) {
    if( g_vkCmdBeginDebugUtilsLabelExt != nullptr ) {
        VkDebugUtilsLabelEXT vkDebugLabel{};
        vkDebugLabel.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
        vkDebugLabel.pLabelName = label.data();
        vkDebugLabel.color[ 0 ] = color[ 0 ];
        vkDebugLabel.color[ 1 ] = color[ 1 ];
        vkDebugLabel.color[ 2 ] = color[ 2 ];
        vkDebugLabel.color[ 3 ] = color[ 3 ];
        vkDebugLabel.pNext = nullptr;
        g_vkCmdBeginDebugUtilsLabelExt( m_CommandBuffer, &vkDebugLabel );
    }
}

void VulkanCommandBuffer::endDebugLabel() {
    if( g_vkCmdEndDebugUtilsLabelExt != nullptr )
        g_vkCmdEndDebugUtilsLabelExt( m_CommandBuffer );
}

#endif
} // namespace onyx::rhi::vulkan
