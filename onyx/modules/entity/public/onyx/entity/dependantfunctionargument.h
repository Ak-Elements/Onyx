#pragma once

#include <onyx/engine/enginesystem.h>

namespace onyx::ecs {
template < typename T >
class DependantFunctionArg;

template < EngineSystem T >
class DependantFunctionArg< T > {
  public:
    template < typename ContextT >
    static decltype( auto ) Get( const ContextT& context ) {
        return context.Engine.template getSystem< T >();
    }
};
} // namespace onyx::ecs