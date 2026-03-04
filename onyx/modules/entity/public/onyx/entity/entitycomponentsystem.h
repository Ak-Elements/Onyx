#pragma once

#include <onyx/entity/entityregistry.h>
#include <onyx/engine/enginesystem.h>

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
            , m_EntityRegistry(&registry)
        {
        }

        //template <typename... Excludes>
        ViewT& GetView(/*Excludes&&... excludes*/)
        {
            return m_View;
        }

       //template <typename U>
       //U* TryGetComponent(EntityId id)
       //{
       //    return m_EntityRegistry->TryGetComponent<U>(id);
       //}
        EntityRegistry* GetRegistry() { return m_EntityRegistry; }
    private:
        ViewT m_View;
        EntityRegistry* m_EntityRegistry = nullptr;
    };

    template <typename... Types>
    struct Entity
    {
    public:
        Entity(EntityQuery<Types...>& query, EntityId entityId)
            : m_Query(query)
            , m_EntityId(entityId)
        {
        }
        
        decltype(auto) Get() { return m_Query.GetView().template get<Types...>(m_EntityId); }

        EntityId GetId() const { return m_EntityId; }

       template <typename U>
       U* TryGetComponent()
       {
           return m_Query.GetRegistry()->template TryGetComponent<U>(m_EntityId);
       }

    private:
        EntityQuery<Types...>& m_Query;
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

    struct EntityCommandBuffer
    {
        EntityCommandBuffer(EntityRegistry& registry)
            : m_Registry(&registry)
        {
        }

        // TODO: they should probably be run at the end of the frame not at the end of the system call
        ~EntityCommandBuffer()
        {
            for (const auto& queuedCommand : m_QueuedCommands)
            {
                queuedCommand();
            }
        }

        template <typename T>
        void AddComponent(EntityId entityId)
        {
            m_QueuedCommands.push_back([registry = m_Registry, entityId]()
            {
                registry->AddComponent<T>(entityId);
            });
        }

        template <typename T, typename... Args>
        void AddComponent(EntityId entityId, Args&&... args)
        {
            m_QueuedCommands.push_back([registry = m_Registry, entityId, ... args = std::forward<Args>(args)]() mutable
            {
                registry->AddComponent<T>(entityId, std::forward<Args>(args)...);
            });
        }

        template <typename T,  typename EntityAccessT, typename... Args> requires is_specialization_of_v<Entity, EntityAccessT>
        void AddComponent(EntityAccessT entity, Args&&... args)
        {
            m_QueuedCommands.push_back([registry = m_Registry, entityId = entity.GetId(), ... args = std::forward<Args>(args)]() mutable
            {
                registry->AddComponent<T>(entityId, std::forward<Args>(args)...);
            });
        }


        template <typename T>
        void RemoveComponent(EntityId entityId)
        {
            m_QueuedCommands.push_back([registry = m_Registry, entityId]()
            {
                registry->RemoveComponent<T>(entityId);
            });
        }

        template <typename T, typename EntityAccessT> requires is_specialization_of_v<Entity, EntityAccessT>
        void RemoveComponent(EntityAccessT entity)
        {
            m_QueuedCommands.push_back([registry = m_Registry, entityId = entity.GetId()]()
            {
                registry->RemoveComponent<T>(entityId);
            });
        }

    private:
        EntityRegistry* m_Registry = nullptr;
        DynamicArray<InplaceFunction<void()>> m_QueuedCommands;
    };

    template <typename T>
    class DependentFunctionArg : public IDependentFunctionArg
    {
    public:
        static decltype(auto) Get(const ECSExecutionContext& context)
        {
            if constexpr (std::is_base_of_v<IEngineSystem, std::remove_reference_t<T>>)
            {
                return context.Engine.GetSystem<std::remove_reference_t<T>>();
            }
            else
            {
                return T{};
            }
        }
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

    template <>
    class DependentFunctionArg<EntityCommandBuffer> : public IDependentFunctionArg
    {
    public:
        ~DependentFunctionArg() override = default;

        static EntityCommandBuffer Get(const ECSExecutionContext& context)
        {
            return EntityCommandBuffer(context.Registry);
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
                EntityQuery<EntityQueryArgs...> query(context.Registry);
                InvokeWithTypeList(TypeList<Args...>{}, [&]<typename... FunctionArg>()
                {
                    callable(query, DependentFunctionArg<FunctionArg>::Get(context)...);
                });
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

        template <typename System, typename... Args>
        static auto BuildSystemCall(void(*callable)(Args...))
        {
            return [=](const ECSExecutionContext& context)
            {
                InvokeWithTypeList(TypeList<Args...>{}, [&]<typename... FunctionArg>()
                {
                    callable(DependentFunctionArg<FunctionArg>::Get(context)...);
                });
            };
        }


    private:

        EntityRegistry* m_EntityRegistry;
        // TODO: DAG?
        DynamicArray<InplaceFunction<void(const ECSExecutionContext&), 64>> m_Systems;
    };
}
