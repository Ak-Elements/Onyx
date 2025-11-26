#include <onyx/application/application.h>
#include <onyx/engine/enginesystemfactory.h>

#include <onyx/log/logger.h>
#include <onyx/log/backends/stdoutlogger.h>
#include <onyx/log/backends/visualstudiolog.h>
#include <onyx/application/log/logsinkfile.h>
#include <onyx/application/debug/gui/notificationloggersink.h>

#include <onyx/graphics/windowsystem.h>
#include <onyx/graphics/window.h>

#include <onyx/ui/imguisystem.h>
#include <onyx/application/taskgraph/taskgraph.h>
#include <onyx/filesystem/filedialog.h>
#include <onyx/filesystem/jsondeserializer.h>
#include <onyx/filesystem/onyxfile.h>

#include <onyx/graphics/graphicssystem.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/nodegraph/nodegraphmodule.h>
#include <onyx/profiler/profiler.h>

#include <onyx/serialize/deserializer.h>

namespace
{
    const char* const sl_CPU_Frame = "CPU";
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

        const FileSystem::Filepath appConfigPath = FileSystem::Path::GetWorkingDirectory() / "data/appconfig.oconf";

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
        FileSystem::Filepath logDirectory = FileSystem::Path::GetFullPath(lastSessionLogPath).parent_path();
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

        // init node graph module
        NodeGraph::Init();

        DynamicArray<StringId32> applicationModules;
        if (configDeserializer.Read<"modules">(applicationModules))
        {
            // create modules requested by project
            for (const StringId32& moduleId : applicationModules)
            {
                const IEngineModuleMeta& meta = EngineModuleFactory::GetMeta(moduleId);
                m_Modules.emplace_back(meta.Create(configDeserializer));
            }
        }

        // init modules project
        for (UniquePtr<IEngineSystem>& engineModule : m_Modules)
        {
            const IEngineModuleMeta& meta = EngineModuleFactory::GetMeta(engineModule->GetTypeId());
            if (meta.IsInitializable())
            {
                meta.Init(*this, *engineModule);
            }

            if (meta.IsUpdatable())
            {
                m_UpdatableModules.emplace_back(meta.BuildUpdateCall(*engineModule));
            }
        }

        OnApplicationCreated(*this);
    }

    void Application::Shutdown()
    {
        FileSystem::FileDialog::Shutdown();

        OnApplicationShutdown(*this);

        // Remove asset system first so we release all loaded resource references
        RemoveModule<Assets::AssetSystem>();

        GetSystem<Graphics::WindowSystem>().GetMainWindow().RemoveOnCloseHandler(this, &Application::OnWindowClose);

        // init modules project
        for (UniquePtr<IEngineSystem>& engineModule : (m_Modules | std::views::reverse) )
        {
            const IEngineModuleMeta& meta = EngineModuleFactory::GetMeta(engineModule->GetTypeId());
            if (meta.IsShutdownable())
            {
                meta.Shutdown(*this, *engineModule);
            }

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
        Graphics::WindowSystem& windowSystem = GetSystem<Graphics::WindowSystem>();
        windowSystem.GetMainWindow().AddOnCloseHandler(this, &Application::OnWindowClose);

        //onyxU64 lastFixedUpdateFrameTime = 0;
        onyxU64 lastFrameTime = Time::GetCurrentMilliseconds();

        while (m_IsRunning)
        {
            const onyxU64 currentFrameTime = Time::GetCurrentMilliseconds();
            const onyxU64 deltaFrameTime = currentFrameTime - lastFrameTime;
            const bool hasBegunFrame = graphicsSystem.BeginFrame();

            if (hasBegunFrame == false)
                continue;

            Graphics::FrameContext& frameContext = graphicsSystem.GetFrameContext();
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
                    updateModule(*this, deltaFrameTime);
                }
            }

            if (hasBegunFrame)
            {
                graphicsSystem.Render();

#if ONYX_USE_IMGUI
                if (hasImGuiSystem)
                {
                    Ui::ImGuiSystem& imGuiSystem = GetSystem<Ui::ImGuiSystem>();
                    imGuiSystem.OnEndFrame();
                }
#endif
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
