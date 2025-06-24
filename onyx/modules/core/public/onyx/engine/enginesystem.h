#pragma once
#include <onyx/noncopyable.h>

namespace Onyx
{
    enum class EngineSystemId : onyxU32;
    class IEngineSystem : public NonCopyable
    {
    };

    class IEngine : public NonCopyable
    {
    public:

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        ONYX_NO_DISCARD bool HasSystem() const
        {
            constexpr EngineSystemId systemId = static_cast<EngineSystemId>(TypeHash<T>());
            return HasSystem(systemId);
        }

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        ONYX_NO_DISCARD T& GetSystem()
        {
            constexpr EngineSystemId systemId = static_cast<EngineSystemId>(TypeHash<T>());
            ONYX_ASSERT(HasSystem(systemId), "System is not registered.");
            return static_cast<T&>(GetSystem(systemId));
        }

        template <typename T> requires std::is_base_of_v<IEngineSystem, T>
        ONYX_NO_DISCARD const T& GetSystem() const
        {
            constexpr EngineSystemId systemId = static_cast<EngineSystemId>(TypeHash<T>());
            ONYX_ASSERT(HasSystem(systemId), "System is not registered.");
            return static_cast<const T&>(GetSystem(systemId));
        }

        virtual bool HasSystem(EngineSystemId systemId) const = 0;
        virtual IEngineSystem& GetSystem(EngineSystemId systemId) = 0;
        virtual const IEngineSystem& GetSystem(EngineSystemId systemId) const = 0;
    };
}