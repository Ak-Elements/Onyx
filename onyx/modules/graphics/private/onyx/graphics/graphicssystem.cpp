#include <onyx/graphics/graphicssystem.h>

#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/window.h>
#include <onyx/graphics/textureasset.h>
#include <onyx/graphics/font/sdffont.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>
#include <onyx/graphics/rendergraph/tasks/atmosphericskytask.h>
#include <onyx/graphics/rendergraph/tasks/debuglightclusterspass.h>
#include <onyx/graphics/rendergraph/tasks/multiplescatteringtask.h>
#include <onyx/graphics/rendergraph/tasks/scatteringtask.h>
#include <onyx/graphics/rendergraph/tasks/skyviewluttask.h>
#include <onyx/graphics/rendergraph/tasks/tonemappass.h>
#include <onyx/graphics/rendergraph/tasks/updatelightclusterstask.h>
#include <onyx/graphics/rendergraph/tasks/updateviewconstantstask.h>
#include <onyx/graphics/serialize/materialshadergraphserializer.h>
#include <onyx/graphics/serialize/rendergraphserializer.h>
#include <onyx/graphics/serialize/sdffontserializer.h>
#include <onyx/graphics/serialize/textureserializer.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>

#include <onyx/graphics/shadergraph/shadergraphnodefactory.h>

#include <onyx/graphics/rendergraph/rendergraph.h> // need this for texturehandle pin meta data - FIX
#include <onyx/graphics/serialize/shaderserializer.h>

#include <onyx/graphics/shadergraph/nodes/fragmentshaderoutnode.h>
#include <onyx/graphics/shadergraph/nodes/getworldnormal.h>
#include <onyx/graphics/shadergraph/nodes/getworldposition.h>
#include <onyx/graphics/shadergraph/nodes/math/arithmeticnodes.h>
#include <onyx/graphics/shadergraph/nodes/math/geometricnodes.h>
#include <onyx/graphics/shadergraph/nodes/math/vectornodes.h>
#include <onyx/graphics/shadergraph/nodes/sampletexturenode.h>
#include <onyx/graphics/shadergraph/nodes/math/simplexnoisenode.h>
#include <onyx/graphics/vulkan/shader.h>
#include <onyx/nodegraph/nodegraphtyperegistry.h>

namespace Onyx
{
    template <>
    struct Serialization<Graphics::GraphicSettings>
    {
        static bool Serialize(Serializer& serializer, const Graphics::GraphicSettings& settings)
        {
            StringView path;
            serializer.Write<"rendergraph">(path);
            serializer.Write<"api">(settings.Api);
            serializer.Write<"isbindless">(settings.IsBindless);
            serializer.Write<"isdynamicrendering">(settings.IsDynamicRenderingEnabled);

#if !ONYX_IS_RETAIL
            serializer.Write<"istimelinesamplingenabled">(settings.IsTimeSamplingEnabled);
            serializer.Write<"isdebugenabled">(settings.IsDebugEnabled);
            serializer.Write<"isshaderdebugenabled">(settings.IsShaderDebugEnabled);
#endif
            return true;

        }
        static bool Deserialize(const Deserializer& deserializer, Graphics::GraphicSettings& outSettings)
        {
            StringView path;
            if (deserializer.Read<"rendergraph">(path))
            {
                outSettings.DefaultRenderGraph = Assets::AssetId(FileSystem::Filepath(path));
            }

            deserializer.Read<"api">(outSettings.Api);

            deserializer.ReadOptional<"isbindless">(outSettings.IsBindless);
            deserializer.ReadOptional<"isdynamicrendering">(outSettings.IsDynamicRenderingEnabled);

#if !ONYX_IS_RETAIL
            deserializer.ReadOptional<"istimelinesamplingenabled">(outSettings.IsTimeSamplingEnabled);
            deserializer.ReadOptional<"isdebugenabled">(outSettings.IsDebugEnabled);
            deserializer.ReadOptional<"isshaderdebugenabled">(outSettings.IsShaderDebugEnabled);
#endif
            return true;
        }
    };

    template <>
    struct Serialization<Graphics::WindowSettings>
    {
        static bool Serialize(Serializer& serializer, const Graphics::WindowSettings& settings)
        {
            serializer.Write<"size">(settings.Size);
            serializer.Write<"mode">(settings.Mode);
            return true;

        }
        static bool Deserialize(const Deserializer& deserializer, Graphics::WindowSettings& outSettings)
        {
            deserializer.Read<"size">(outSettings.Size);
            deserializer.Read<"mode">(outSettings.Mode);
            return true;
        }
    };

