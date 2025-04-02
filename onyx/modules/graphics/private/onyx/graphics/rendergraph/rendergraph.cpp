#include <onyx/graphics/rendergraph/rendergraph.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/graphics/texture.h>
#include <onyx/graphics/texturestorage.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>
#include <onyx/graphics/vulkan/texturestorage.h>
#include <onyx/log/logger.h>
#include <onyx/thread/threadpool/threadpool.h>

#include <onyx/graphics/vulkan/commandbuffer.h>

ONYX_PROFILE_CREATE_TAG(RenderGraph, 0x3ed694);

namespace Onyx::Graphics
{
    void RenderGraph::Init(GraphicsApi& graphicsApi)
    {
        ONYX_PROFILE(RenderGraph);
        ONYX_PROFILE_FUNCTION;

        // allocate all resources and descriptors?
        Graph.Compile();
        
        // Allocating resources
        DynamicArray<RenderGraphResourceId> allocations;
        DynamicArray<RenderGraphResourceId> deallocations;

        HashMap<RenderGraphResourceId, onyxU32> resourceRefCounts;

        DynamicArray<RenderGraphResourceId> freeList;

        // Create render, framebuffers & pso's
        const auto topologicalOrder = Graph.GetTopologicalOrder();
        for (const LocalNodeId nodeId : topologicalOrder)
        {
            IRenderGraphNode& graphNode = Graph.GetNode<IRenderGraphNode>(nodeId);
            const onyxU32 inputPinCount = graphNode.GetInputPinCount();
            for (onyxU32 i = 0; i < inputPinCount; ++i)
            {
                const NodeGraph::PinBase* inputPin = graphNode.GetInputPin(i);
                if (inputPin->IsConnected() == false)
                    continue;

                ++resourceRefCounts[inputPin->GetLinkedPinGlobalId()];
            }
        }

        for (const LocalNodeId nodeId : topologicalOrder)
        {
            const bool isLastNode = nodeId == topologicalOrder[(topologicalOrder.size() - 2)];

            IRenderGraphNode& graphNode = Graph.GetNode<IRenderGraphNode>(nodeId);
            // remove resource cache
            graphNode.Init(graphicsApi, ResourceCache);

            onyxU32 outputPinCount = graphNode.GetOutputPinCount();
            for (onyxU32 i = 0; i < outputPinCount; ++i)
            {
                const NodeGraph::PinBase* outputPin = graphNode.GetOutputPin(i);
                RenderGraphResource& output = ResourceCache[outputPin->GetGlobalId()];

                if (outputPin->GetType() == static_cast<NodeGraph::PinTypeId>(TypeHash<BufferHandle>()))
                    continue;

                if (output.IsExternal)
                {
                    // external resources get patched in during the rendering
                    continue;
                }

                if (CreateAttachment(graphicsApi, output, freeList) == false)
                {
                    // TODO: Add info for node / which output resource etc.
                    ONYX_LOG_WARNING("Failed creating output attachment for graph resource.");
                    continue;
                }

                if (isLastNode)
                {
                    FinalTextureId = outputPin->GetGlobalId();
                }
            }

            const onyxU32 inputPinCount = graphNode.GetInputPinCount();
            for (onyxU32 i = 0; i < inputPinCount; ++i)
            {
                const NodeGraph::PinBase* inputPin = graphNode.GetInputPin(i);
                // check for invalid ID
                if (inputPin->IsConnected() == false)
                    continue;

                Guid64 id = inputPin->GetLinkedPinGlobalId();
                --resourceRefCounts[id];

                RenderGraphResource& input = ResourceCache[id];
                if ((input.IsExternal == false) && (resourceRefCounts[input.Info.Id] == 0))
                {
                    //deallocations.
                    // Track deallocations?
                    if ((input.Info.Type == RenderGraphResourceType::Attachment) || (input.Info.Type == RenderGraphResourceType::Texture))
                    {
                        const TextureHandle textureHandle = std::get<TextureHandle>(input.Handle);
                        ONYX_ASSERT(textureHandle, "Adding invalid graphics handle to free list.");
                        freeList.push_back(input.Info.Id);

                        // TODO: Add debug info to nodes
                        ONYX_LOG_INFO("Deallocated resource in node.");
                    }

                }
            }
        }

        // create renderpass and framebuffer
        for (const LocalNodeId nodeId : topologicalOrder)
        {
            IRenderGraphNode& graphNode = Graph.GetNode<IRenderGraphNode>(nodeId);
            graphNode.Compile(graphicsApi, ResourceCache);
        }

    }

