#pragma once
#include <onyx/engine/enginesystem.h>

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

    struct IEngineModuleMeta
    {
        virtual ~IEngineModuleMeta() = default;

        virtual constexpr bool IsInitializable() const = 0;
        virtual constexpr bool IsShutdownable() const = 0;
        virtual constexpr bool IsUpdatable() const = 0;

        virtual UniquePtr<IEngineSystem> Create() const = 0;
        virtual UniquePtr<IEngineSystem> Create(const Deserializer&) const = 0;
        virtual void Init(IEngine& engine, IEngineSystem&) const = 0;
        virtual void Shutdown(IEngine& engine, IEngineSystem&) const = 0;
        virtual InplaceFunction<void(IEngine&, DeltaGameTime)> BuildUpdateCall(IEngineSystem&) const = 0;
    };

    template <typename T> requires std::is_base_of_v<IEngineSystem, T>
    struct EngineModuleMeta final : public IEngineModuleMeta
    {
        bool IsInitializable() const override { return Initializable<T>; }
        bool IsShutdownable() const override { return Shutdownable<T>; }
        bool IsUpdatable() const override { return Updatable<T>; }

        UniquePtr<IEngineSystem> Create() const override  { return MakeUnique<T>(); }
        UniquePtr<IEngineSystem> Create(const Deserializer& deserializer) const override
        {
            if constexpr (Deserializable<T>)
            {
                UniquePtr<T> newInstance = MakeUnique<T>();
                deserializer.Read(*newInstance);
                return std::move(newInstance);
            }
            else
            {
                return MakeUnique<T>();
            }
        }

        void Init(IEngine& engine, IEngineSystem& systemInstance) const override
        {
            if constexpr (Initializable<T>)
            {
                T& typedSystemInstance = static_cast<T&>(systemInstance);
                using FunctionArgs = decltype(GetFunctionArgumentTypes(&T::Init));
                InvokeWithTypeList(FunctionArgs{}, [&]<typename... FunctionArg>()
                {
                    typedSystemInstance.Init(engine.GetSystem<std::remove_reference_t<FunctionArg>>()...);
                });
            }
            else
            {
                ONYX_ASSERT(false, "Not supported");
            }
        }

        void Shutdown(IEngine& engine, IEngineSystem& systemInstance) const override
        {
            if constexpr (Shutdownable<T>)
            {
                T& typedSystemInstance = static_cast<T&>(systemInstance);
                using FunctionArgs = decltype(GetFunctionArgumentTypes(&T::Shutdown));
                InvokeWithTypeList(FunctionArgs{}, [&]<typename... FunctionArg>()
                {
                    typedSystemInstance.Shutdown(engine.GetSystem<std::remove_reference_t<FunctionArg>>()...);
                });
                //CallShutdown<T, FunctionArgs...>(typedSystemInstance, engine);
            }
            else
            {
                ONYX_ASSERT(false, "Not supported");
            }
        }

        InplaceFunction<void(IEngine&, DeltaGameTime)> BuildUpdateCall(IEngineSystem& systemInstance) const override
        {
            if constexpr (Updatable<T>)
            {
                using FunctionArgs = decltype(GetFunctionArgumentTypes(&T::Update));
                return [&](IEngine& engine, DeltaGameTime deltaTime)
                {
                    T& typedSystemInstance = static_cast<T&>(systemInstance);
                    InvokeWithTypeList(FunctionArgs{}, [&]<typename... FunctionArg>()
                    {
                        typedSystemInstance.Update(ResolveFunctionArg<FunctionArg>(engine, deltaTime)...);
                    });
                };
            }
            else
            {
                ONYX_ASSERT(false, "Not supported");
                return nullptr;
            }
        }

    private:
        template<typename FunctionArg>
        FunctionArg ResolveFunctionArg(IEngine& engine, DeltaGameTime deltaTime) const
        {
            if constexpr (std::is_same_v<DeltaGameTime, FunctionArg>)
                return deltaTime;
            else if constexpr (std::is_base_of_v<IEngine, std::remove_const_t<std::remove_reference_t<FunctionArg>>>)
                return static_cast<FunctionArg>(engine);
            else
                return engine.GetSystem<std::remove_reference_t<FunctionArg>>();
        }

        template <typename... Args>
        void CallInit(T& instance, IEngine& engine) const
        {
            instance.Init(engine.GetSystem<std::remove_reference_t<Args>>()...);
        }

        template <typename... Args>
        void CallShutdown(T& instance, IEngine& engine) const
        {
            instance.Shutdown(engine.GetSystem<std::remove_reference_t<Args>>()...);
        }

        template <typename... Args>
        void CallUpdate(T& instance, IEngine& engine) const
        {
            instance.Update(engine.GetSystem<std::remove_reference_t<Args>>()...);
        }
    };

    struct EngineModuleFactory
    {
    public:
        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        static bool RegisterSystem()
        {
            s_SystemMeta[T::TypeId] = MakeUnique<EngineModuleMeta<T>>();
            return true;
        }

        
        static const IEngineModuleMeta& GetMeta(StringId32 moduleId) { return *s_SystemMeta.at(moduleId); }

    private:
        static inline HashMap<StringId32, UniquePtr<IEngineModuleMeta>> s_SystemMeta;
    };
}
