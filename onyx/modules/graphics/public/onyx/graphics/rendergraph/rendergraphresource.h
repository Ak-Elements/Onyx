#pragma once 

#include <onyx/rhi/graphicshandles.h>
#include <onyx/rhi/bufferproperties.h>
#include <onyx/rhi/graphicstypes.h>
#include <onyx/rhi/framecontext.h>
#include <onyx/rhi/renderpass.h>

namespace onyx::graphics
{
    class RenderGraph;
    
    struct RenderGraphContext
    {
        const rhi::FrameContext& FrameContext;
        RenderGraph& Graph;

        template <typename T>
        T GetPinData(onyxU32 /*localPinId*/)
        {
            return T();
            //return Graph.GetResource(localPinId);
        }

    };

    using RenderGraphResourceId = onyxU64;

    constexpr RenderGraphResourceId INVALID_RESOURCE_ID = 0;
    constexpr RenderGraphResourceId SWAPCHAIN_RESOURCE_ID = hash::FNV1aHash<onyxU32>("swapchain");
    constexpr RenderGraphResourceId DEPTH_RESOURCE_ID = hash::FNV1aHash<onyxU32>("depth");
    constexpr RenderGraphResourceId VIEW_CONSTANTS_RESOURCE_ID = hash::FNV1aHash<onyxU32>("u_viewconstants");

    enum class RenderGraphResourceType
    {
        Invalid,
        Buffer,
        Texture,
        Attachment,
        Reference,
    };

    struct RenderGraphTextureResourceInfo
    {
        RenderGraphResourceType Type;

        Vector3s32 Size;
        rhi::TextureFormat Format = rhi::TextureFormat::BGRA_UNORM8;

        rhi::RenderPassSettings::LoadOp LoadOp = rhi::RenderPassSettings::LoadOp::DontCare;
        Vector4u8 ClearColor;

        bool IsExternal;
        bool HasSize;
    };

    struct RenderGraphBufferResourceInfo
    {
        rhi::BufferProperties BufferProperties;
    };

    struct RenderGraphResourceInfo
    {
        RenderGraphResourceType Type = RenderGraphResourceType::Invalid;
        RenderGraphResourceId Id = INVALID_RESOURCE_ID;

//#if ONYX_IS_DEBUG TODO: should be a hash in Release
        String Name;
//#endif
    };

    struct RenderGraphTexture
    {
        Vector3s32 Size;
        rhi::TextureFormat Format;

        rhi::RenderPassSettings::LoadOp LoadOp;
        Vector4u8 ClearColor;
    };

    struct RenderGraphBuffer
    {
        rhi::BufferProperties BufferProperties;
    };

    struct RenderGraphResource
    {
        RenderGraphResourceInfo Info;

        bool IsExternal = false;

        Variant<rhi::TextureHandle, rhi::BufferHandle> Handle;
        Variant<RenderGraphTextureResourceInfo, RenderGraphBufferResourceInfo> Properties;

        bool DrawPinInPropertyGrid(StringView name, RenderGraphResource& value)
        {
            ONYX_UNUSED(name);
            ONYX_UNUSED(value);
            return true;
        }

        constexpr bool operator==(RenderGraphResource& other) { return (Info.Id == other.Info.Id) /*&& (Handle == other.Handle)*/; }
        constexpr bool operator!=(RenderGraphResource& other) { return !(*this == other); }
    };
}