    void RenderGraph::Shutdown(GraphicsApi& graphicsApi)
    {
        ONYX_PROFILE(RenderGraph);
        ONYX_PROFILE_FUNCTION;


        for (const LocalNodeId nodeId : Graph.GetTopologicalOrder())
        {
            IRenderGraphNode& graphNode = Graph.GetNode<IRenderGraphNode>(nodeId);
            graphNode.Shutdown(graphicsApi);
        }

        ResourceCache.clear();
        Graph.Clear();
    }

    void RenderGraph::BeginFrame(const FrameContext& frameContext)
    {
        ONYX_PROFILE(RenderGraph);
        ONYX_PROFILE_FUNCTION;

        const TextureHandle& swapchainTarget = frameContext.Api->GetAcquiredSwapChainImage();
        RenderGraphResource& swapchainResource = ResourceCache[SWAPCHAIN_RESOURCE_ID];
        swapchainResource.Handle = swapchainTarget;

        const TextureHandle& depthTarget = frameContext.Api->GetDepthImage();
        RenderGraphResource& depthResource = ResourceCache[DEPTH_RESOURCE_ID];
        depthResource.Handle = depthTarget;

        // TODO: move to OnResize
        RenderGraphTextureResourceInfo& resourceInfo = std::get<RenderGraphTextureResourceInfo>(swapchainResource.Properties);
        resourceInfo.Size = Vector3s32(frameContext.Api->GetSwapchainExtent(), 0);

        RenderGraphContext graphContext{ frameContext, *this };

        for (onyxS8 nodeId : Graph.GetTopologicalOrder())
        {
            IRenderGraphNode& node = Graph.GetNode<IRenderGraphNode>(nodeId);

            if (node.IsEnabled() == false)
            {
                continue;
            }

            node.BeginFrame(graphContext);
        }
    }

    void RenderGraph::Render(const FrameContext& context)
    {
        ONYX_PROFILE(RenderGraph);
        ONYX_PROFILE_FUNCTION;

        // kickoff root tasks
        RenderGraphContext graphContext{ context, *this };
        CommandBuffer& commandBuffer = context.Api->GetCommandBuffer(context.FrameIndex, true);

        for (onyxS8 nodeId : Graph.GetTopologicalOrder())
        {
            IRenderGraphNode& node = Graph.GetNode<IRenderGraphNode>(nodeId);

            if (node.IsEnabled() == false)
            {
                continue;
            }

            node.PreRender(graphContext, commandBuffer);
            node.Render(graphContext, commandBuffer);
            node.PostRender(graphContext, commandBuffer);
        }
    }

    void RenderGraph::EndFrame(const FrameContext& frameContext)
    {
        ONYX_PROFILE(RenderGraph);
        ONYX_PROFILE_FUNCTION;

        // wait for tasks
        RenderGraphContext graphContext{ frameContext, *this };

        for (onyxS8 nodeId : Graph.GetTopologicalOrder())
        {
            IRenderGraphNode& node = Graph.GetNode<IRenderGraphNode>(nodeId);

            if (node.IsEnabled() == false)
            {
                continue;
            }

            node.EndFrame(graphContext);
        }

        // Transition image to present
        RenderGraphResource& swapchainResource = ResourceCache[SWAPCHAIN_RESOURCE_ID];
        TextureHandle& swapchainTarget = std::get<TextureHandle>(swapchainResource.Handle);
        Vulkan::VulkanTextureStorage& storage = swapchainTarget.Storage.As<Vulkan::VulkanTextureStorage>();

        CommandBuffer& commandBuffer = frameContext.Api->GetCommandBuffer(frameContext.FrameIndex, true);
        Vulkan::VulkanCommandBuffer& cmdBuffer = static_cast<Vulkan::VulkanCommandBuffer&>(commandBuffer);
        storage.TransitionLayout(cmdBuffer, Context::Graphics, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ACCESS_2_NONE, 0, 1);
    }

    const RenderGraphResource& RenderGraph::GetResource(RenderGraphResourceId id) const
    {
        ONYX_PROFILE(RenderGraph);
        ONYX_PROFILE_FUNCTION;

        // TODO: Add default missing texture
        ONYX_ASSERT(ResourceCache.contains(id), "Invalid resource");
        return ResourceCache.at(id);
    }

