#pragma once
#include <onyx/engine/enginesystem.h>

#include <onyx/serialize/deserializer.h>

namespace Onyx
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

    template<typename T>
    concept HasSettingsT = requires { typename T::SettingsT; };

    template <typename T>
    struct EngineSystemMeta {};

    //struct IEngineModuleMeta
    //{
    //    virtual ~IEngineModuleMeta() = default;
    //
    //    virtual constexpr bool IsInitializable() const = 0;
    //    virtual constexpr bool IsShutdownable() const = 0;
    //    virtual constexpr bool IsUpdatable() const = 0;
    //
    //    virtual UniquePtr<IEngineSystem> Create(IEngine& engine, const Deserializer& deserializer) const = 0;
    //    //virtual UniquePtr<IEngineSystem> Create(const Deserializer&) const = 0;
    //   // virtual void Init(IEngine& engine, IEngineSystem&) const = 0;
    //   // virtual void Shutdown(IEngine& engine, IEngineSystem&) const = 0;
    //    virtual InplaceFunction<void(IEngine&, DeltaGameTime)> BuildUpdateCall(IEngineSystem&) const = 0;
    //};
    //
    //template <typename T, typename... Args> requires std::is_base_of_v<IEngineSystem, T>// && std::is_constructible_v<T, Args...>
    //struct EngineModuleMeta final : public IEngineModuleMeta
    //{
    //    bool IsInitializable() const override { return Initializable<T>; }
    //    bool IsShutdownable() const override { return Shutdownable<T>; }
    //    bool IsUpdatable() const override { return Updatable<T>; }
    //
    //    UniquePtr<IEngineSystem> Create(IEngine& engine, const Deserializer& deserializer) const override
    //    {
    //        if constexpr (HasSettingsT<T>)
    //        {
    //            return MakeUnique<T>(ResolveConstructorArg<Args>(engine, deserializer)...);
    //        }
    //        else
    //        {
    //            return MakeUnique<T>(ResolveConstructorArg<Args>(engine)...);
    //        }
    //    }
    //
    // // UniquePtr<IEngineSystem> Create(const Deserializer& deserializer) const override
    // // {
    // //     if constexpr (Deserializable<T>)
    // //     {
    // //         UniquePtr<T> newInstance = MakeUnique<T>();
    // //         deserializer.Read(*newInstance);
    // //         return std::move(newInstance);
    // //     }
    // //     else
    // //     {
    // //         return MakeUnique<T>();
    // //     }
    // // }
    //
    //  //void Init(IEngine& engine, IEngineSystem& systemInstance) const override
    //  //{
    //  //    if constexpr (Initializable<T>)
    //  //    {
    //  //        T& typedSystemInstance = static_cast<T&>(systemInstance);
    //  //        using FunctionArgs = decltype(GetFunctionArgumentTypes(&T::Init));
    //  //        InvokeWithTypeList(FunctionArgs{}, [&]<typename... FunctionArg>()
    //  //        {
    //  //            typedSystemInstance.Init(engine.GetSystem<std::remove_reference_t<FunctionArg>>()...);
    //  //        });
    //  //    }
    //  //    else
    //  //    {
    //  //        ONYX_ASSERT(false, "Not supported");
    //  //    }
    //  //}
    //
    //   //void Shutdown(IEngine& engine, IEngineSystem& systemInstance) const override
    //   //{
    //   //    if constexpr (Shutdownable<T>)
    //   //    {
    //   //        T& typedSystemInstance = static_cast<T&>(systemInstance);
    //   //        using FunctionArgs = decltype(GetFunctionArgumentTypes(&T::Shutdown));
    //   //        InvokeWithTypeList(FunctionArgs{}, [&]<typename... FunctionArg>()
    //   //        {
    //   //            typedSystemInstance.Shutdown(engine.GetSystem<std::remove_reference_t<FunctionArg>>()...);
    //   //        });
    //   //    }
    //   //    else
    //   //    {
    //   //        ONYX_ASSERT(false, "Not supported");
    //   //    }
    //   //}
    //
    //    InplaceFunction<void(IEngine&, DeltaGameTime)> BuildUpdateCall(IEngineSystem& systemInstance) const override
    //    {
    //        if constexpr (Updatable<T>)
    //        {
    //            using FunctionArgs = decltype(GetFunctionArgumentTypes(&T::Update));
    //            return [&](IEngine& engine, DeltaGameTime deltaTime)
    //            {
    //                T& typedSystemInstance = static_cast<T&>(systemInstance);
    //                InvokeWithTypeList(FunctionArgs{}, [&]<typename... FunctionArg>()
    //                {
    //                    typedSystemInstance.Update(ResolveFunctionArg<FunctionArg>(engine, deltaTime)...);
    //                });
    //            };
    //        }
    //        else
    //        {
    //            ONYX_ASSERT(false, "Not supported");
    //            return nullptr;
    //        }
    //    }
    //
    //private:
    //    // decltype(auto) is needed to return settings as values and systems as &, just auto fails the deduction
    //    template<typename FunctionArg> requires HasSettingsT<T>
    //    auto ResolveConstructorArg(IEngine& engine, const Deserializer& deserializer) const -> decltype(auto)
    //    {
    //        using FunctionArgType = std::remove_cvref_t<FunctionArg>;
    //        if constexpr (std::is_base_of_v<IEngine, FunctionArgType>)
    //            return static_cast<FunctionArg&>(engine);
    //        else if constexpr (HasSettingsT<T> && std::is_same_v<typename T::SettingsT, FunctionArgType>)
    //        {
    //            typename T::SettingsT settings;
    //            deserializer.Read(settings);
    //            return settings;
    //        }
    //        else
    //        {
    //            return engine.GetSystem<FunctionArgType>();
    //        }
    //    }
    //
    //    template<typename FunctionArg> requires (HasSettingsT<T> == false)
    //    FunctionArg ResolveConstructorArg(IEngine& engine) const
    //    {
    //        using FunctionArgType = std::remove_cvref_t<FunctionArg>;
    //        if constexpr (std::is_base_of_v<IEngine, FunctionArgType>)
    //            return static_cast<FunctionArg>(engine);
    //        else
    //        {
    //            return engine.GetSystem<FunctionArgType>();
    //        }
    //    }
    //
    //    template<typename FunctionArg>
    //    FunctionArg ResolveFunctionArg(IEngine& engine, DeltaGameTime deltaTime) const
    //    {
    //        using FunctionArgType = std::remove_cvref_t<FunctionArg>;
    //        if constexpr (std::is_same_v<DeltaGameTime, FunctionArgType>)
    //            return deltaTime;
    //        else if constexpr (std::is_base_of_v<IEngine, FunctionArgType>)
    //            return static_cast<FunctionArg>(engine);
    //        else
    //            return engine.GetSystem<FunctionArgType>();
    //    }
    //};

    struct EngineSystemFactory
    {
        using CreateFunction = UniquePtr<IEngineSystem>(*)(const EngineSystemCreateContext&);
        using UpdateFunction = void(*)(IEngineSystem&, const EngineSystemUpdateContext&);

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        static bool Register()
        {
           // s_SystemMeta[T::TypeId] = new EngineModuleMeta<T, Args...>();
            s_CreateFunctions[T::TypeId] = &EngineSystemMeta<T>::Create;

            if constexpr (Updatable<T>)
            {
                s_UpdateFunctions[T::TypeId] = &EngineSystemMeta<T>::Update;
            }

            return true;
        }

        static UniquePtr<IEngineSystem> Create(StringId32 moduleId, const EngineSystemCreateContext& context)
        {
            ONYX_ASSERT(s_CreateFunctions.contains(moduleId));
            return s_CreateFunctions.at(moduleId)(context);
        }

        static Optional<UpdateFunction> GetUpdate(StringId32 moduleId)
        {
            auto it = s_UpdateFunctions.find(moduleId);
            if (it == s_UpdateFunctions.end())
                return std::nullopt;

            return it->second;
        }
        //static const IEngineModuleMeta& GetMeta(StringId32 moduleId) { return *s_SystemMeta.at(moduleId); }

    private:
        static inline HashMap<StringId32, CreateFunction> s_CreateFunctions;
        static inline HashMap<StringId32, UpdateFunction> s_UpdateFunctions;
    };
}
