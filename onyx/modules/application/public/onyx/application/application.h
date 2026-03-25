#pragma once
#include <onyx/engine/enginesystem.h>

#include <onyx/engine/enginesystemfactory.h>
#include <onyx/platform/platformfwd.h>

namespace onyx::application {
class TaskGraph;
class ApplicationModule;

class Application : public IEngine {
  public:
    Application();
    ~Application() override;

    Application( const Application& other ) = delete;
    Application& operator=( const Application& other ) = delete;

    Application( Application&& other ) noexcept = default;
    Application& operator=( Application&& other ) noexcept = default;

    void init();
    void shutdown();
    void run();

    ONYX_NO_DISCARD Logger* getLogger() const { return m_logger.get(); }

    template < typename T >
    requires std::is_base_of_v< IEngineSystem, T >
    void removeModule() {
        ONYX_ASSERT( HasSystem< T >(), "Module is not added." );

        auto it = std::ranges::find_if( m_modules, [ = ]( const UniquePtr< IEngineSystem >& module ) {
            return module->getTypeId() == T::TypeId;
        } );
        // TODO: Remove all modules that depend on it
        m_modules.erase( it );
    }

    template < typename T >
    requires std::is_base_of_v< IEngineSystem, T >
    ONYX_NO_DISCARD bool HasSystem() const {
        return hasSystem( T::TypeId );
    }

    template < typename T >
    requires std::is_base_of_v< IEngineSystem, T >
    ONYX_NO_DISCARD T& GetSystem() {
        ONYX_ASSERT( HasSystem( T::TypeId ), "Module is not registered." );
        return static_cast< T& >( getSystem( T::TypeId ) );
    }

    template < typename T >
    requires std::is_base_of_v< IEngineSystem, T >
    ONYX_NO_DISCARD const T& GetSystem() const {
        ONYX_ASSERT( HasSystem( T::TypeId ), "Module is not registered." );
        return static_cast< const T& >( getSystem( T::TypeId ) );
    }

    ONYX_NO_DISCARD bool hasSystem( StringId32 systemId ) const override {
        return std::ranges::any_of( m_modules, [ = ]( const UniquePtr< IEngineSystem >& module ) {
            return module->getTypeId() == systemId;
        } );
    }

    IEngineSystem& getSystem( StringId32 systemId ) override {
        ONYX_ASSERT( HasSystem( systemId ), "Module is not added." );
        auto it = std::ranges::find_if( m_modules, [ = ]( const UniquePtr< IEngineSystem >& module ) {
            return module->getTypeId() == systemId;
        } );
        return *( *it );
    }

    ONYX_NO_DISCARD const IEngineSystem& getSystem( StringId32 systemId ) const override {
        ONYX_ASSERT( HasSystem( systemId ), "Module is not added." );
        auto it = std::ranges::find_if( m_modules, [ = ]( const UniquePtr< IEngineSystem >& module ) {
            return module->getTypeId() == systemId;
        } );
        return *( *it );
    }

  private:
    void onWindowDestroy( const platform::Window& window );

  private:
    struct SystemUpdate {
        uint32_t SystemIndex;
        EngineSystemFactory::UpdateFunction UpdateFunctionPtr;
    };

    bool m_isRunning = true;

    UniquePtr< Logger > m_logger;

    DynamicArray< UniquePtr< IEngineSystem > > m_modules;

    DynamicArray< SystemUpdate > m_updatableModules;
};

void onApplicationCreate();
void onApplicationCreated( Application& application );

void onApplicationShutdown( Application& application );
} // namespace onyx::application
