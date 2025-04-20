#pragma once

#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/graphicstypes.h>
#include <onyx/graphics/renderpass.h>
#include <onyx/graphics/buffer.h>
#include <onyx/graphics/graphicshandles.h>
#include <onyx/graphics/pipeline.h>
#include <onyx/graphics/texturestorage.h>
#include <onyx/graphics/shader/shadermodule.h>
#include <onyx/nodegraph/node.h>

namespace Onyx::NodeGraph
{
    template <>
    struct PinMetaObject<Graphics::BufferHandle>
    {
        static bool DrawPinInPropertyGrid(StringView name, Graphics::BufferHandle& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF5C5CCD; /* Indian Red */ }

        static bool Serialize(FileSystem::JsonValue& json, const Graphics::BufferHandle& handle)
        {
            ONYX_UNUSED(json);
            ONYX_UNUSED(handle);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Graphics::BufferHandle& handle)
        {
            ONYX_UNUSED(json);
            ONYX_UNUSED(handle);
            return true;
        }
    };

    template <>
    struct PinMetaObject<Graphics::TextureHandle>
    {
        static bool DrawPinInPropertyGrid(StringView name, Graphics::TextureHandle& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFB48246; /*Steel Blue*/ }

        static bool Serialize(FileSystem::JsonValue& json, const Graphics::TextureHandle& handle)
        {
            ONYX_UNUSED(json);
            ONYX_UNUSED(handle);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Graphics::TextureHandle& handle)
        {
            ONYX_UNUSED(json);
            ONYX_UNUSED(handle);
            return true;
        }
    };
}

namespace Onyx::Graphics
{
    class GraphicsApi;
    class CommandBuffer;
    class RenderGraph;
    struct FrameContext;

    struct RenderGraphContext
    {
        const FrameContext& FrameContext;
        RenderGraph& Graph;

        template <typename T>
        T GetPinData(onyxU32 localPinId)
        {
            return T();
            //return Graph.GetResource(localPinId);
        }

    };

    using RenderGraphResourceId = onyxU64;

    constexpr RenderGraphResourceId INVALID_RESOURCE_ID = 0;
    constexpr RenderGraphResourceId SWAPCHAIN_RESOURCE_ID = Hash::FNV1aHash32("swapchain");
    constexpr RenderGraphResourceId DEPTH_RESOURCE_ID = Hash::FNV1aHash32("depth");
    constexpr RenderGraphResourceId VIEW_CONSTANTS_RESOURCE_ID = Hash::FNV1aHash32("u_viewconstants");

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
        TextureFormat Format = TextureFormat::BGRA_UNORM8;

        RenderPassSettings::LoadOp LoadOp = RenderPassSettings::LoadOp::DontCare;
        Vector4u8 ClearColor;

