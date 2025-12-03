#include <onyx/application/application.h>

#include <editor/modules/editormodule.h>
#include <editor/rendertasks/compositetask.h>
#include <editor/rendertasks/gridtask.h>
#include <editor/rendertasks/guitask.h>

#include <onyx/application/rendertasks/depthprepassrendertask.h>
#include <onyx/application/rendertasks/staticmeshrendertask.h>
#include <onyx/application/rendertasks/textrendertask.h>
#include <onyx/filesystem/imagefile.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>

#include <onyx/profiler/profiler.h>

namespace Onyx::Application
{
    void OnApplicationCreate(ApplicationSettings& settings)
    {
        settings.Name = "Onyx-Editor";

        Graphics::WindowSettings windowSettings
        {
            settings.Name,
            {1600,900},
            Graphics::WindowMode::Windowed,
        };

        settings.WindowSettings = windowSettings;
        settings.DataDirectory = "irrlicht/data";
        settings.TempDirectory = "tmp";
        settings.DefaultRenderGraph = "rendergraphs/default.orendergraph";

        Graphics::RenderGraphNodeFactory::RegisterNode<DepthPrePassRenderGraphNode>("Scene/Depth Pre Pass");
        Graphics::RenderGraphNodeFactory::RegisterNode<UIRenderGraphNode>("UI/ImGui Pass");
        Graphics::RenderGraphNodeFactory::RegisterNode<CompositeRenderGraphNode>("Graphics/Composite");
        Graphics::RenderGraphNodeFactory::RegisterNode<GridRenderGraphNode>("Editor/Editor Scene Grid");
        Graphics::RenderGraphNodeFactory::RegisterNode<StaticMeshRenderGraphNode>("Graphics/Static Mesh Pass");
        Graphics::RenderGraphNodeFactory::RegisterNode<MSDFFontRenderPass>("Scene/3D MSDF Font Pass");
    }

    void OnApplicationCreated(Application& application)
    {
        application.AddModule<Editor::EditorModule>();
        //application.AddModule<Game::GameSystem>();
    }

    void OnApplicationShutdown(Application& /*application*/ )
    {
        
    }
}
