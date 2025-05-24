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
        virtual bool HasSystem(EngineSystemId systemId) const = 0;
        virtual IEngineSystem& GetSystem(EngineSystemId systemId) = 0;
        virtual const IEngineSystem& GetSystem(EngineSystemId systemId) const = 0;
    };
}