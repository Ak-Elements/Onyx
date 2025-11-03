#pragma once

#include <onyx/noncopyable.h>
#include <onyx/graphics/camera.h>
#include <onyx/graphics/framebuffercache.h>
#include <onyx/graphics/graphicssystem.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/renderpasscache.h>
#include <onyx/graphics/lighting/lighting.h>
#include <onyx/graphics/shader/psocache.h>
#include <onyx/graphics/shader/shadercache.h>

#include <onyx/graphics/presentthread.h> 


namespace Onyx::Graphics
{
    struct ShaderProperties;
}

namespace Onyx
{
    class Engine;

    namespace Graphics
    {
        namespace Vulkan
        {
            class CommandPool;
        }

        struct BufferProperties;
        class CommandBuffer;
        struct TextureProperties;
        struct TextureStorageProperties;
        class Texture;
        class TextureStorage;
        class Framebuffer;
        struct FramebufferSettings;
        class RenderPass;
        struct RenderPassSettings;
        class ShaderCache;
        enum class Context : onyxU8;
        class Window;
        class RenderContext;

        enum class ApiType : onyxU8
        {
            None,
            Dx12,
            Vulkan
        };

        static constexpr onyxU8 BINDLESS_SET = 0;
        static constexpr onyxU32 MAX_BINDLESS_RESOURCES = 1024;

        struct FrameData { virtual ~FrameData() = default; };

        struct ViewConstants
        {
            Matrix4<onyxF32> ProjectionMatrix;
            Matrix4<onyxF32> InverseProjectionMatrix;
            Matrix4<onyxF32> ViewMatrix;
            Matrix4<onyxF32> InverseViewMatrix;
            Matrix4<onyxF32> ViewProjectionMatrix;
            Matrix4<onyxF32> InverseViewProjectionMatrix;

            Vector2f32 Viewport;
            onyxF32 Near;
            onyxF32 Far;

            Vector3f32 CameraPosition;
            onyxF32 Padding;

            Vector3f32 CameraDirection;
            onyxF32 Padding2;
        };

        struct LightBlock
        {
            Vector3f32 color;
            float intensity;

            Vector3f32 position;
            float innerAngle;

            Vector3f32 direction;
            float outerAngle;

            int type;
            int numShadowSamples;
            float radius;
            float pad;
        };

        struct LightClusterAABB
        {
            Vector4f32 Min;
            Vector4f32 Max;
        };

        struct ONYX_ALIGN(16) Lighting
        {
            Array<DirectionalLight, 64> DirectionalLights;
            Array<PointLight, 64> PointLights;
            Array<SpotLight, 64> SpotLights;

            onyxU32 DirectionalLightsCount = 0;
            onyxU32 PointLightsCount;
            onyxU32 SpotLightsCount;
            onyxU32 Padding;
        };

        struct FrameContext
        {
            //TODO: Remove and pass as & parameter to functions along with the context
            GraphicsApi* Api;

            ViewConstants ViewConstants;
            Lighting Lighting;

            onyxF32 TimeOfDay = 15.0f;
            onyxU32 TonemapFunctor = 5;

            onyxU64 ComputeFrame = 0;
            onyxU64 AbsoluteFrame = 0;
            onyxU8 FrameIndex = 0;

            UniquePtr<FrameData> FrameData;
        };

        class GraphicsApiImpl : public NonCopyable
        {
            friend class GraphicsApi;

        public:
            virtual ~GraphicsApiImpl() = default;

            virtual void Init(const GraphicSettings& settings, const Window& window) = 0;
            virtual void Shutdown() = 0;

            virtual bool BeginFrame(const FrameContext&) = 0;
            virtual bool EndFrame(const FrameContext&) = 0;
            
            virtual void OnWindowResize(onyxU32 width, onyxU32 height) = 0;

            virtual bool IsBindless() const = 0;

            virtual CommandBuffer& GetCommandBuffer(onyxU8 frameIndex) = 0;
            virtual CommandBuffer& GetCommandBuffer(onyxU8 frameIndex, bool shouldBegin) = 0;
            virtual CommandBuffer& GetComputeCommandBuffer(onyxU8 frameIndex) = 0;
            virtual CommandBuffer& GetComputeCommandBuffer(onyxU8 frameIndex, bool shouldBegin) = 0;
            virtual void SubmitInstantCommandBuffer(Context context, onyxU8 frameIndex, InplaceFunction<void(CommandBuffer&)>&& functor) = 0;

