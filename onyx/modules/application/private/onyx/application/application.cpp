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
#include <onyx/filesystem/jsondeserializer.h>
#include <onyx/filesystem/onyxfile.h>

#include <onyx/graphics/graphicssystem.h>
#include <onyx/profiler/profiler.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace
{
    const char* const sl_CPU_Frame = "CPU";
}

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
}

namespace Onyx::Application
{
    namespace
    {
        bool DeserializeApplicationSettings(ApplicationSettings& settings, const Deserializer& deserializer)
        {
            deserializer.Read<"name">(settings.Name);
            settings.WindowSettings.Title = settings.Name;

            //TODO: fix icon
            //appConfigJson.Get("icon", WindowSettings.m_Icon );

            StringView inputMapPath;
            if (deserializer.Read<"inputmap">(inputMapPath))
            {
                settings.InputConfig = Assets::AssetId(FileSystem::Filepath(inputMapPath));
            }

            deserializer.ReadForEach<"mountpoints">([&](const Deserializer& scopedDeserializer)
            {
                    String dataRootName;
                    scopedDeserializer.Read<"name">(dataRootName);

                    StringView dataRootPath;
                    scopedDeserializer.Read<"path">(dataRootPath);

                    if (dataRootName.ends_with(":/") == false)
                    {
                        dataRootName += ":/";
                    }

                    settings.MountPoints[StringId32(dataRootName)] = { .Prefix = dataRootName, .Path = FileSystem::Filepath(dataRootPath).make_preferred() };

                    return true;
            });

            deserializer.Read<"graphics">(settings.GraphicSettings);
            deserializer.Read<"window">(settings.WindowSettings);

            return true;
        }
    }
    ApplicationSettings::ApplicationSettings()
    {
        FileSystem::OnyxFile appSettings(FileSystem::Path::GetWorkingDirectory() / "data/appconfig.oconf");
        FileSystem::JsonValue appConfigJson = appSettings.LoadJson();

        FileSystem::JsonDeserializer deserializer(appConfigJson.Json);
        DeserializeApplicationSettings(*this, deserializer);
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
        bool hasImGuiSystem = HasSystem<Ui::ImGuiSystem>();
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
            if (hasImGuiSystem)
            {
                Ui::ImGuiSystem& imGuiSystem = GetSystem<Ui::ImGuiSystem>();
                imGuiSystem.OnBeginFrame(frameContext);
            }
            
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
                if (hasImGuiSystem)
                {
                    Ui::ImGuiSystem& imGuiSystem = GetSystem<Ui::ImGuiSystem>();
                    imGuiSystem.OnEndFrame();
                }
#endif
                graphicsApi.EndFrame();
            }

            lastFrameTime = currentFrameTime;
            //loc_FpsStatusBarItem->Update(deltaFrameTime);

            FrameMarkNamed(sl_CPU_Frame);
            FrameMark;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
