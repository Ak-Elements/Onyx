#pragma once
#include <onyx/engine/enginesystem.h>

#include <onyx/rhi/framebuffercache.h>
#include <onyx/rhi/graphicshandles.h>
#include <onyx/rhi/presentthread.h>
#include <onyx/rhi/renderpasscache.h>
#include <onyx/rhi/shader/psocache.h>
#include <onyx/rhi/shader/shadercache.h>

#include <onyx/rhi/framecontext.h>
#include <onyx/rhi/graphicsettings.h>

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::Platform
{
    class PlatformSystem;
}

namespace Onyx::Graphics
{
    class ShaderGraphNode;
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

    class GraphicsSystem : public IEngineSystem
    {
        friend class RenderPassCache;
        friend class FramebufferCache;
        friend class ShaderCache;

        using BeginFrameSignalT = Signal<void(const FrameContext&)>;
        using RenderFrameSignalT = Signal<void(const FrameContext&)>;
        using EndFrameSignalT = Signal<void(const FrameContext&)>;

    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::GraphicsSystem";
        StringId32 GetTypeId() const override { return TypeId; }

        GraphicsSystem(const GraphicSettings& settings, Assets::AssetSystem& assetSystem, Platform::PlatformSystem& platformSystem);
        ~GraphicsSystem() override;

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

        onyxU16 GetRefreshRate() const;

        void SetCamera(const Camera& camera) { m_QueuedCamera = &camera; }
        void ResetCamera() { m_QueuedCamera = nullptr; }

        const TextureHandle& GetAcquiredSwapChainImage() const;
        TextureFormat GetSwapchainTextureFormat() const;
        const Vector2s32& GetSwapchainExtent() const;

        const TextureHandle& GetDepthImage() const;
        TextureFormat GetDepthTextureFormat() const { return m_DepthTextureFormat; }
        const Vector2s32& GetDepthTextureExtent() const { return m_DepthTextureExtent; }

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

        Sink<BeginFrameSignalT> OnBeginFrame() { return Sink{ m_BeginFrameSignal }; }
        Sink<BeginFrameSignalT> OnRenderFrame() { return Sink{ m_RenderFrameSignal }; }
        Sink<BeginFrameSignalT> OnEndFrame() { return Sink{ m_EndFrameSignal }; }

    protected:
        void LoadSettings();

    private:
        void CreateDepthImages(Vector2s32 extents);
        void CreateViewConstantBuffers();

        RenderPassHandle CreateRenderPass(const RenderPassSettings& settings);
        FramebufferHandle CreateFramebuffer(const FramebufferSettings& settings);

    private:
        std::mutex m_Mutex;
        Assets::AssetSystem* m_AssetSystem = nullptr;
        Platform::PlatformSystem* m_PlatformSystem = nullptr;

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
        RenderPassCache m_RenderPassCache{ *this };
        FramebufferCache m_FramebufferCache{ *this };

        HashMap<StringId32, BlendState> m_BlendStates;

        BeginFrameSignalT m_BeginFrameSignal;
        RenderFrameSignalT m_RenderFrameSignal;
        EndFrameSignalT m_EndFrameSignal;

        bool m_HasComputeWork = false;
        bool m_HasWindowResized = false;
    };
}

namespace Onyx
{
    template <>
    struct Serialization<Graphics::GraphicSettings>
    {
        static bool Serialize(Serializer& serializer, const Graphics::GraphicSettings& settings);
        static bool Deserialize(const Deserializer& deserializer, Graphics::GraphicSettings& outSettings);
    };
}
