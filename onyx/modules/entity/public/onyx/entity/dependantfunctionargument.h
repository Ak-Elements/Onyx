#pragma once

#include <onyx/engine/enginesystem.h>

namespace Onyx::Entity
{
    template <typename T>
    class DependantFunctionArg;

    template <EngineSystem T>
    class DependantFunctionArg<T>
    {
    public:
        template <typename ContextT>
        static decltype(auto) Get(const ContextT& context)
        {
            return context.Engine.template GetSystem<T>();
        }
    };
}