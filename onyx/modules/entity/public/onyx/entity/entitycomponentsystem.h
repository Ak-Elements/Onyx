#pragma once

#include <onyx/entity/entityregistry.h>

namespace Onyx
{
    class IEngine;
}

namespace Onyx::Entity
{
    class EntityRegistry;
}

namespace Onyx::Graphics
{
    struct FrameContext;
}

namespace Onyx::Entity
{
    template <typename T, typename... Other>
    struct EntityQuery
    {
        using ViewT = entt::basic_view<entt::get_t<EntityRegistry::EntityRegistryT::storage_for_type<T>,
            EntityRegistry::EntityRegistryT::storage_for_type<Other>...>,
            entt::exclude_t<>>;

        EntityQuery(EntityRegistry& registry)
            : m_View(registry.GetView<T, Other...>())
        {
        }

        //template <typename... Excludes>
        ViewT& GetView(/*Excludes&&... excludes*/)
        {
            return m_View;
        }

    private:
        ViewT m_View;
        EntityRegistry* m_EntityRegistry = nullptr;
    };

    template <typename Type, typename... Other>
    struct Entity
    {
    public:
        Entity(EntityQuery<Type, Other...>& query, EntityId entityId)
            : m_Query(query)
            , m_EntityId(entityId)
        {
            
        }
        decltype(auto) Get() { return m_Query.GetView().template get<Type, Other...>(m_EntityId); }

    private:
        EntityQuery<Type, Other...>& m_Query;
        EntityId m_EntityId;
    };

    class IDependentFunctionArg
    {
    public:
        IDependentFunctionArg() = default;
        virtual ~IDependentFunctionArg() = default;
    };

    struct ECSExecutionContext
    {
        DeltaGameTime DeltaTime;
        EntityRegistry& Registry;
        IEngine& Engine;
    };

    template <typename T>
    class DependentFunctionArg : public IDependentFunctionArg
    {
    public:
        static T Get(const ECSExecutionContext& /*context*/) { return T{}; }
    };

    template <typename T, typename... Others>
    class DependentFunctionArg<EntityQuery<T, Others...>> : public IDependentFunctionArg
    {
    public:
        static EntityQuery<T, Others...> Get(const ECSExecutionContext& context)
        {
            return EntityQuery<T, Others...>(context.Registry);
        }
    };

    template <>
    class DependentFunctionArg<const EntityRegistry&> : public IDependentFunctionArg
    {
    public:
        ~DependentFunctionArg() override = default;

        static const EntityRegistry& Get(const ECSExecutionContext& context)
        {
            return context.Registry;
        }
    };

    template <>
    class DependentFunctionArg<DeltaGameTime> : public IDependentFunctionArg
    {
    public:
        ~DependentFunctionArg() override = default;

        static DeltaGameTime Get(const ECSExecutionContext& context)
        {
            return context.DeltaTime;
        }
    };

    class EntityComponentSystemsGraph
    {
    public:
        EntityComponentSystemsGraph() = default;
        ~EntityComponentSystemsGraph() = default;

        void SetRegistry(EntityRegistry* reg)
        {
            m_EntityRegistry = reg;
        }

        template <typename Callable>
        void Register(Callable callable)
        {
            InplaceFunction<void(const ECSExecutionContext&), 64> functor = BuildSystemCall<Callable>(callable);
            m_Systems.emplace_back(functor);
        }

        void Update(const ECSExecutionContext& context) const;

    private:
        template <typename System, typename... EntityQueryArgs, typename... Args>
        static auto BuildSystemCall(void(*callable)(EntityQuery<EntityQueryArgs...>, Args...))
        {
            return [=](const ECSExecutionContext& context)
            {
                const auto& dependencies = ForEachAndCollect<Tuple<Args...>>([&]<typename U>() -> U
                {
                    return DependentFunctionArg<U>::Get(context);
                });

                EntityQuery<EntityQueryArgs...> query(context.Registry);
                std::apply(callable, std::tuple_cat(std::make_tuple(query), dependencies));
            };
        }

        template <typename System, typename... EntityAccessT, typename... Args>
        static auto BuildSystemCall(void(*callable)(Entity<EntityAccessT...>, Args...))
        {
            return [=](const ECSExecutionContext& context)
                {
                    const auto& dependencies = ForEachAndCollect<Tuple<Args...>>([&]<typename U>() -> U
                    {
                        return DependentFunctionArg<U>::Get(context);
                    });

                    EntityQuery<EntityAccessT...> query(context.Registry);
                    const auto& entitiesView = query.GetView();
                    for (const EntityId entityId : entitiesView)
                    {
                        Entity<EntityAccessT...> entity{ query, entityId };
                        std::apply(callable, std::tuple_cat(std::make_tuple(entity), dependencies));
                    }
                };
        }

    private:

        EntityRegistry* m_EntityRegistry;
        // TODO: DAG?
        DynamicArray<InplaceFunction<void(const ECSExecutionContext&), 64>> m_Systems;
    };
}