    bool Serialization<Graphics::GraphicsSystem>::Serialize(Serializer& serializer, const Graphics::GraphicsSystem& system)
    {
        return serializer.Write<"graphics">(system.GetGraphicsApi().GetSettings()) &&
            serializer.Write<"window">(system.GetWindow().GetSettings());
    }

    bool Serialization<Graphics::GraphicsSystem>::Deserialize(const Deserializer& deserializer, Graphics::GraphicsSystem& outSystem)
    {
        Graphics::WindowSettings& windowSettings = outSystem.GetWindow().GetSettings();
        return deserializer.Read<"graphics">(outSystem.GetGraphicsApi().GetSettings()) &&
            deserializer.Read<"window">(windowSettings) &&
            deserializer.Read<"name">(windowSettings.Title);
    }
}

namespace Onyx::Graphics
{
    GraphicsSystem::GraphicsSystem()
        : m_Window(MakeUnique<Window>())
        , m_GraphicsApi(MakeUnique<GraphicsApi>())
    {
        RenderGraphNodeFactory::RegisterNode<GetViewConstantsNode>();
        RenderGraphNodeFactory::RegisterNode<CreateLightClusters>();
        RenderGraphNodeFactory::RegisterNode<UpdateLightClustersRenderGraphNode>();
        RenderGraphNodeFactory::RegisterNode<CreateTransmittanceRenderGraphNode>();
        RenderGraphNodeFactory::RegisterNode<ComputeMultipleScatteringRenderGraphNode>();
        RenderGraphNodeFactory::RegisterNode<SkyViewLutRenderGraphNode>();
        RenderGraphNodeFactory::RegisterNode<AtmosphericSkyRenderGraphNode>();
        RenderGraphNodeFactory::RegisterNode<ToneMapPass>();
        RenderGraphNodeFactory::RegisterNode<DebugLightClustersRenderPass>();

        NodeGraph::NodeGraphTypeRegistry::RegisterType<TextureHandle>();
        NodeGraph::NodeGraphTypeRegistry::RegisterType<BufferHandle, "Onyx::Graphics::BufferHandle">();

        NodeGraph::RegisterArithmeticNodes<ShaderGraphNodeFactory, "Onyx::Graphics::ShaderGraph">();
        NodeGraph::RegisterGeometricNodes<ShaderGraphNodeFactory, "Onyx::Graphics::ShaderGraph">();
        NodeGraph::RegisterVectorNodes<ShaderGraphNodeFactory, "Onyx::Graphics::ShaderGraph">();

        ShaderGraphNodeFactory::RegisterNode<FragmentShaderOutNode>();
        ShaderGraphNodeFactory::RegisterNode<PBRMaterialShaderOutNode>();

        ShaderGraphNodeFactory::RegisterNode<SampleTextureNode>();

        ShaderGraphNodeFactory::RegisterNode<GetWorldPositionNode>();
        ShaderGraphNodeFactory::RegisterNode<GetWorldNormalNode>();

        ShaderGraphNodeFactory::RegisterNode<SimplexNoise2DShaderGraphNode>();
        ShaderGraphNodeFactory::RegisterNode<SimplexNoise3DShaderGraphNode>();
    }

    GraphicsSystem::~GraphicsSystem() = default;

    void GraphicsSystem::Init(Assets::AssetSystem& assetSystem)
    {
        auto shaderFactory = [&]() -> Reference<Shader>
            {
                switch (m_GraphicsApi->GetApiType())
                {
                case ApiType::Vulkan:
                    return Reference<Vulkan::Shader>::Create();
                case ApiType::Dx12:
                case ApiType::None:
                    return nullptr;
                }

                return nullptr;
            };

        Assets::AssetSystem::Register<TextureAsset, TextureSerializer>(assetSystem, *m_GraphicsApi);
        Assets::AssetSystem::Register<MaterialShaderGraph, MaterialShaderGraphSerializer>(assetSystem, *m_GraphicsApi);
        Assets::AssetSystem::Register<RenderGraph, RenderGraphSerializer>(assetSystem, *m_GraphicsApi);
        Assets::AssetSystem::Register<Shader, ShaderSerializer>(shaderFactory, assetSystem, *m_GraphicsApi);
        Assets::AssetSystem::Register<SDFFont, SDFFontSerializer>(assetSystem);

        m_Window->Create();
        m_GraphicsApi->Init(assetSystem, *m_Window);

        m_Window->SetIcon("engine:/onyx128x128.png");
        m_Window->Show();
    }

    void GraphicsSystem::Shutdown()
    {
        m_GraphicsApi->Shutdown();
        m_Window->Destroy();
    }
}
