#pragma once
#include <onyx/noncopyable.h>

namespace Onyx
{
    enum class EngineSystemId : onyxU32;
    class IEngineSystem : public NonCopyable
    {
    public:
        virtual ~IEngineSystem() = default;
    };
}