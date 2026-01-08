#include <onyx/application/application.h>
#include <onyx/engine/enginesystemfactory.h>

#include <onyx/log/logger.h>
#include <onyx/log/backends/stdoutlogger.h>
#include <onyx/log/backends/visualstudiolog.h>
#include <onyx/application/log/logsinkfile.h>
#include <onyx/application/debug/gui/notificationloggersink.h>

#include <onyx/ui/imguisystem.h>
#include <onyx/filesystem/filedialog.h>
#include <onyx/filesystem/jsondeserializer.h>
#include <onyx/filesystem/onyxfile.h>

#include <onyx/rhi/graphicssystem.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>

#include <onyx/serialize/deserializer.h>

#include <onyx/platform/platformsystem.h>

namespace
{
    const char* const sl_CPU_Frame = "CPU";

    Onyx::Reference<Onyx::Graphics::RenderGraph> m_MainRenderGraph;
}

namespace Onyx::Application
{
    Application::Application() = default;
    Application::~Application() = default;

    void Application::Init()
    {
        Thread::MAIN_THREAD_ID = std::this_thread::get_id();

        m_Logger = MakeUnique<Logger>();
        Logger::s_DefaultLogger = m_Logger.get();

        auto typeId = Platform::PlatformSystem::TypeId;
        ONYX_UNUSED(typeId);

        auto workingDir = FileSystem::Path::GetWorkingDirectory().string();
        const FilePath appConfigPath = FileSystem::Path::GetWorkingDirectory() / "data/appconfig.oconf";
        ONYX_UNUSED(workingDir);
        
        FileSystem::OnyxFile appSettings(appConfigPath);
        FileSystem::JsonValue appConfigJson = appSettings.LoadJson();

        FileSystem::JsonDeserializer configDeserializer(appConfigJson.Json);

        HashMap<StringId32, FileSystem::MountPoint> mountPoints;
        configDeserializer.ReadForEach<"mountpoints">([&](const Deserializer& scopedDeserializer)
        {
            FileSystem::MountPoint mountPoint;
            if (scopedDeserializer.Read(mountPoint))
            {
                mountPoints[StringId32(mountPoint.Prefix)] = mountPoint;
                return true;
            }
            return false;
        });
        
        FileSystem::Path::SetMountPoints(mountPoints);

        FileSystem::FileDialog::Init();

        constexpr StringView lastSessionLogPath = "tmp:/logs/last_session.log";
        FilePath logDirectory = FileSystem::Path::GetFullPath(lastSessionLogPath).parent_path();
        if (FileSystem::Path::Exists(logDirectory) == false)
        {
            FileSystem::Path::CreateDirectory(logDirectory);
        }

        m_Logger->AddLoggingBackend<StdoutLogger>();

#if ONYX_IS_MSVC
        m_Logger->AddLoggingBackend<VisualStudioLogger>();
#endif

#if ONYX_UI_MODULE && ONYX_USE_IMGUI
        m_Logger->AddLoggingBackend<GuiNotificationLoggerSink>();
#endif

        m_Logger->AddLoggingBackend<LogSinkFile>(lastSessionLogPath);

        m_Logger->SetSeverity(LogLevel::Trace);
        m_Logger->Init();

        bool hasLoadedModules = configDeserializer.ReadForEach<"modules">([&](const Deserializer& scopedDeserializer)
        {
            StringId32 moduleId;
            if (scopedDeserializer.Read<"typeId">(moduleId) == false)
                return false;

            onyxU32 systemIndex = numeric_cast<onyxU32>(m_Modules.size());
            
            EngineSystemCreateContext context{ *this, scopedDeserializer };
            m_Modules.emplace_back(EngineSystemFactory::Create(moduleId, context));
            Optional<EngineSystemFactory::UpdateFunction> updateFunction = EngineSystemFactory::GetUpdate(moduleId);
            if (updateFunction.has_value())
            {
                m_UpdatableModules.emplace_back(systemIndex, *updateFunction);
            }

            return true;
        });

        if (hasLoadedModules == false)
        {
            ONYX_LOG_ERROR("Failed loading modules");
        }

        OnApplicationCreated(*this);

        Assets::AssetSystem& assetSystem = GetSystem<Assets::AssetSystem>();
        assetSystem.GetAsset("engine:/rendergraphs/default.orendergraph", m_MainRenderGraph);
    }