        bool IsExternal;
        bool HasSize;
    };

    struct RenderGraphBufferResourceInfo
    {
        BufferProperties BufferProperties;
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
        TextureFormat Format;

        RenderPassSettings::LoadOp LoadOp;
        Vector4u8 ClearColor;
    };

    struct RenderGraphBuffer
    {
        BufferProperties BufferProperties;
    };

    struct RenderGraphResource
    {
        RenderGraphResourceInfo Info;

        bool IsExternal = false;

        Variant<TextureHandle, BufferHandle> Handle;
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

    // rename to render graph task
    struct RenderGraphPolicy
    {
        using RenderGraphResourceCache = HashMap<RenderGraphResourceId, RenderGraphResource>;
    public:
        virtual void Init(GraphicsApi& api, RenderGraphResourceCache& resourceCache) = 0;
        virtual void Shutdown(GraphicsApi& api) = 0;

        virtual void Compile(GraphicsApi& api, RenderGraphResourceCache& resourceCache) = 0;

        virtual void BeginFrame(const RenderGraphContext& context) = 0;

        virtual void PreRender(RenderGraphContext& context, CommandBuffer& commandBuffer) = 0;
        virtual void Render(RenderGraphContext& context, CommandBuffer& commandBuffer) = 0;
        virtual void PostRender(RenderGraphContext& context, CommandBuffer& commandBuffer) = 0;

        virtual void EndFrame(const RenderGraphContext& context) = 0;

        virtual bool IsEnabled() = 0;

        virtual void OnSwapChainResized(GraphicsApi& api, RenderGraphResourceCache& resourceCache) = 0;
    };

    class IRenderGraphNode : public NodeGraph::NodeWithPolicy<RenderGraphPolicy>
    {
    public:
        void Init(GraphicsApi& /*api*/, RenderGraphResourceCache& /*resourceCache*/) override { }
        void Shutdown(GraphicsApi& /*api*/) override { }
        void Compile(GraphicsApi& /*api*/, RenderGraphResourceCache& /*resourceCache*/) override { }
        void BeginFrame(const RenderGraphContext& /*context*/) override { }
        void PreRender(RenderGraphContext& /*context*/, CommandBuffer& /*commandBuffer*/) override { }
        void Render(RenderGraphContext& /*context*/, CommandBuffer& /*commandBuffer*/) override { }
        void PostRender(RenderGraphContext& /*context*/, CommandBuffer& /*commandBuffer*/) override { }
        void EndFrame(const RenderGraphContext& /*context*/) override { }

        bool IsEnabled() override { return false; }

        void OnSwapChainResized(GraphicsApi& /*api*/, RenderGraphResourceCache& /*resourceCache*/) override {}
    };

    class RenderGraphShaderNode : public IRenderGraphNode 
    {
    public:
        void Init(GraphicsApi& api, RenderGraphResourceCache& resourceCache) override;
        void Shutdown(GraphicsApi& api) final;

        void Compile(GraphicsApi& api, RenderGraphResourceCache& resourceCache) override;

        void BeginFrame(const RenderGraphContext& context) override;
        void PreRender(RenderGraphContext& context, CommandBuffer& commandBuffer) final;
        void Render(RenderGraphContext& context, CommandBuffer& commandBuffer) override;
        void PostRender(RenderGraphContext& context, CommandBuffer& commandBuffer) final;

        void EndFrame(const RenderGraphContext& context) final;

        bool OnSerialize(FileSystem::JsonValue& json) const override;
        bool OnDeserialize(const FileSystem::JsonValue& json) override;

        const RenderGraphTextureResourceInfo& GetInputResourceInfo(onyxU32 pinIndex) { return pinIndex >= m_InputAttachmentInfos.size() ? m_InputAttachmentInfos.emplace_back() : m_InputAttachmentInfos[pinIndex]; }
        const RenderGraphTextureResourceInfo& GetOuputResourceInfo(onyxU32 pinIndex) { return pinIndex >= m_OutputAttachmentInfos.size() ? m_OutputAttachmentInfos.emplace_back() : m_OutputAttachmentInfos[pinIndex]; }

        void OnSwapChainResized(GraphicsApi& /*api*/, RenderGraphResourceCache& /*resourceCache*/) override;

        virtual bool IsComputeTask() const { return false; }

    protected:
#if ONYX_IS_EDITOR
        bool OnDrawInPropertyGrid(HashMap<onyxU64, std::any>& constantPinData) override;
#endif

        void BindResources(ShaderEffectHandle& shader, const RenderGraphResourceCache& resourceCache, const FrameContext& frameContext);

    protected:
        virtual void OnInit(GraphicsApi&, RenderGraphResourceCache&) {}
        virtual void OnShutdown(GraphicsApi&) {}

        virtual void OnBeginFrame(const RenderGraphContext&) {}

        virtual void OnPreRender(RenderGraphContext&, CommandBuffer&) {}
        virtual void OnRender(RenderGraphContext&, CommandBuffer&) {}
        virtual void OnPostRender(RenderGraphContext&, CommandBuffer&) {}

        virtual void OnEndFrame(const RenderGraphContext&) {}

        void CreateRenderPass(GraphicsApi& api, RenderGraphResourceCache& resourceCache);
        void UpdateFramebuffer(GraphicsApi& api, RenderGraphResourceCache& resourceCache);

    protected:
        RenderPassHandle m_RenderPass;
        FramebufferHandle m_Framebuffer;

        DynamicArray<RenderGraphTextureResourceInfo> m_InputAttachmentInfos;

        DynamicArray<RenderGraphTextureResourceInfo> m_OutputAttachmentInfos;
        DynamicArray<RenderGraphBufferResourceInfo> m_OutputBufferInfos;
    };

    class RenderGraphFixedShaderNode : public RenderGraphShaderNode
    {
    public:
        void Init(GraphicsApi& api, RenderGraphResourceCache& resourceCache) final;

        void Compile(GraphicsApi& api, RenderGraphResourceCache& resourceCache) override;

        void BeginFrame(const RenderGraphContext& context) final;
        void Render(RenderGraphContext& context, CommandBuffer& commandBuffer) override;

        bool IsComputeTask() const override { ONYX_ASSERT(m_Shader.IsValid()); return m_Shader->IsComputeShader(); }
        bool IsEnabled() override { return m_ShaderEffect.IsValid(); }

        bool OnSerialize(FileSystem::JsonValue& json) const override;
        bool OnDeserialize(const FileSystem::JsonValue& json) override;

    private:
#if ONYX_IS_EDITOR
        bool OnDrawInPropertyGrid(HashMap<onyxU64, std::any>& constantPinData) override;
#endif
    protected:
        String& GetShaderPath() { return m_ShaderPath; }

        PipelineProperties m_PipelineProperties;

        ShaderEffectHandle m_ShaderEffect;

        String m_ShaderPath;
        ShaderHandle m_Shader; // <- Should be an AssetHandle, that would make it possible to register on OnLoad
    };
}
