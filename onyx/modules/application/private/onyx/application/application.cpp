#include <onyx/application/application.h>
#include <onyx/engine/enginesystemfactory.h>

#include <onyx/application/debug/gui/notificationloggersink.h>
#include <onyx/application/log/logsinkfile.h>
#include <onyx/log/backends/stdoutlogger.h>
#include <onyx/log/backends/visualstudiolog.h>
#include <onyx/log/logger.h>

#include <onyx/filesystem/filedialog.h>
#include <onyx/filesystem/jsondeserializer.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/ui/imguisystem.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/profiler/profiler.h>
#include <onyx/rhi/graphicssystem.h>

#include <onyx/serialize/deserializer.h>

#include <onyx/platform/platformsystem.h>

#include <onyx/application/debug/gui/keyboardoverlay.h>

namespace {
const char* const CpuFrame = "CPU";
}

namespace onyx::application {
Application::Application() = default;
Application::~Application() = default;

void Application::init() {
    Thread::s_mainThreadId = std::this_thread::get_id();

    m_logger = makeUnique< Logger >();
    Logger::s_defaultLogger = m_logger.get();

    const FilePath appConfigPath = file_system::Path::GetWorkingDirectory() / "data/appconfig.oconf";
    file_system::OnyxFile appSettings( appConfigPath );
    file_system::JsonValue appConfigJson = appSettings.LoadJson();

    file_system::JsonDeserializer configDeserializer( appConfigJson.Json );

    HashMap< StringId32, file_system::MountPoint > mountPoints;
    configDeserializer.readForEach< "mountpoints" >( [ & ]( const Deserializer& scopedDeserializer ) {
        file_system::MountPoint mountPoint;
        if ( scopedDeserializer.read( mountPoint ) ) {
            mountPoints[ StringId32( mountPoint.Prefix ) ] = mountPoint;
            return true;
        }
        return false;
    } );

    file_system::Path::SetMountPoints( mountPoints );

    file_system::FileDialog::Init();

    constexpr StringView LastSessionLogPath = "tmp:/logs/last_session.log";
    FilePath logDirectory = file_system::Path::GetFullPath( LastSessionLogPath ).parent_path();
    if ( file_system::Path::Exists( logDirectory ) == false ) {
        file_system::Path::CreateDirectory( logDirectory );
    }

    m_logger->addLoggingBackend< StdoutLogger >();

#if ONYX_IS_MSVC
    m_Logger->AddLoggingBackend< VisualStudioLogger >();
#endif

#if ONYX_UI_MODULE && ONYX_USE_IMGUI
    m_logger->addLoggingBackend< GuiNotificationLoggerSink >();
#endif

    m_logger->addLoggingBackend< LogSinkFile >( LastSessionLogPath );

    m_logger->setSeverity( LogLevel::Trace );
    m_logger->init();

    bool hasLoadedModules = configDeserializer.readForEach< "modules" >(
        [ & ]( const Deserializer& scopedDeserializer ) {
            StringId32 moduleId;
            if ( scopedDeserializer.read< "typeId" >( moduleId ) == false )
                return false;

            uint32_t systemIndex = numericCast< uint32_t >( m_modules.size() );

            EngineSystemCreateContext context{ *this, scopedDeserializer };
            m_modules.emplace_back( EngineSystemFactory::create( moduleId, context ) );
            Optional< EngineSystemFactory::UpdateFunction > updateFunction = EngineSystemFactory::getUpdate( moduleId );
            if ( updateFunction.has_value() ) {
                m_updatableModules.emplace_back( systemIndex, *updateFunction );
            }

            return true;
        } );

    if ( hasLoadedModules == false ) {
        ONYX_LOG_ERROR( "Failed loading modules" );
    }

    onApplicationCreated( *this );
}

void Application::shutdown() {
    file_system::FileDialog::Shutdown();

    onApplicationShutdown( *this );

    // Remove asset system first so we release all loaded resource references
    removeModule< assets::AssetSystem >();

    // init modules project
    for ( UniquePtr< IEngineSystem >& engineModule : ( m_modules | std::views::reverse ) ) {
        engineModule.reset();
    }

    m_logger->shutdown();
}

void Application::run() {
    ONYX_PROFILE_SET_THREAD( Main )

#if ONYX_USE_IMGUI
    bool hasImGuiSystem = hasSystem< ui::ImGuiSystem >();
#endif

    platform::PlatformSystem& platformSystem = GetSystem< platform::PlatformSystem >();
    platformSystem.onWindowDestroy< &Application::onWindowDestroy >( this );
    rhi::GraphicsSystem& graphicsSystem = GetSystem< rhi::GraphicsSystem >();

    uint64_t lastFrameTime = Time::GetCurrentMilliseconds();

    while ( m_isRunning ) {
        const uint64_t currentFrameTime = Time::GetCurrentMilliseconds();
        const DeltaGameTime deltaFrameTime = currentFrameTime - lastFrameTime;

        const bool hasBegunFrame = graphicsSystem.BeginFrame();

        if ( hasBegunFrame == false )
            continue;

        rhi::FrameContext& frameContext = graphicsSystem.GetFrameContext();
#if ONYX_USE_IMGUI
        if ( hasImGuiSystem ) {
            ui::ImGuiSystem& imGuiSystem = GetSystem< ui::ImGuiSystem >();
            imGuiSystem.OnBeginFrame( frameContext );
        }
#endif

        {
            ONYX_PROFILE_SECTION( UpdateModules )
            EngineSystemUpdateContext context{ *this, deltaFrameTime, 0 };
            for ( const auto& updateInfo : m_updatableModules ) {
                updateInfo.UpdateFunctionPtr( *m_modules[ updateInfo.SystemIndex ], context );
            }
        }

        if ( hasBegunFrame ) {
            graphicsSystem.Render();

#if ONYX_USE_IMGUI
            if ( hasImGuiSystem ) {
                ui::ImGuiSystem& imGuiSystem = GetSystem< ui::ImGuiSystem >();
                imGuiSystem.OnRenderFrame( frameContext );
                imGuiSystem.OnEndFrame( frameContext );
            }
#endif
            graphicsSystem.EndFrame();
        }

        // TODO: Call this similar to the update calls
        platformSystem.onEndFrame();

        lastFrameTime = currentFrameTime;
        // loc_FpsStatusBarItem->Update(deltaFrameTime);

        FrameMarkNamed( CpuFrame );
        FrameMark;
        std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
    }
}

void Application::onWindowDestroy( const platform::Window& window ) {
    // close the application if the main window closes
    const platform::PlatformSystem& platformSystem = GetSystem< platform::PlatformSystem >();
    m_isRunning = platformSystem.GetMainWindow().getId() != window.getId();
}

} // namespace onyx::application
