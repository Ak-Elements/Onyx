#include <onyx/application/application.h>

#include <onyx/log/logger.h>
#include <onyx/log/backends/stdoutlogger.h>
#include <onyx/log/backends/visualstudiolog.h>

#include <onyx/application/debug/gui/notificationloggersink.h>

#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/window.h>

#include <onyx/ui/imguisystem.h>
#include <onyx/application/taskgraph/taskgraph.h>
#include <onyx/filesystem/filedialog.h>

#include <onyx/graphics/graphicssystem.h>
#include <onyx/profiler/profiler.h>

namespace
{
    const char* const sl_CPU_Frame = "CPU";
}

namespace Onyx::Application
{
    namespace
    {
        bool DeserializeApplicationSettings(ApplicationSettings& settings, const FileSystem::JsonValue& inJson)
        {
            inJson.Get("name", settings.Name);
            settings.WindowSettings.Title = settings.Name;

            //TODO: fix icon
            //appConfigJson.Get("icon", WindowSettings.m_Icon );

            FileSystem::Filepath inputMapPath;
            if (inJson.Get("inputmap", inputMapPath))
            {
                settings.InputConfig = Assets::AssetId(inputMapPath);
            }

            FileSystem::JsonValue dataRoots;
            if (inJson.Get("mountpoints", dataRoots))
            {
                for (const auto& jsonDataRoot : dataRoots.Json)
                {
                    String dataRootName = jsonDataRoot["name"];
                    FileSystem::Filepath dataRootPath = jsonDataRoot["path"];

                    if (dataRootName.ends_with(":/") == false)
                    {
                        dataRootName += ":/";
                    }

                    settings.MountPoints[Hash::FNV1aHash32(dataRootName)] = { .Prefix = dataRootName, .Path = dataRootPath.make_preferred() };
                }
            }

            FileSystem::JsonValue graphicSettings;
            if (inJson.Get("graphics", graphicSettings))
            {
                FileSystem::Filepath renderGraphPath = "engine:/rendergraphs/default.orendergraph";
                graphicSettings.Get("rendergraph", renderGraphPath);
                settings.GraphicSettings.DefaultRenderGraph = Assets::AssetId(renderGraphPath);

                graphicSettings.Get("api", settings.GraphicSettings.Api);
            }

            FileSystem::JsonValue windowSettings;
            if (inJson.Get("window", windowSettings))
            {
                windowSettings.Get("size", settings.WindowSettings.Size);
                windowSettings.Get("mode", settings.WindowSettings.Mode);
            }

            return true;
        }
    }
    ApplicationSettings::ApplicationSettings()
    {
        FileSystem::OnyxFile appSettings(FileSystem::Path::GetWorkingDirectory() / "data/appconfig.oconf");
        FileSystem::JsonValue appConfigJson = appSettings.LoadJson();

        DeserializeApplicationSettings(*this, appConfigJson);
    }

    Application::Application(const ApplicationSettings& settings)
        : m_Settings(settings)
    {
        Thread::MAIN_THREAD_ID = std::this_thread::get_id();

        m_Logger = MakeUnique<Logger>();
        Logger::s_DefaultLogger = m_Logger.get();

        m_Logger->AddLoggingBackend<StdoutLogger>();
#if ONYX_IS_VISUAL_STUDIO
        m_Logger->AddLoggingBackend<VisualStudioLogger>();
#endif

#if ONYX_UI_MODULE && ONYX_USE_IMGUI
        m_Logger->AddLoggingBackend<GuiNotificationLoggerSink>();
#endif

        m_Logger->SetSeverity(LogLevel::Trace);
        m_Logger->Init();

        FileSystem::Path::SetMountPoints(settings.MountPoints);
        FileSystem::FileDialog::Init();

        OnApplicationCreated(*this);
    }

    Application::~Application() = default;

    void Application::Run()
    {
        ONYX_PROFILE_SET_THREAD(Main)

#if ONYX_USE_IMGUI
        Ui::ImGuiSystem& imGuiSystem = GetSystem<Ui::ImGuiSystem>();
#endif

        Graphics::GraphicsSystem& graphicsSystem = GetSystem<Graphics::GraphicsSystem>();
        Graphics::GraphicsApi& graphicsApi = graphicsSystem.GetGraphicsApi();
        graphicsSystem.GetWindow().AddOnCloseHandler(this, &Application::OnWindowClose);

        //onyxU64 lastFixedUpdateFrameTime = 0;
        onyxU64 lastFrameTime = Time::GetCurrentMilliseconds();

        while (m_IsRunning)
        {
            const onyxU64 currentFrameTime = Time::GetCurrentMilliseconds();
            const onyxU64 deltaFrameTime = currentFrameTime - lastFrameTime;
           
            const bool hasBegunFrame = graphicsApi.BeginFrame();

            if (hasBegunFrame == false)
                continue;

            Graphics::FrameContext& frameContext = graphicsApi.GetFrameContext();
            ONYX_UNUSED(frameContext);
#if ONYX_USE_IMGUI
            imGuiSystem.OnBeginFrame(frameContext);
#endif

            {
                ONYX_PROFILE_SECTION(UpdateModules)
                for (const auto& updateModule : m_UpdatableModules)
                {
                    updateModule(deltaFrameTime);
                }
            }

            if (hasBegunFrame)
            {
                graphicsApi.Render();

#if ONYX_USE_IMGUI
                imGuiSystem.OnEndFrame();
#endif
                graphicsApi.EndFrame();
            }

            lastFrameTime = currentFrameTime;
            //loc_FpsStatusBarItem->Update(deltaFrameTime);

            FrameMarkNamed(sl_CPU_Frame);
            FrameMark;
            std::this_thread::yield();
        }
    }

    void Application::Shutdown()
    {
        FileSystem::FileDialog::Shutdown();

        OnApplicationShutdown(*this);

        GetSystem<Graphics::GraphicsSystem>().GetWindow().RemoveOnCloseHandler(this, &Application::OnWindowClose);

        // Shutdown functors are in 'random' order, we should shutdown in the opposite order of creation
        for (const auto& shutdownFunctor : (m_ShutdownFunctors | std::views::values))
            shutdownFunctor();

        m_Logger->Shutdown();
    }

    void Application::OnWindowClose()
    {
        m_IsRunning = false;
    }
}
