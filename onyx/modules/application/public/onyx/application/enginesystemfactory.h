#include <onyx/engine/enginesystem.h>

#include <onyx/application/application.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx::Application
{
    template <typename T>
    concept IsMemberFunctionPointer = std::is_member_function_pointer_v<T>;

    template <typename T>
    concept Initializable = requires (T type)
    {
        { &T::Init } -> IsMemberFunctionPointer;
    };

    template <typename T>
    concept Shutdownable = requires (T type)
    {
        { &T::Shutdown } -> IsMemberFunctionPointer;
    };

    template <typename T>
    concept Updatable = requires (T type)
    {
        { &T::Update } -> IsMemberFunctionPointer;
    };

    struct EngineModuleMeta
    {
        Function<UniquePtr<IEngineSystem>()> CreateFunctor;
        Function<UniquePtr<IEngineSystem>(const Deserializer&)> CreateWithConfigFunctor;
        Function<void(Application&, IEngineSystem&)> InitFunctor;
        Function<void(Application&, IEngineSystem&)> ShutdownFunctor;

        Function<void(Application&, IEngineSystem&, DeltaGameTime)> UpdateFunctor;
    };

    struct EngineModuleFactory
    {
    public:
        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        static bool RegisterSystem()
        {
            EngineModuleMeta& systemMeta = s_SystemMeta[T::TypeId];

            systemMeta.CreateFunctor = []() { return MakeUnique<T>(); };

            if constexpr (Deserializable<T>)
            {
                systemMeta.CreateWithConfigFunctor = [](const Deserializer& deserializer)
                {
                    UniquePtr<T> newInstance = MakeUnique<T>();
                    deserializer.Read(*newInstance);
                    return std::move(newInstance);
                };
            }

            if constexpr (Initializable<T>)
            {
                systemMeta.InitFunctor = [](Application& application, IEngineSystem& systemInstance)
                {
                    T& typedSystemInstance = static_cast<T&>(systemInstance);
                    using FunctionArgs = decltype(GetFunctionArgumentTypes(&T::Init));

                    auto dependencies = ForEachAndCollect<FunctionArgs>([&]<typename U>() -> U
                    {
                        return GetModuleDependency<std::remove_reference_t<U>>(application);
                    });

                    std::apply(std::bind_front(&T::Init, &typedSystemInstance), dependencies);
                };
                
            }

            if constexpr (Shutdownable<T>)
            {
                systemMeta.ShutdownFunctor = [](Application& application, IEngineSystem& systemInstance)
                {
                    T& typedSystemInstance = static_cast<T&>(systemInstance);
                    using FunctionArgs = decltype(GetFunctionArgumentTypes(&T::Shutdown));
                    auto dependencies = ForEachAndCollect<FunctionArgs>([&]<typename U>() -> U { return GetModuleDependency<std::remove_reference_t<U>>(application); });
                    std::apply(std::bind_front(&T::Shutdown, &typedSystemInstance), dependencies);
                };
            }

            if constexpr (Updatable<T>)
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
            if constexpr (std::is_base_of_v<Graphics::Window, T>)
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