        private:
            virtual void WaitIdle() const = 0;

            virtual const TextureHandle& GetAcquiredSwapChainImage(onyxU8 i) const = 0;
            virtual onyxU32 GetAcquiredBackbufferIndex() const = 0;

            virtual TextureFormat GetSwapchainTextureFormat() const = 0;
            virtual const Vector2s32& GetSwapchainExtent() const = 0;

            virtual RenderPassHandle CreateRenderPass(const RenderPassSettings& settings) = 0;
            virtual FramebufferHandle CreateFramebuffer(const FramebufferSettings& settings) = 0;
            virtual PipelineHandle CreatePipeline(ShaderHandle& shader, const PipelineProperties& properties) = 0;
            virtual DynamicArray<DescriptorSetHandle> CreateDescriptorSet(const ShaderHandle& shader, StringView debugName) = 0;

            virtual void CreateTexture(TextureHandle& outTexture, const TextureStorageProperties& storageProperties, const TextureProperties& properties) = 0;
            virtual void CreateTexture(TextureHandle& outTexture, const TextureStorageProperties& storageProperties, const TextureProperties& properties, const Span<onyxU8>& initialData) = 0;
            virtual void CreateAlias(TextureHandle& outTexture, TextureStorageHandle& storageHandle, const TextureStorageProperties& aliasStorageProperties, const TextureProperties& aliasTextureProperties) = 0;
            
            virtual void CreateBuffer(BufferHandle& outBuffer, const BufferProperties& properties) = 0;
            virtual BufferHandle GetTransientBuffer(onyxU8 frameIndex, const BufferProperties& properties) = 0;
        };

        class GraphicsApi : public NonCopyable
        {
            friend class RenderPassCache;
            friend class FramebufferCache;
            friend class ShaderCache;
        public:
            // maybe non static? but what would be the use case for non static?

            GraphicsApi() = default;
            ~GraphicsApi() override;

            void Init(const GraphicSettings& graphicSettings, Assets::AssetSystem& assetSystem, Window& window);
            void Shutdown();

            bool BeginFrame();
            void Render();
            void EndFrame();

            ApiType GetApiType() const { return m_Settings.Api; }

            template <typename T>
            T& GetApi() { return *static_cast<T*>(m_GraphicsApi.get()); }
            template <typename T>
            const T& GetApi() const { return *static_cast<const T*>(m_GraphicsApi.get()); }

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
            const RenderGraph& GetRenderGraph() const { return *m_RenderGraph; }

            ShaderCache& GetShaderCache() { return m_ShaderCache; }

            RenderPassHandle GetOrCreateRenderPass(const RenderPassSettings& settings);
            FramebufferHandle GetOrCreateFramebuffer(const FramebufferSettings& settings);
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

            bool IsBindless() const { return m_GraphicsApi->IsBindless(); }
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

            PresentThread m_PresentThread { *this };

            const Camera* m_QueuedCamera = nullptr;
            const Camera* m_Camera = nullptr; // non owning pointer

            UniquePtr<GraphicsApiImpl> m_GraphicsApi;

            onyxU8 m_FrameIndex = 0;
            InplaceArray<FrameContext, MAX_FRAMES_IN_FLIGHT> m_FrameContext;

            TextureFormat m_DepthTextureFormat = TextureFormat::Invalid;
            Vector2s32 m_DepthTextureExtent;

            InplaceArray<TextureHandle, MAX_FRAMES_IN_FLIGHT> m_DepthImages;
            InplaceArray<BufferHandle, MAX_FRAMES_IN_FLIGHT> m_ViewConstantsUniformBuffers;

            ShaderCache m_ShaderCache { *this };
            PsoCache m_PsoCache;
            Reference<RenderGraph> m_RenderGraph;
            RenderPassCache m_RenderPassCache { *this };
            FramebufferCache m_FramebufferCache{ *this };

            HashMap<StringId32, BlendState> m_BlendStates;

            bool m_HasComputeWork = false;
            bool m_HasWindowResized = false;
        };
    }
}
