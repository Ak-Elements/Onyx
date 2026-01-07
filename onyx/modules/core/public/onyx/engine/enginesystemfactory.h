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

    struct EngineSystemFactory
    {
        using CreateFunction = UniquePtr<IEngineSystem>(*)(const EngineSystemCreateContext&);
        using UpdateFunction = void(*)(IEngineSystem&, const EngineSystemUpdateContext&);

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        static bool Register()
        {
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

    private:
        static inline HashMap<StringId32, CreateFunction> s_CreateFunctions;
        static inline HashMap<StringId32, UpdateFunction> s_UpdateFunctions;
    };
}
