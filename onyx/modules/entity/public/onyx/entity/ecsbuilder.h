#pragma once

#include <onyx/entity/componentfactory.h>
#include <onyx/entity/entityaccess.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/noncopyable.h>

namespace onyx::ecs {
class EcsBuilder : public NonCopyable {
  public:
    EcsBuilder( ComponentFactory& componentFactory, EntityComponentSystemsGraph& entitySystemsGraph )
        : m_componentFactory( &componentFactory )
        , m_entitySystemsGraph( &entitySystemsGraph ) {}

    template < typename Callable >
    void registerSystem( Callable systemFunction ) const {
        // TODO: auto register components
        m_entitySystemsGraph->registerSystem< Callable >( systemFunction );
    }

    template < typename ComponentT >
    void registerComponent() const {
        m_componentFactory->Register< ComponentT >();
    }

    template < typename ComponentT >
    void registerComponent( ComponentFactoryFunction< ComponentT > factory ) const {
        m_componentFactory->Register< ComponentT >( factory );
    }

  private:
    ComponentFactory* m_componentFactory;
    EntityComponentSystemsGraph* m_entitySystemsGraph;
};
} // namespace onyx::ecs
