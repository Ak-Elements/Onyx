#include <onyx/graphics/graphicssystem.h>

#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/window.h>
#include <onyx/graphics/textureasset.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>
#include <onyx/graphics/rendergraph/tasks/atmosphericskytask.h>
#include <onyx/graphics/rendergraph/tasks/multiplescatteringtask.h>
#include <onyx/graphics/rendergraph/tasks/scatteringtask.h>
#include <onyx/graphics/rendergraph/tasks/skyviewluttask.h>
#include <onyx/graphics/rendergraph/tasks/tonemappass.h>
#include <onyx/graphics/rendergraph/tasks/updatelightclusterstask.h>
#include <onyx/graphics/rendergraph/tasks/updateviewconstantstask.h>
#include <onyx/graphics/serialize/materialshadergraphserializer.h>
#include <onyx/graphics/serialize/rendergraphserializer.h>
#include <onyx/graphics/serialize/textureserializer.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>

#include <onyx/nodegraph/shadernodefactory.h>

#include <onyx/graphics/rendergraph/rendergraph.h> // need this for texturehandle pin meta data - FIX

#include <onyx/graphics/shadergraph/nodes/fragmentshaderoutnode.h>
#include <onyx/graphics/shadergraph/nodes/getworldnormal.h>
#include <onyx/graphics/shadergraph/nodes/getworldposition.h>
#include <onyx/graphics/shadergraph/nodes/math/arithmeticnodes.h>
#include <onyx/graphics/shadergraph/nodes/math/geometricnodes.h>
#include <onyx/graphics/shadergraph/nodes/math/vectornodes.h>
#include <onyx/graphics/shadergraph/nodes/sampletexturenode.h>

namespace Onyx::NodeGraph
{
    NodeRegistry<ShaderNodeMetaData> TypedNodeFactory<Graphics::ShaderGraphNode, ShaderNodeMetaData>::ms_NodeRegistry;
}


namespace Onyx::Graphics
{
    GraphicsSystem::GraphicsSystem() = default;
    GraphicsSystem::~GraphicsSystem() = default;

    void GraphicsSystem::Init(const GraphicSettings& graphicSettings, const WindowSettings& windowSettings, Assets::AssetSystem& assetSystem)
    {
        RenderGraphNodeFactory::RegisterNode<GetViewConstantsNode>("Scene/Get View Constants");
        RenderGraphNodeFactory::RegisterNode<CreateLightClusters>("Lighting/Create Light Clusters");
        RenderGraphNodeFactory::RegisterNode<UpdateLightClustersRenderGraphNode>("Lighting/Update Light Pass");
        RenderGraphNodeFactory::RegisterNode<CreateTransmittanceRenderGraphNode>("Scattering/Create Transmittance");
        RenderGraphNodeFactory::RegisterNode<ComputeMultipleScatteringRenderGraphNode>("Scattering/Compute MultipleScattering");
        RenderGraphNodeFactory::RegisterNode<SkyViewLutRenderGraphNode>("Scattering/Compute SkyViewLut");
        RenderGraphNodeFactory::RegisterNode<AtmosphericSkyRenderGraphNode>("Scattering/Atmospheric Sky");
        RenderGraphNodeFactory::RegisterNode<ToneMapPass>("PostFx/Tonemap");

        m_Window = MakeUnique<Window>();
        m_Window->Create(windowSettings);

        m_GraphicsApi = MakeUnique<GraphicsApi>(*m_Window);
        m_GraphicsApi->SetApiType(graphicSettings.Api);

        Assets::AssetSystem::Register<TextureAsset, TextureSerializer>(assetSystem, *m_GraphicsApi);
        Assets::AssetSystem::Register<MaterialShaderGraph, MaterialShaderGraphSerializer>(assetSystem, *m_GraphicsApi);
        Assets::AssetSystem::Register<RenderGraph, RenderGraphSerializer>(assetSystem, *m_GraphicsApi);

        NodeGraph::RegisterArithmeticNodes<NodeGraph::ShaderNodeFactory>();
        NodeGraph::RegisterGeometricNodes<NodeGraph::ShaderNodeFactory>();
        NodeGraph::RegisterVectorNodes<NodeGraph::ShaderNodeFactory>();

        NodeGraph::ShaderNodeFactory::RegisterNode<FragmentShaderOutNode>("Shader/Fragment/Shader Out");
        NodeGraph::ShaderNodeFactory::RegisterNode<PBRMaterialShaderOutNode>("Shader/Fragment/PBR Material");

        NodeGraph::ShaderNodeFactory::RegisterNode<SampleTextureNode>("Shader/Texture/Sample Texture");

        NodeGraph::ShaderNodeFactory::RegisterNode<GetWorldPositionNode>("Shader/Fragment/Get World Position");
        NodeGraph::ShaderNodeFactory::RegisterNode<GetWorldNormalNode>("Shader/Fragment/Get World Normal");
    }

    void GraphicsSystem::Shutdown()
    {
        m_GraphicsApi->Shutdown();
        m_Window->Destroy();
    }

}
