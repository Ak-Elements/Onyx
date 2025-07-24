#pragma once

#include <onyx/noncopyable.h>

namespace Onyx
{
    class IEngine;

    class IEngineSystem : public NonCopyable
    {
    public:
        virtual StringId32 GetTypeId() const = 0;
    };

    class IEngine : public NonCopyable
    {
    public:

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        ONYX_NO_DISCARD bool HasSystem() const
        {
            return HasSystem(T::TypeId);
        }

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        ONYX_NO_DISCARD T& GetSystem()
        {
            ONYX_ASSERT(HasSystem(T::TypeId), "System is not registered.");
            return static_cast<T&>(GetSystem(T::TypeId));
        }

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        ONYX_NO_DISCARD const T& GetSystem() const
        {
            ONYX_ASSERT(HasSystem(T::TypeId), "System is not registered.");
            return static_cast<const T&>(GetSystem(T::TypeId));
        }

        virtual bool HasSystem(StringId32 systemId) const = 0;
        virtual IEngineSystem& GetSystem(StringId32 systemId) = 0;
        virtual const IEngineSystem& GetSystem(StringId32 systemId) const = 0;
    };
}
