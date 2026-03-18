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

#include <onyx/application/debug/gui/keyboardoverlay.h>

namespace
{
    const char* const sl_CPU_Frame = "CPU";
}

namespace onyx::application
{
    Application::Application() = default;
    Application::~Application() = default;

    void Application::Init()
    {
        Thread::MAIN_THREAD_ID = std::this_thread::get_id();

        m_Logger = MakeUnique<Logger>();
        Logger::s_defaultLogger = m_Logger.get();

        const FilePath appConfigPath = file_system::Path::GetWorkingDirectory() / "data/appconfig.oconf";
        file_system::OnyxFile appSettings(appConfigPath);
        file_system::JsonValue appConfigJson = appSettings.LoadJson();

        file_system::JsonDeserializer configDeserializer(appConfigJson.Json);

        HashMap<StringId32, file_system::MountPoint> mountPoints;
        configDeserializer.ReadForEach<"mountpoints">([&](const Deserializer& scopedDeserializer)
        {
            file_system::MountPoint mountPoint;
            if (scopedDeserializer.Read(mountPoint))
            {
                mountPoints[StringId32(mountPoint.Prefix)] = mountPoint;
                return true;
            }
            return false;
        });

        file_system::Path::SetMountPoints(mountPoints);

        file_system::FileDialog::Init();

        constexpr StringView lastSessionLogPath = "tmp:/logs/last_session.log";
        FilePath logDirectory = file_system::Path::GetFullPath(lastSessionLogPath).parent_path();
        if (file_system::Path::Exists(logDirectory) == false)
        {
            file_system::Path::CreateDirectory(logDirectory);
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
    }

    void Application::Shutdown()
    {
        file_system::FileDialog::Shutdown();

        OnApplicationShutdown(*this);

        // Remove asset system first so we release all loaded resource references
        RemoveModule<assets::AssetSystem>();

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
        bool hasImGuiSystem = HasSystem<ui::ImGuiSystem>();
#endif

        rhi::GraphicsSystem& graphicsSystem = GetSystem<rhi::GraphicsSystem>();
        // TODO: Fix
        // Graphics::WindowSystem& windowSystem = GetSystem<Graphics::WindowSystem>();
       // windowSystem.GetMainWindow().AddOnCloseHandler(this, &Application::OnWindowClose);

        //onyxU64 lastFixedUpdateFrameTime = 0;
        onyxU64 lastFrameTime = Time::GetCurrentMilliseconds();

        while (m_IsRunning)
        {
            const onyxU64 currentFrameTime = Time::GetCurrentMilliseconds();
            const onyxU64 deltaFrameTime = currentFrameTime - lastFrameTime;

            const bool hasBegunFrame = graphicsSystem.BeginFrame();

            if (hasBegunFrame == false)
                continue;

            rhi::FrameContext& frameContext = graphicsSystem.GetFrameContext();
#if ONYX_USE_IMGUI
            if (hasImGuiSystem)
            {
                ui::ImGuiSystem& imGuiSystem = GetSystem<ui::ImGuiSystem>();
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

#if ONYX_USE_IMGUI
                if (hasImGuiSystem)
                {
                    ui::ImGuiSystem& imGuiSystem = GetSystem<ui::ImGuiSystem>();
                    imGuiSystem.OnRenderFrame(frameContext);
                    imGuiSystem.OnEndFrame(frameContext);
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
