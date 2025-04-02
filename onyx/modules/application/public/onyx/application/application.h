#pragma once
#include <onyx/noncopyable.h>
#include <onyx/engine/enginesystem.h>

#include <onyx/entity/typeinfo.h>
#include <onyx/typetraits.h>
#include <onyx/assets/asset.h>

#include <onyx/filesystem/path.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/window.h>
#include <onyx/graphics/graphicssystem.h>

namespace Onyx::Graphics
{
    enum class ApiType : onyxU8;
}

namespace Onyx
{
    namespace Input
    {
        struct InputAction;
    }

    namespace Assets
    {
        class AssetSystem;
    }

    class Logger;

    namespace Graphics
    {
        class RenderGraph;
        class Window;
        class GraphicsApi;
        class GraphicsSystem;
    }
}

namespace Onyx::Application
{
    class TaskGraph;
    class ApplicationModule;

    struct ApplicationSettings
    {
        ApplicationSettings();

        String Name = "Onyx";
        bool UseFixedUpdateInterval = true;
        onyxU64 FixedUpdateInterval = 20;

        Graphics::GraphicSettings GraphicSettings;
        Graphics::WindowSettings WindowSettings;

        HashMap<onyxU32, FileSystem::MountPoint> MountPoints;

        Assets::AssetId InputConfig = "engine:/inputcontexts.oinput";
    };

    // Helper to detect member function existence
    template <typename, typename = void>
    struct HasInit : std::false_type {};

    template <typename T>
    struct HasInit<T, std::void_t<decltype(&T::Init)>> : std::true_type {};

    template <typename, typename = void>
    struct HasShutdown : std::false_type {};

    template <typename T>
    struct HasShutdown<T, std::void_t<decltype(&T::Shutdown)>> : std::true_type {};

    // Helper to detect member function existence
    template <typename, typename = void>
    struct HasUpdate : std::false_type {};

    template <typename T>
    struct HasUpdate<T, std::void_t<decltype(&T::Update)>> : std::true_type {};

    // Concept to check if a type has an Init function with any parameters
    template <typename T>
    concept InitializableSystem = HasInit<T>::value;

    // Concept to check if a type has a Shutdown function with any parameters
    template <typename T>
    concept TerminableSystem = HasShutdown<T>::value;

    // Concept to check if a type has a Shutdown function with any parameters
    template <typename T>
    concept UpdatableSystem = HasUpdate<T>::value;

    class Application : public NonCopyable
    {
    public:
        Application(const ApplicationSettings& settings);
        ~Application();

        Application(const Application& other) = delete;
        Application& operator=(const Application& other) = delete;

        Application(Application&& other) noexcept = default;
        Application& operator=(Application&& other) noexcept = default;

        const ApplicationSettings& GetSettings() const { return m_Settings; }

        void Run();
        void Shutdown();

        Logger* GetLogger() const { return m_Logger.get(); }

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        bool HasSystem() const
        {
            constexpr EngineSystemId systemId = static_cast<EngineSystemId>(TypeHash<T>());
            return m_Modules.contains(systemId);
        }

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        T& GetSystem()
        {
            ONYX_ASSERT(HasSystem<T>(), "System is not registered.");
            constexpr EngineSystemId systemId = static_cast<EngineSystemId>(TypeHash<T>());
            return static_cast<T&>(*m_Modules[systemId]);
        }

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        const T& GetSystem() const
        {
            ONYX_ASSERT(HasSystem<T>(), "System is not registered.");
            constexpr EngineSystemId systemId = static_cast<EngineSystemId>(TypeHash<T>());
            return static_cast<const T&>(*m_Modules.at(systemId));
        }

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        T& AddSystem()
        {
            ONYX_ASSERT(HasSystem<T>() == false, "System is already registered.");
            constexpr EngineSystemId systemId = static_cast<EngineSystemId>(TypeHash<T>());

            UniquePtr<T> newSystem= MakeUnique<T>();

            if constexpr (InitializableSystem<T>)
            {
                using FunctionArgs = decltype(GetFunctionArgumentTypes(&T::Init));
                auto dependencies = ForEachAndCollect<FunctionArgs>([&]<typename U>() -> U { return GetSystemDependency<std::remove_reference_t<U>>(); });
                std::apply(std::bind_front(&T::Init, newSystem.get()), dependencies);
            }

            if constexpr (TerminableSystem<T>)
            {
                using FunctionArgs = decltype(GetFunctionArgumentTypes(&T::Shutdown));
                auto dependencyArgs = ForEachAndCollect<FunctionArgs>([&]<typename U>() -> U { return GetSystemDependency<std::remove_reference_t<U>>(); });
                m_ShutdownFunctors.emplace(systemId, [system = newSystem.get(), dependencyArgs]()
                {
                    std::apply(std::bind_front(&T::Shutdown, system), dependencyArgs);
                });
            }

            if constexpr (UpdatableSystem<T>)
            {
                m_UpdatableModules.emplace_back([system = newSystem.get()](onyxU64 deltaTime)
                {
                    system->Update(deltaTime);
                });
            }
            
            T& moduleRef = *newSystem;
            m_Modules[systemId] = std::move(newSystem);
            return moduleRef;
        }

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        void RemoveModule()
        {
            ONYX_ASSERT(HasSystem<T>(), "System is not registered.");

            constexpr EngineSystemId systemId = static_cast<EngineSystemId>(TypeHash<T>());
            auto it = m_ShutdownFunctors.find(systemId);
            if (it != m_ShutdownFunctors.end())
            {
                it->second();
                m_ShutdownFunctors.erase(it);
            }

            // TODO: Remove all modules that depend on it
            m_Modules.erase(systemId);
        }

    private:
        void OnWindowClose();

        template <typename T>
        T& GetSystemDependency()
        {
            if constexpr (std::is_base_of_v<ApplicationSettings, T>)
                return m_Settings;
            else if constexpr (std::is_base_of_v<Graphics::GraphicSettings, T>)
                    return m_Settings.GraphicSettings;
            else if constexpr (std::is_base_of_v<Graphics::WindowSettings, T>)
                return m_Settings.WindowSettings;
            else if constexpr (std::is_base_of_v<Graphics::Window, T>)
                return GetSystem<Graphics::GraphicsSystem>().GetWindow();
            else if constexpr (std::is_base_of_v<Graphics::GraphicsApi, T>)
                return GetSystem<Graphics::GraphicsSystem>().GetGraphicsApi();
            else if constexpr (std::is_same_v<Application, T>)
                return *this;
            else
            {
                static_assert(std::is_base_of_v<IEngineSystem, T>, "Dependency must be derived from ApplicationModule or Window/GraphicsApi");
                return GetSystem<T>();
            }
        }

    private:
        onyxU64 m_LastFrameTime = 0;
        bool m_IsRunning = true;

        ApplicationSettings m_Settings;

        UniquePtr<Logger> m_Logger;

        HashMap<EngineSystemId, UniquePtr<IEngineSystem>> m_Modules;
        HashMap<EngineSystemId, InplaceFunction<void(), 64>> m_ShutdownFunctors;

        DynamicArray<InplaceFunction<void(onyxU64)>> m_UpdatableModules;
    };

    void OnApplicationCreate(ApplicationSettings& settings);
    void OnApplicationCreated(Application& application);

    void OnApplicationShutdown(Application& application);
}
