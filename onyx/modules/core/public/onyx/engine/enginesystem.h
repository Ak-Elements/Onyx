#pragma once

#include <onyx/noncopyable.h>
#include <onyx/serialize/deserializer.h>
#include <onyx/engine/enginevariablesregistry.h>

namespace Onyx
{
    class IEngine;
    class Deserializer;

    class IEngineSystem : public NonCopyable
    {
    public:
        virtual StringId32 GetTypeId() const = 0;
    };

    template <typename T>
    concept EngineSystem = std::is_base_of_v<IEngineSystem, T>;

    class IEngine : public NonCopyable
    {
    public:
        template <typename T>// requires std::is_base_of_v<IEngineSystem, T>
        ONYX_NO_DISCARD bool HasSystem() const
        {
            return HasSystem(T::TypeId);
        }

        template <typename T>// requires std::is_base_of_v<IEngineSystem, T>
        ONYX_NO_DISCARD T& GetSystem()
        {
            ONYX_ASSERT(HasSystem(T::TypeId), "System is not registered.");
            return static_cast<T&>(GetSystem(T::TypeId));
        }

        template <typename T>// requires std::is_base_of_v<IEngineSystem, T>
        ONYX_NO_DISCARD const T& GetSystem() const
        {
            ONYX_ASSERT(HasSystem(T::TypeId), "System is not registered.");
            return static_cast<const T&>(GetSystem(T::TypeId));
        }

        virtual bool HasSystem(StringId32 systemId) const = 0;
        virtual IEngineSystem& GetSystem(StringId32 systemId) = 0;
        virtual const IEngineSystem& GetSystem(StringId32 systemId) const = 0;

        static const EngineVariablesRegistry& GetVariablesRegistry() { return s_EngineVariables; }

        static void Register(IEngineVariable& variable)
        {
            s_EngineVariables.Register(variable);
        } 

        static void Unregister(IEngineVariable& variable)
        {
            s_EngineVariables.Unregister(variable);
        }

    private:
        static inline EngineVariablesRegistry s_EngineVariables; 
    };

    struct EngineSystemCreateContext
    {
        IEngine& Engine;
        const Deserializer& Deserializer;

        template <typename T> requires std::is_base_of_v<IEngine, T>
        T& Get() const { return static_cast<T&>(Engine); }

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        T& Get() const { return Engine.GetSystem<T>(); }

        template <typename T> requires (Deserializable<T> && (std::is_base_of_v<IEngineSystem, T> == false))
        auto Get() const
        {
            T obj;
            Deserializer.Read<"settings">(obj);
            return obj;
        }
    };

    struct EngineSystemUpdateContext
    {
        IEngine& Engine;
        DeltaGameTime Delta;
        GameTime Time;

        template <typename T> requires std::is_base_of_v<IEngine, T>
        T& Get() const { return static_cast<T&>(Engine); }

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        T& Get() const { return Engine.GetSystem<T>(); }

        template <typename T> requires std::is_same_v<DeltaGameTime, T>
        DeltaGameTime Get() const { return Delta; }

        template <typename T> requires std::is_same_v<GameTime, T>
        GameTime Get() const { return Time; }
    };
}
