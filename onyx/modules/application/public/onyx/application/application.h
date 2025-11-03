#pragma once
#include <onyx/engine/enginesystem.h>

#include <onyx/assets/asset.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/filesystem/path.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/graphicssystem.h>
#include <onyx/graphics/window.h>
#include <onyx/localization/localizationmodule.h>

namespace Onyx::Application
{
    struct EngineModuleFactory;
}

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

    class Application : public IEngine
    {
    public:
        Application();
        ~Application() override;

        Application(const Application& other) = delete;
        Application& operator=(const Application& other) = delete;

        Application(Application&& other) noexcept = default;
        Application& operator=(Application&& other) noexcept = default;

        void Init();
        void Shutdown();
        void Run();

        Logger* GetLogger() const { return m_Logger.get(); }

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        void RemoveModule()
        {
            ONYX_ASSERT(HasSystem<T>(), "Module is not added.");

            auto it = m_ShutdownFunctors.find(T::TypeId);
            if (it != m_ShutdownFunctors.end())
            {
                it->second();
                m_ShutdownFunctors.erase(it);
            }

            // TODO: Remove all modules that depend on it
            m_Modules.erase(T::TypeId);
        }

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        ONYX_NO_DISCARD bool HasSystem() const
        {
            return HasSystem(T::TypeId);
        }

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        ONYX_NO_DISCARD T& GetSystem()
        {
            ONYX_ASSERT(HasSystem(T::TypeId), "Module is not registered.");
            return static_cast<T&>(GetSystem(T::TypeId));
        }

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        ONYX_NO_DISCARD const T& GetSystem() const
        {
            ONYX_ASSERT(HasSystem(T::TypeId), "Module is not registered.");
            return static_cast<const T&>(GetSystem(T::TypeId));
        }

        bool HasSystem(StringId32 systemId) const override
        {
            return std::ranges::any_of(m_Modules, [=](const UniquePtr<IEngineSystem>& module) { return module->GetTypeId() == systemId; });
        }

        IEngineSystem& GetSystem(StringId32 systemId) override
        {
            ONYX_ASSERT(HasSystem(systemId), "Module is not added.");
            auto it = std::ranges::find_if(m_Modules, [=](const UniquePtr<IEngineSystem>& module) { return module->GetTypeId() == systemId; });
            return *it->get();
        }

        const IEngineSystem& GetSystem(StringId32 systemId) const override
        {
            ONYX_ASSERT(HasSystem(systemId), "Module is not added.");
            auto it = std::ranges::find_if(m_Modules, [=](const UniquePtr<IEngineSystem>& module) { return module->GetTypeId() == systemId; });
            return *it->get();
        }

    private:
        void OnWindowClose();

    private:
        bool m_IsRunning = true;

        UniquePtr<Logger> m_Logger;

        DynamicArray<UniquePtr<IEngineSystem>> m_Modules;
        HashMap<StringId32, InplaceFunction<void(), 64>> m_ShutdownFunctors;

        DynamicArray<InplaceFunction<void(DeltaGameTime)>> m_UpdatableModules;
    };

    void OnApplicationCreate();
    void OnApplicationCreated(Application& application);

    void OnApplicationShutdown(Application& application);
}
