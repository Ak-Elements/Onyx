#pragma once
#include <onyx/engine/enginesystem.h>

#include <onyx/application/enginesystemfactory.h>

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

            const IEngineModuleMeta& meta = EngineModuleFactory::GetMeta(T::TypeId);
            auto it = std::ranges::find_if(m_Modules, [=](const UniquePtr<IEngineSystem>& module) { return module->GetTypeId() == T::TypeId; });

            if (meta.IsShutdownable())
            {
                UniquePtr<IEngineSystem>& systemInstance = *it;
                meta.Shutdown(*this, *systemInstance);
            }

            // TODO: Remove all modules that depend on it
            m_Modules.erase(it);
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

        DynamicArray<InplaceFunction<void(IEngine&, DeltaGameTime)>> m_UpdatableModules;
    };

    void OnApplicationCreate();
    void OnApplicationCreated(Application& application);

    void OnApplicationShutdown(Application& application);
}
