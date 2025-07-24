#include <onyx/engine/enginesystem.h>

#include <onyx/application/application.h>
#include <onyx/input/inputmodulesettings.h>

namespace Onyx::Application
{
    struct EngineModuleMeta
    {
        std::function<UniquePtr<IEngineSystem>()> CreateFunctor;
        std::function<void(Application&, IEngineSystem&) > InitFunctor;
        std::function<void(Application&, IEngineSystem&)> ShutdownFunctor;

        std::function<void(Application&, IEngineSystem&, DeltaGameTime)> UpdateFunctor;
    };

    struct EngineModuleFactory
    {
    public:
        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        static bool RegisterSystem()
        {
            EngineModuleMeta& systemMeta = s_SystemMeta[T::TypeId];

            systemMeta.CreateFunctor = []() { return MakeUnique<T>(); };

            if constexpr (InitializableSystem<T>)
            {
                systemMeta.InitFunctor = [](Application& application, IEngineSystem& systemInstance)
                    {
                        T& typedSystemInstance = static_cast<T&>(systemInstance);
                        using FunctionArgs = decltype(GetFunctionArgumentTypes(&T::Init));
                        auto dependencies = ForEachAndCollect<FunctionArgs>([&]<typename U>() -> U { return GetModuleDependency<std::remove_reference_t<U>>(application); });
                        std::apply(std::bind_front(&T::Init, &typedSystemInstance), dependencies);
                    };
            }

            if constexpr (TerminableSystem<T>)
            {
                systemMeta.ShutdownFunctor = [](Application& application, IEngineSystem& systemInstance)
                    {
                        T& typedSystemInstance = static_cast<T&>(systemInstance);
                        using FunctionArgs = decltype(GetFunctionArgumentTypes(&T::Shutdown));
                        auto dependencies = ForEachAndCollect<FunctionArgs>([&]<typename U>() -> U { return GetModuleDependency<std::remove_reference_t<U>>(application); });
                        std::apply(std::bind_front(&T::Shutdown, &typedSystemInstance), dependencies);
                    };
            }

            if constexpr (UpdatableSystem<T>)
            {
                systemMeta.UpdateFunctor = [](Application& application, IEngineSystem& systemInstance, DeltaGameTime deltaTime)
                    {
                        using FunctionArgs = decltype(GetFunctionArgumentTypes(&T::Update));
                        auto dependencies = ForEachAndCollect<FunctionArgs>([&]<typename U>() -> U
                        {
                            if constexpr (std::is_same_v<U, DeltaGameTime>)
                            {
                                return deltaTime;
                            }
                            else
                            {
                                return GetModuleDependency<std::remove_reference_t<U>>(application);
                            }
                        });
                        T& typedSystemInstance = static_cast<T&>(systemInstance);
                        std::apply(std::bind_front(&T::Update, &typedSystemInstance), dependencies);
                    };
            }

            return true;
        }

        static const EngineModuleMeta& GetMeta(StringId32 moduleId) { return s_SystemMeta.at(moduleId); }

    private:
        template <typename T>
        static T& GetModuleDependency(Application& application)
        {
            if constexpr (std::is_base_of_v<ApplicationSettings, T>)
                return application.GetSettings();
            else if constexpr (std::is_base_of_v<Graphics::GraphicSettings, T>)
                return application.GetSettings().GraphicSettings;
            else if constexpr (std::is_base_of_v<Graphics::WindowSettings, T>)
                return application.GetSettings().WindowSettings;
            else if constexpr (std::is_base_of_v<Localization::LocalizationSettings, T>)
                return application.GetSettings().LocalizationSettings;
            else if constexpr (std::is_base_of_v<Input::InputModuleSettings, T>)
                return application.GetSettings().InputModuleSettings;
            else if constexpr (std::is_base_of_v<Graphics::Window, T>)
                return application.GetSystem<Graphics::GraphicsSystem>().GetWindow();
            else if constexpr (std::is_base_of_v<Graphics::GraphicsApi, T>)
                return application.GetSystem<Graphics::GraphicsSystem>().GetGraphicsApi();
            else if constexpr (std::is_base_of_v<IEngine, T>)
                return application;
            else
            {
                static_assert(std::is_base_of_v<IEngineSystem, T>, "Dependency must be derived from IEngineSystem or Window/GraphicsApi");
                return application.GetSystem<T>();
            }
        }
        
    private:
        static inline HashMap<StringId32, EngineModuleMeta> s_SystemMeta;
    };
}
