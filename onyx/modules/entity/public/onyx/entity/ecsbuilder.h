#pragma once

#include <onyx/entity/componentfactory.h>
#include <onyx/entity/entitycomponentsystem.h>

namespace Onyx::Entity
{
    class EcsBuilder : public NonCopyable
    {
    public:
        EcsBuilder(ComponentFactory& componentFactory, EntityComponentSystemsGraph& entitySystemsGraph)
            : m_ComponentFactory(&componentFactory)
            , m_EntitySystemsGraph(&entitySystemsGraph)
        {
        }

        template <typename Callable>
        void RegisterSystem(Callable systemFunction) const
        {
            //TODO: auto register components
            m_EntitySystemsGraph->Register<Callable>(systemFunction);
        }

        template <typename ComponentT>
        void RegisterComponent() const
        {
            m_ComponentFactory->Register<ComponentT>();
        }

        template <typename ComponentT>
        void RegisterComponent(ComponentFactoryFunction<ComponentT> factory) const
        {
            m_ComponentFactory->Register<ComponentT>(factory);
        }

    private:
        ComponentFactory* m_ComponentFactory;
        EntityComponentSystemsGraph* m_EntitySystemsGraph;
    };
}
