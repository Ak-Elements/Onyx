#pragma once
#include <onyx/engine/enginesystem.h>

#include <onyx/graphics/framebuffercache.h>
#include <onyx/graphics/graphicshandles.h>
#include <onyx/graphics/presentthread.h>
#include <onyx/graphics/renderpasscache.h>
#include <onyx/graphics/shader/psocache.h>
#include <onyx/graphics/shader/shadercache.h>

#include <onyx/graphics/framecontext.h>
#include <onyx/graphics/graphicsettings.h>

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::Graphics
{
    enum class ApiType : onyxU8;
    enum class Context : onyxU8;

    struct BufferProperties;
    struct FramebufferSettings;
    struct RenderPassSettings;
    struct TextureProperties;
    struct TextureStorageProperties;
    struct ViewConstants;

    class Camera;
    class CommandBuffer;
    class Framebuffer;
    class GraphicsApiInterface;
    class RenderContext;
    class RenderPass;
    class RenderGraph;
    class ShaderCache;
    class Texture;
    class TextureStorage;
    class Window;
    class WindowSystem;

    class GraphicsSystem : public IEngineSystem
    {
        friend class RenderPassCache;
        friend class FramebufferCache;
        friend class ShaderCache;
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::GraphicsSystem";
        StringId32 GetTypeId() const override { return TypeId; }

        GraphicsSystem();
        ~GraphicsSystem() override;

        void Init(Assets::AssetSystem& assetSystem, WindowSystem& window);
        void Shutdown();

        bool BeginFrame();
        void Render();
        void EndFrame();

        ApiType GetApiType() const { return m_Settings.Api; }
        GraphicSettings& GetSettings() { return m_Settings; }
        const GraphicSettings& GetSettings() const { return m_Settings; }

        template <typename T>
        T& GetApi() { return *static_cast<T*>(m_GraphicsSystem.get()); }
        template <typename T>
        const T& GetApi() const { return *static_cast<const T*>(m_GraphicsSystem.get()); }

        const Window& GetWindow() const { ONYX_ASSERT(m_Window != nullptr); return *m_Window; }

        onyxU16 GetRefreshRate() const;

        void SetRenderGraph(Reference<RenderGraph>& renderGraph);

        void SetCamera(const Camera& camera) { m_QueuedCamera = &camera; }
        void ResetCamera() { m_QueuedCamera = nullptr; }

        const TextureHandle& GetAcquiredSwapChainImage() const;
        TextureFormat GetSwapchainTextureFormat() const;
        const Vector2s32& GetSwapchainExtent() const;

        const TextureHandle& GetDepthImage() const;
        TextureFormat GetDepthTextureFormat() const { return m_DepthTextureFormat; }
        const Vector2s32& GetDepthTextureExtent() const { return m_DepthTextureExtent; }

        //TODO: remove non const ref getter
        Reference<RenderGraph>& GetRenderGraph() { return m_RenderGraph; }
        const RenderGraph& GetRenderGraph() const;

        ShaderCache& GetShaderCache() { return m_ShaderCache; }

        RenderPassHandle GetOrCreateRenderPass(const RenderPassSettings& settings);
        FramebufferHandle GetOrCreateFramebuffer(const FramebufferSettings& settings);
        ShaderInstanceHandle CreateShaderInstance(Assets::AssetId shaderAssetId);
        ShaderInstanceHandle CreateShaderInstance(Assets::AssetId shaderAssetId, const PipelineProperties& properties);

        void CreateTexture(TextureHandle& outTexture, const TextureStorageProperties& storageProperties, const TextureProperties& properties);
        void CreateTexture(TextureHandle& outTexture, const TextureStorageProperties& storageProperties, const TextureProperties& properties, const Span<onyxU8>& initialData);
        void CreateAlias(TextureHandle& outTexture, TextureStorageHandle& storageHandle, const TextureStorageProperties& aliasStorageProperties, const TextureProperties& aliasTextureProperties);

        void CreateBuffer(BufferHandle& outBuffer, const BufferProperties& properties);
        BufferHandle GetTransientBuffer(const BufferProperties& properties);

        DynamicArray<DescriptorSetHandle> CreateDescriptorSet(const ShaderHandle& shader, StringView debugName) const;

        onyxU8 GetFrameIndex() const { return m_FrameIndex; }
        FrameContext& GetFrameContext() { return m_FrameContext[m_FrameIndex]; }
        const FrameContext& GetFrameContext() const { return m_FrameContext[m_FrameIndex]; }
        const ViewConstants& GetViewContsants() const { return m_FrameContext[m_FrameIndex].ViewConstants; }
        const BufferHandle& GetViewConstantsBuffer() const { return m_ViewConstantsUniformBuffers[m_FrameIndex]; }

        CommandBuffer& GetCommandBuffer(onyxU8 frameIndex);
        CommandBuffer& GetCommandBuffer(onyxU8 frameIndex, bool shouldBegin);
        CommandBuffer& GetComputeCommandBuffer(onyxU8 frameIndex);
        CommandBuffer& GetComputeCommandBuffer(onyxU8 frameIndex, bool shouldBegin);

        void SubmitInstantCommandBuffer(Context context, InplaceFunction<void(CommandBuffer&)>&& functor);
        const BlendState& GetDefaultBlendState() const;

        bool IsBindless() const;
#if !ONYX_IS_RETAIL
        bool IsShaderDebugEnabled() const { return m_Settings.IsShaderDebugEnabled; }
#endif
        void WaitIdle();

        void OnWindowResize(onyxU32 width, onyxU32 height);
        void CreatePipeline(const PipelineProperties& properties);

    protected:
        void LoadSettings();

    private:
        void OnRenderGraphLoaded(Reference<RenderGraph>& loadedGraph);
        void OnShaderLoaded(Assets::AssetSystem* assetSystem, Reference<Shader>& loadedGraph);
        void CreateDepthImages();
        void CreateViewConstantBuffers();

        RenderPassHandle CreateRenderPass(const RenderPassSettings& settings);
        FramebufferHandle CreateFramebuffer(const FramebufferSettings& settings);

    private:
        std::mutex m_Mutex;
        Assets::AssetSystem* m_AssetSystem = nullptr;
        Window* m_Window = nullptr;

        GraphicSettings m_Settings;

        PresentThread m_PresentThread{ *this };

        const Camera* m_QueuedCamera = nullptr;
        const Camera* m_Camera = nullptr; // non owning pointer

        UniquePtr<GraphicsApiInterface> m_GraphicsSystem;

        onyxU8 m_FrameIndex = 0;
        InplaceArray<FrameContext, MAX_FRAMES_IN_FLIGHT> m_FrameContext;

        TextureFormat m_DepthTextureFormat = TextureFormat::Invalid;
        Vector2s32 m_DepthTextureExtent;

        InplaceArray<TextureHandle, MAX_FRAMES_IN_FLIGHT> m_DepthImages;
        InplaceArray<BufferHandle, MAX_FRAMES_IN_FLIGHT> m_ViewConstantsUniformBuffers;

        ShaderCache m_ShaderCache{ *this };
        PsoCache m_PsoCache;
        Reference<RenderGraph> m_RenderGraph;
        RenderPassCache m_RenderPassCache{ *this };
        FramebufferCache m_FramebufferCache{ *this };

        HashMap<StringId32, BlendState> m_BlendStates;

        bool m_HasComputeWork = false;
        bool m_HasWindowResized = false;

    };
}

namespace Onyx
{
    template <>
    struct Serialization<Graphics::GraphicsSystem>
    {
        static bool Serialize(Serializer& serializer, const Graphics::GraphicsSystem& settings);
        static bool Deserialize(const Deserializer& deserializer, Graphics::GraphicsSystem& outSettings);
    };

    template <>
    struct Serialization<Graphics::GraphicSettings>
    {
        static bool Serialize(Serializer& serializer, const Graphics::GraphicSettings& settings);
        static bool Deserialize(const Deserializer& deserializer, Graphics::GraphicSettings& outSettings);
    };
}