    void Application::Shutdown()
    {
        FileSystem::FileDialog::Shutdown();

        OnApplicationShutdown(*this);

        // Remove asset system first so we release all loaded resource references
        RemoveModule<Assets::AssetSystem>();

        // TODO: Fix
        //GetSystem<Graphics::WindowSystem>().GetMainWindow().RemoveOnCloseHandler(this, &Application::OnWindowClose);

        // init modules project
        for (UniquePtr<IEngineSystem>& engineModule : (m_Modules | std::views::reverse) )
        {
            engineModule.reset();
        }

        m_Logger->Shutdown();
    }

    void Application::Run()
    {
        ONYX_PROFILE_SET_THREAD(Main)

#if ONYX_USE_IMGUI
        bool hasImGuiSystem = HasSystem<Ui::ImGuiSystem>();
#endif

        Graphics::GraphicsSystem& graphicsSystem = GetSystem<Graphics::GraphicsSystem>();
        // TODO: Fix
        // Graphics::WindowSystem& windowSystem = GetSystem<Graphics::WindowSystem>();
       // windowSystem.GetMainWindow().AddOnCloseHandler(this, &Application::OnWindowClose);

        //onyxU64 lastFixedUpdateFrameTime = 0;
        onyxU64 lastFrameTime = Time::GetCurrentMilliseconds();

        while (m_IsRunning)
        {
            const onyxU64 currentFrameTime = Time::GetCurrentMilliseconds();
            const onyxU64 deltaFrameTime = currentFrameTime - lastFrameTime;
            const bool hasRenderGraph = m_MainRenderGraph.IsValid() && m_MainRenderGraph->IsLoaded();

            if (hasRenderGraph == false)
                continue;

            const bool hasBegunFrame = graphicsSystem.BeginFrame();


            if (hasBegunFrame == false)
                continue;


            Graphics::FrameContext& frameContext = graphicsSystem.GetFrameContext();
            //m_MainRenderGraph->OnSwapChainResized(graphicsSystem);
            m_MainRenderGraph->BeginFrame(frameContext);
            //ONYX_UNUSED(frameContext);
#if ONYX_USE_IMGUI
            if (hasImGuiSystem)
            {
                Ui::ImGuiSystem& imGuiSystem = GetSystem<Ui::ImGuiSystem>();
                imGuiSystem.OnBeginFrame(frameContext);
            }
            
#endif

            {
                ONYX_PROFILE_SECTION(UpdateModules)
                EngineSystemUpdateContext context{ *this, deltaFrameTime, 0 };
                for (const auto& updateInfo : m_UpdatableModules)
                {
                    updateInfo.UpdateFunctionPtr(*m_Modules[updateInfo.SystemIndex], context);
                }
            }

            if (hasBegunFrame)
            {
               graphicsSystem.Render();
               m_MainRenderGraph->Render(frameContext);

#if ONYX_USE_IMGUI
                if (hasImGuiSystem)
                {
                    Ui::ImGuiSystem& imGuiSystem = GetSystem<Ui::ImGuiSystem>();
                    imGuiSystem.OnEndFrame();
                }
#endif
                m_MainRenderGraph->EndFrame(frameContext);
                graphicsSystem.EndFrame();
            }

            lastFrameTime = currentFrameTime;
            //loc_FpsStatusBarItem->Update(deltaFrameTime);

            FrameMarkNamed(sl_CPU_Frame);
            FrameMark;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void Application::OnWindowClose()
    {
        m_IsRunning = false;
    }
}