    void RenderGraph::OnSwapChainResized(GraphicsApi& graphicsApi)
    {
        ONYX_PROFILE(RenderGraph);
        ONYX_PROFILE_FUNCTION;

        for (LocalNodeId nodeId : Graph.GetTopologicalOrder())
        {
            IRenderGraphNode& node = Graph.GetNode<IRenderGraphNode>(nodeId);
            node.OnSwapChainResized(graphicsApi, ResourceCache);
        }
    }

    RenderGraphResource& RenderGraph::GetResource(RenderGraphResourceId id)
    {
        ONYX_PROFILE(RenderGraph);
        ONYX_PROFILE_FUNCTION;

        // TODO: Add default missing texture
        ONYX_ASSERT(ResourceCache.contains(id), "Invalid resource");
        return ResourceCache.at(id);
    }

    bool RenderGraph::CreateAttachment(GraphicsApi& graphicsApi, RenderGraphResource& resource, DynamicArray<RenderGraphResourceId>& freeList)
    {
        ONYX_PROFILE(RenderGraph);
        ONYX_PROFILE_FUNCTION;

        const RenderGraphTextureResourceInfo& resourceInfo = std::get<RenderGraphTextureResourceInfo>(resource.Properties);

        const Vector2s32& swapChainExtent = graphicsApi.GetSwapchainExtent();

        TextureStorageProperties storageProperties;
        storageProperties.m_Size = resourceInfo.HasSize ? resourceInfo.Size : Vector3s32(swapChainExtent, 1);
        storageProperties.m_Format = resourceInfo.Format;
        storageProperties.m_IsFrameBuffer = true;
        storageProperties.m_IsTexture = true;
        storageProperties.m_GpuAccess = GPUAccess::Write;
#if ONYX_IS_DEBUG
        storageProperties.m_DebugName = resource.Info.Name + " Storage";
#endif
        //[Aaron] do we really want to create the view here?
        TextureProperties texProp;
        texProp.m_Format = resourceInfo.Format;
        texProp.m_AllowCubeMapLoads = false;
        texProp.m_MaxMipLevel = storageProperties.m_MaxMipLevel;
        texProp.m_ArraySize = storageProperties.m_ArraySize;
#if ONYX_IS_DEBUG
        texProp.m_DebugName = resource.Info.Name + " View";
#endif

        // first check if we have a resource in the free list that we can use
        for (onyxU32 freeIndex = 0; freeIndex < freeList.size(); ++freeIndex)
        {
            RenderGraphResourceId id = freeList[freeIndex];
            RenderGraphResource& freeResource = ResourceCache[id];

            TextureHandle& freeTexture = std::get<TextureHandle>(freeResource.Handle);
            const TextureStorageProperties& freeTextureStorageProperties = freeTexture.Storage->GetProperties();

            // wrap in a function to check if its alias-able?
            if ((storageProperties.m_Size != freeTextureStorageProperties.m_Size) ||
                (storageProperties.m_Format != freeTextureStorageProperties.m_Format))
                continue;

            // Add logic for handling already aliased textures?

#if ONYX_IS_DEBUG
            texProp.m_DebugName = resource.Info.Name + " Alias | " + freeTextureStorageProperties.m_DebugName;
#endif
            graphicsApi.CreateAlias(std::get<TextureHandle>(resource.Handle), freeTexture.Storage, storageProperties, texProp);
            return true;
        }

        graphicsApi.CreateTexture(std::get<TextureHandle>(resource.Handle), storageProperties, texProp);
        return true;
    }

    //bool RenderGraph::CreateBuffer(GraphicsApi& graphicsApi, RenderGraphNode& node, RenderGraphResource& resource)
    //{
    //    ONYX_UNUSED(graphicsApi);
    //    ONYX_UNUSED(node);
    //    ONYX_UNUSED(resource);
    //    //const RenderGraphBufferResourceInfo& resourceInfo = std::get<RenderGraphBufferResourceInfo>(resource.Properties);

    //    //if (resourceInfo.IsDynamic)
    //    //{
    //    //    return node.CreateResource(Api, resource);
    //    //}
    //    //else
    //    //{
    //    //    // TODO: Add creation of buffer based on json data
    //    //    return false;
    //    //}

    //    return false;
    //}
}
