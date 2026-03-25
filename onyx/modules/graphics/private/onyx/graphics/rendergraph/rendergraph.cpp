#include <onyx/graphics/rendergraph/rendergraph.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/log/logger.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/framecontext.h>
#include <onyx/rhi/graphicshandles.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/graphicstypes.h>
#include <onyx/rhi/texturestorage.h>
#include <onyx/thread/threadpool/threadpool.h>

ONYX_PROFILE_CREATE_TAG( RenderGraph, 0x3ed694 );

namespace onyx::graphics {
void RenderGraph::Init( rhi::GraphicsSystem& graphicsSystem ) {
    ONYX_PROFILE( RenderGraph );
    ONYX_PROFILE_FUNCTION;

    // allocate all resources and descriptors?
    m_Graph.Compile();

    // Allocating resources
    DynamicArray< RenderGraphResourceId > allocations;
    DynamicArray< RenderGraphResourceId > deallocations;

    HashMap< RenderGraphResourceId, uint32_t > resourceRefCounts;

    DynamicArray< RenderGraphResourceId > freeList;

    // Create render, framebuffers & pso's
    const auto topologicalOrder = m_Graph.GetTopologicalOrder();
    for ( const LocalNodeId nodeId : topologicalOrder ) {
        IRenderGraphNode& graphNode = m_Graph.GetNode< IRenderGraphNode >( nodeId );
        const uint32_t inputPinCount = graphNode.GetInputPinCount();
        for ( uint32_t i = 0; i < inputPinCount; ++i ) {
            const node_graph::PinBase* inputPin = graphNode.GetInputPin( i );
            if ( inputPin->IsConnected() == false )
                continue;

            ++resourceRefCounts[ inputPin->GetLinkedPinGlobalId().get() ];
        }
    }

    for ( const LocalNodeId nodeId : topologicalOrder ) {
        const bool isLastNode = nodeId == topologicalOrder[ ( topologicalOrder.size() - 1 ) ];

        IRenderGraphNode& graphNode = m_Graph.GetNode< IRenderGraphNode >( nodeId );
        // remove resource cache
        graphNode.Init( graphicsSystem, m_ResourceCache );

        uint32_t outputPinCount = graphNode.GetOutputPinCount();
        for ( uint32_t i = 0; i < outputPinCount; ++i ) {
            const node_graph::PinBase* outputPin = graphNode.GetOutputPin( i );
            RenderGraphResource& output = m_ResourceCache[ outputPin->GetGlobalId().get() ];

            if ( outputPin->GetType() == static_cast< node_graph::PinTypeId >( TypeHash< rhi::BufferHandle >() ) )
                continue;

            // TODO: Improve handling of final texture Id as this is very error prone
            if ( isLastNode ) {
                m_FinalTextureId = outputPin->GetGlobalId().get();
            }

            RenderGraphTextureResourceInfo& textureInfo = std::get< RenderGraphTextureResourceInfo >(
                output.Properties );
            if ( textureInfo.Type == RenderGraphResourceType::Reference ) {
                RenderGraphTextureResourceInfo& linkedTextureInfo = std::get< RenderGraphTextureResourceInfo >(
                    m_ResourceCache[ 0xba68b0d91801004 ].Properties );
                textureInfo.Format = linkedTextureInfo.Format;
                textureInfo.Size = linkedTextureInfo.Size;
                textureInfo.HasSize = linkedTextureInfo.HasSize;
                continue;
            }

            if ( output.IsExternal ) {
                // external resources get patched in during the rendering
                continue;
            }

            if ( CreateAttachment( graphicsSystem, output, freeList ) == false ) {
                // TODO: Add info for node / which output resource etc.
                ONYX_LOG_WARNING( "Failed creating output attachment for graph resource." );
                continue;
            }
        }

        const uint32_t inputPinCount = graphNode.GetInputPinCount();
        for ( uint32_t i = 0; i < inputPinCount; ++i ) {
            const node_graph::PinBase* inputPin = graphNode.GetInputPin( i );
            // check for invalid ID
            if ( inputPin->IsConnected() == false )
                continue;

            Guid64 id = inputPin->GetLinkedPinGlobalId();
            --resourceRefCounts[ id.get() ];

            RenderGraphResource& input = m_ResourceCache[ id.get() ];
            if ( ( input.IsExternal == false ) && ( resourceRefCounts[ input.Info.Id ] == 0 ) ) {
                // deallocations.
                //  Track deallocations?
                // if ((input.Info.Type == RenderGraphResourceType::Attachment) || (input.Info.Type ==
                // RenderGraphResourceType::Texture))
                //{
                //     const TextureHandle textureHandle = std::get<TextureHandle>(input.Handle);
                //     ONYX_ASSERT(textureHandle, "Adding invalid graphics handle to free list.");
                //     freeList.push_back(input.Info.Id);
                //
                //    // TODO: Add debug info to nodes
                //    ONYX_LOG_INFO("Deallocated resource in node.");
                //}
            }
        }
    }

    // create renderpass and framebuffer
    for ( const LocalNodeId nodeId : topologicalOrder ) {
        IRenderGraphNode& graphNode = m_Graph.GetNode< IRenderGraphNode >( nodeId );
        graphNode.Compile( graphicsSystem, m_ResourceCache );
    }

    graphicsSystem.OnBeginFrame().Connect< &RenderGraph::OnBeginFrame >( this );
    graphicsSystem.OnRenderFrame().Connect< &RenderGraph::OnRenderFrame >( this );
    graphicsSystem.OnEndFrame().Connect< &RenderGraph::OnEndFrame >( this );

    m_IsInitialized = true;
}

void RenderGraph::Shutdown( rhi::GraphicsSystem& graphicsSystem ) {
    ONYX_PROFILE( RenderGraph );
    ONYX_PROFILE_FUNCTION;

    for ( const LocalNodeId nodeId : m_Graph.GetTopologicalOrder() ) {
        IRenderGraphNode& graphNode = m_Graph.GetNode< IRenderGraphNode >( nodeId );
        graphNode.Shutdown( graphicsSystem );
    }

    m_ResourceCache.clear();
    m_Graph.Clear();

    graphicsSystem.OnBeginFrame().Disconnect( this );
    graphicsSystem.OnRenderFrame().Disconnect( this );
    graphicsSystem.OnEndFrame().Disconnect( this );

    m_IsInitialized = false;
}

void RenderGraph::OnBeginFrame( const rhi::FrameContext& frameContext ) {
    ONYX_PROFILE( RenderGraph );
    ONYX_PROFILE_FUNCTION;

    const rhi::TextureHandle& swapchainTarget = frameContext.Api->GetAcquiredSwapChainImage();
    RenderGraphResource& swapchainResource = m_ResourceCache[ SWAPCHAIN_RESOURCE_ID ];
    swapchainResource.Handle = swapchainTarget;

    const rhi::TextureHandle& depthTarget = frameContext.Api->GetDepthImage();
    RenderGraphResource& depthResource = m_ResourceCache[ DEPTH_RESOURCE_ID ];
    depthResource.Handle = depthTarget;

    // TODO: move to OnResize
    RenderGraphTextureResourceInfo& resourceInfo = std::get< RenderGraphTextureResourceInfo >(
        swapchainResource.Properties );
    resourceInfo.Size = Vector3s32( frameContext.Api->GetSwapchainExtent(), 0 );

    RenderGraphContext graphContext{ frameContext, *this };
    for ( int8_t nodeId : m_Graph.GetTopologicalOrder() ) {
        IRenderGraphNode& node = m_Graph.GetNode< IRenderGraphNode >( nodeId );

        if ( node.IsEnabled() == false ) {
            continue;
        }

        node.BeginFrame( graphContext );
    }
}

void RenderGraph::OnRenderFrame( const rhi::FrameContext& context ) {
    ONYX_PROFILE( RenderGraph );
    ONYX_PROFILE_FUNCTION;

    // kickoff root tasks
    RenderGraphContext graphContext{ context, *this };
    rhi::CommandBuffer& commandBuffer = context.Api->GetCommandBuffer( context.FrameIndex, true );

    for ( int8_t nodeId : m_Graph.GetTopologicalOrder() ) {
        IRenderGraphNode& node = m_Graph.GetNode< IRenderGraphNode >( nodeId );

        if ( node.HasBegunFrame() == false ) {
            continue;
        }

        node.PreRender( graphContext, commandBuffer );
        node.Render( graphContext, commandBuffer );
        node.PostRender( graphContext, commandBuffer );
    }
}

void RenderGraph::OnEndFrame( const rhi::FrameContext& frameContext ) {
    ONYX_PROFILE( RenderGraph );
    ONYX_PROFILE_FUNCTION;

    // wait for tasks
    RenderGraphContext graphContext{ frameContext, *this };
    for ( int8_t nodeId : m_Graph.GetTopologicalOrder() ) {
        IRenderGraphNode& node = m_Graph.GetNode< IRenderGraphNode >( nodeId );

        if ( node.HasBegunFrame() == false ) {
            continue;
        }

        node.EndFrame( graphContext );
    }

    rhi::TextureHandle finalTexture = std::get< rhi::TextureHandle >( m_ResourceCache.at( m_FinalTextureId ).Handle );
    if ( finalTexture.IsValid() ) {
        auto& commandBuffer = frameContext.Api->GetCommandBuffer( frameContext.FrameIndex, true );
        commandBuffer.TransitionLayout( finalTexture,
                                        rhi::Context::Graphics,
                                        rhi::Access::ShaderRead,
                                        rhi::ImageLayout::ReadOptimal );
    }
}

bool RenderGraph::HasResource( RenderGraphResourceId id ) const {
    return m_ResourceCache.contains( id );
}

const RenderGraphResource& RenderGraph::GetResource( RenderGraphResourceId id ) const {
    ONYX_PROFILE( RenderGraph );
    ONYX_PROFILE_FUNCTION;

    // TODO: Add default missing texture
    ONYX_ASSERT( m_ResourceCache.contains( id ), "Invalid resource" );
    return m_ResourceCache.at( id );
}

void RenderGraph::OnSwapChainResized( rhi::GraphicsSystem& graphicsSystem ) {
    ONYX_PROFILE( RenderGraph );
    ONYX_PROFILE_FUNCTION;

    for ( LocalNodeId nodeId : m_Graph.GetTopologicalOrder() ) {
        IRenderGraphNode& node = m_Graph.GetNode< IRenderGraphNode >( nodeId );
        node.OnSwapChainResized( graphicsSystem, m_ResourceCache );
    }
}

RenderGraphResource& RenderGraph::GetResource( RenderGraphResourceId id ) {
    ONYX_PROFILE( RenderGraph );
    ONYX_PROFILE_FUNCTION;

    // TODO: Add default missing texture
    ONYX_ASSERT( m_ResourceCache.contains( id ), "Invalid resource" );
    return m_ResourceCache.at( id );
}

bool RenderGraph::CreateAttachment( rhi::GraphicsSystem& graphicsSystem,
                                    RenderGraphResource& resource,
                                    DynamicArray< RenderGraphResourceId >& freeList ) {
    ONYX_PROFILE( RenderGraph );
    ONYX_PROFILE_FUNCTION;

    const RenderGraphTextureResourceInfo& resourceInfo = std::get< RenderGraphTextureResourceInfo >(
        resource.Properties );

    const Vector2s32& swapChainExtent = graphicsSystem.GetSwapchainExtent();

    rhi::TextureStorageProperties storageProperties;
    storageProperties.m_Size = resourceInfo.HasSize ? resourceInfo.Size : Vector3s32( swapChainExtent, 1 );
    storageProperties.m_Format = resourceInfo.Format;
    storageProperties.m_IsFrameBuffer = true;
    storageProperties.m_IsTexture = true;
    storageProperties.m_GpuAccess = rhi::GPUAccess::Write;
#if ONYX_IS_DEBUG
    storageProperties.m_DebugName = resource.Info.Name + " Storage";
#endif
    //[Aaron] do we really want to create the view here?
    rhi::TextureProperties texProp;
    texProp.m_Format = resourceInfo.Format;
    texProp.m_AllowCubeMapLoads = false;
    texProp.m_MaxMipLevel = storageProperties.m_MaxMipLevel;
    texProp.m_ArraySize = storageProperties.m_ArraySize;
#if ONYX_IS_DEBUG
    texProp.m_DebugName = resource.Info.Name + " View";
#endif

    // first check if we have a resource in the free list that we can use
    for ( uint32_t freeIndex = 0; freeIndex < freeList.size(); ++freeIndex ) {
        RenderGraphResourceId id = freeList[ freeIndex ];
        RenderGraphResource& freeResource = m_ResourceCache[ id ];

        rhi::TextureHandle& freeTexture = std::get< rhi::TextureHandle >( freeResource.Handle );
        const rhi::TextureStorageProperties& freeTextureStorageProperties = freeTexture.Storage->GetProperties();

        // wrap in a function to check if its alias-able?
        if ( ( storageProperties.m_Size != freeTextureStorageProperties.m_Size ) ||
             ( storageProperties.m_Format != freeTextureStorageProperties.m_Format ) )
            continue;

        // Add logic for handling already aliased textures?

#if ONYX_IS_DEBUG
        texProp.m_DebugName = resource.Info.Name + " Alias | " + freeTextureStorageProperties.m_DebugName;
#endif
        graphicsSystem.CreateAlias( std::get< rhi::TextureHandle >( resource.Handle ),
                                    freeTexture.Storage,
                                    storageProperties,
                                    texProp );
        return true;
    }

    graphicsSystem.CreateTexture( std::get< rhi::TextureHandle >( resource.Handle ), storageProperties, texProp );
    return true;
}

// bool RenderGraph::CreateBuffer(GraphicsSystem& graphicsApi, RenderGraphNode& node, RenderGraphResource& resource)
//{
//     ONYX_UNUSED(graphicsApi);
//     ONYX_UNUSED(node);
//     ONYX_UNUSED(resource);
//     //const RenderGraphBufferResourceInfo& resourceInfo =
//     std::get<RenderGraphBufferResourceInfo>(resource.Properties);

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
} // namespace onyx::graphics
