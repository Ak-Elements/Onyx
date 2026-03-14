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
    template <typename... ComponentAccessDefinition>
    struct EntityQuery;

    template <typename... ComponentAccessDefinition>
    struct Entity;

    struct Access
    {
        enum class MemoryAccess : onyxU8
        {
            None,
            Read,
            Write
        };

        enum class Filter : onyxU8
        {
            Include,
            Exclude,
            Maybe
        };

    private:
        template<typename T, MemoryAccess A, Filter F>
        struct ComponentAccessDefinition
        {
            using Type = T;
            static constexpr MemoryAccess Access = A;
            static constexpr Filter Filter = F;
        };

        template <typename... ComponentAccessDefinitions>
        struct AccessBuilder
        {
            template<typename... Ts>
            using Read = AccessBuilder<ComponentAccessDefinitions..., ComponentAccessDefinition<Ts, MemoryAccess::Read, Filter::Include>...>;

            template<typename... Ts>
            using ReadIfExists = AccessBuilder<ComponentAccessDefinitions..., ComponentAccessDefinition<Ts, MemoryAccess::Read, Filter::Maybe>...>;

            template<typename... Ts>
            using Write = AccessBuilder<ComponentAccessDefinitions..., ComponentAccessDefinition<Ts, MemoryAccess::Write, Filter::Include>...>;

            template<typename... Ts>
            using WriteIfExits = AccessBuilder<ComponentAccessDefinitions..., ComponentAccessDefinition<Ts, MemoryAccess::Write, Filter::Maybe>...>;

            template<typename... Ts>
            using With = AccessBuilder<ComponentAccessDefinitions..., ComponentAccessDefinition<Ts, MemoryAccess::None, Filter::Include>...>;

            template<typename... Ts>
            using Without = AccessBuilder<ComponentAccessDefinitions..., ComponentAccessDefinition<Ts, MemoryAccess::None, Filter::Exclude>...>;

            template<typename... Ts>
            using Has = AccessBuilder<ComponentAccessDefinitions..., ComponentAccessDefinition<Ts, MemoryAccess::None, Filter::Maybe>...>;

            using ReadComponents = decltype(std::tuple_cat(
                std::conditional_t< ( ComponentAccessDefinitions::Access == MemoryAccess::Read ),
                    std::tuple<typename ComponentAccessDefinitions::Type>, std::tuple<>>{}...));

            using WriteComponents = decltype(std::tuple_cat(
                std::conditional_t< ( ComponentAccessDefinitions::Access == MemoryAccess::Write ),
                    std::tuple<typename ComponentAccessDefinitions::Type>, std::tuple<>>{}...));

            using AsQuery = EntityQuery<ComponentAccessDefinitions...>;
            using AsEntity = Entity<ComponentAccessDefinitions...>;
        };

        public:
            template<typename... Ts>
            using Read = AccessBuilder<ComponentAccessDefinition<Ts, MemoryAccess::Read, Filter::Include>...>;

            template<typename... Ts>
            using ReadIfExists = AccessBuilder<ComponentAccessDefinition<Ts, MemoryAccess::Read, Filter::Maybe>...>;

            template<typename... Ts>
            using Write = AccessBuilder<ComponentAccessDefinition<Ts, MemoryAccess::Write, Filter::Include>...>;

            template<typename... Ts>
            using WriteIfExits = AccessBuilder<ComponentAccessDefinition<Ts, MemoryAccess::Write, Filter::Maybe>...>;

            template<typename... Ts>
            using With = AccessBuilder<ComponentAccessDefinition<Ts, MemoryAccess::None, Filter::Include>...>;

            template<typename... Ts>
            using Without = AccessBuilder<ComponentAccessDefinition<Ts, MemoryAccess::None, Filter::Exclude>...>;

            template<typename... Ts>
            using Has = AccessBuilder<ComponentAccessDefinition<Ts, MemoryAccess::None, Filter::Maybe>...>;
    };

    template <typename... ComponentAccessDefinitions>
    struct EntityQuery
    {
        using IncludedComponents = decltype(std::tuple_cat(
            std::conditional_t< ( ComponentAccessDefinitions::Filter == Access::Filter::Include ),
                std::tuple<ComponentAccessDefinitions>, std::tuple<>>{}...));

        using ExcludedComponents = decltype(std::tuple_cat(
            std::conditional_t< ( ComponentAccessDefinitions::Filter == Access::Filter::Exclude ),
                std::tuple<ComponentAccessDefinitions>, std::tuple<>>{}...));

        template<typename... Included, typename... Excluded>
        constexpr static auto MakeViewType(std::tuple<Included...>, std::tuple<Excluded...>)
        {
            return entt::basic_view<entt::get_t<EntityRegistry::EntityRegistryT::storage_for_type<
                std::conditional_t<Included::Access == Access::MemoryAccess::Write, typename Included::Type, const typename Included::Type>>...>,
                entt::exclude_t<typename Excluded::Type...>>();
        }

        using ViewT = decltype(MakeViewType(IncludedComponents{}, ExcludedComponents{}));

        EntityQuery(const EntityQuery&) = delete;
        EntityQuery &operator=(const EntityQuery&) = delete;

        EntityQuery(EntityRegistry& registry)
            : m_View(MakeView(registry, IncludedComponents{},
                              ExcludedComponents{})),
              m_EntityRegistry(&registry) {}

        ViewT& GetView() { return m_View; }
        const ViewT& GetView() const { return m_View; }
    
        EntityRegistry* GetRegistry() { return m_EntityRegistry; }

        ViewT::iterator begin() { return m_View.begin(); }         
        ViewT::iterator end() { return m_View.end(); }         

    private:
        template<typename... Included, typename... Excluded>
        static ViewT MakeView(EntityRegistry& registry, std::tuple<Included...>, std::tuple<Excluded...>)
        {
            return registry.GetView<
                std::conditional_t<
                    Included::Access == Access::MemoryAccess::Write, // Condition 
                    typename Included::Type, const typename Included::Type>...> // Type
                ();
        }
        
    private:
        ViewT m_View;
        EntityRegistry* m_EntityRegistry = nullptr;
    };

    template <typename... ComponentAccessDefinitions>
    struct Entity
    {
    public:
        using IncludedMemoryAccessComponents = decltype(std::tuple_cat(
            std::conditional_t<
                ( ComponentAccessDefinitions::Access != Access::MemoryAccess::None ) && // Condition
                ( ComponentAccessDefinitions::Filter == Access::Filter::Include ), // Condition
            std::tuple<ComponentAccessDefinitions>, std::tuple<>>{}...));

        Entity(EntityQuery<ComponentAccessDefinitions...>& query, EntityId entityId)
            : m_Query(query)
            , m_EntityId(entityId)
        {
        }
       
        template<std::size_t I>
        decltype(auto) get()
        { 
            using ComponentAccessDefinition = std::tuple_element_t<I, IncludedMemoryAccessComponents >;
            using ComponentT = std::conditional_t<
                ComponentAccessDefinition::Access == Access::MemoryAccess::Write,
                typename ComponentAccessDefinition::Type,
                const typename ComponentAccessDefinition::Type>; 

            return m_Query.GetView().template get<ComponentT>(m_EntityId);


        }

        EntityId GetId() const { return m_EntityId; }

        template <typename T> requires (
            (std::is_same_v<T, typename ComponentAccessDefinitions::Type> &&
            ComponentAccessDefinitions::Filter == Access::Filter::Maybe &&
            ComponentAccessDefinitions::Access == Access::MemoryAccess::Write) || ...)
        T* TryGetComponent()
        {
            return m_Query.GetRegistry()->template TryGetComponent<T>(m_EntityId);
        }

        template <typename T> requires (
            (std::is_same_v<T, typename ComponentAccessDefinitions::Type> &&
            ComponentAccessDefinitions::Filter == Access::Filter::Maybe &&
            ComponentAccessDefinitions::Access == Access::MemoryAccess::Read) || ...)
        const T* TryGetComponent() const
        {
            return m_Query.GetRegistry()->template TryGetComponent<T>(m_EntityId);
        }

        template <typename T> requires (
            (std::is_same_v<T, typename ComponentAccessDefinitions::Type> &&
            ComponentAccessDefinitions::Filter == Access::Filter::Maybe &&
            ComponentAccessDefinitions::Access == Access::MemoryAccess::None) || ...)
        bool HasComponent() const
        {
            return m_Query.GetRegistry()->template HasComponent<T>(m_EntityId);
        }
    private:
        EntityQuery<ComponentAccessDefinitions...>& m_Query;
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
            if constexpr (std::is_base_of_v<IEngineSystem, T>)
            {
                return context.Engine.GetSystem<T>();
            }
            else
            {
                return T{};
            }
        }
    };

    template <typename... ComponentAccessDefinitions>
    class DependentFunctionArg<EntityQuery<ComponentAccessDefinitions...>> : public IDependentFunctionArg
    {
    public:
        static EntityQuery<ComponentAccessDefinitions...> Get(const ECSExecutionContext& context)
        {
            return EntityQuery<ComponentAccessDefinitions...>(context.Registry);
        }
    };

    template <>
    class DependentFunctionArg<const EntityRegistry> : public IDependentFunctionArg
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
                InvokeWithTypeList(TypeList<Args...>{}, [&]<typename... FunctionArg>()
                {
                    callable(EntityQuery<EntityQueryArgs...>(context.Registry), DependentFunctionArg<std::remove_cvref_t<FunctionArg>>::Get(context)...);
                });
            };
        }

        template <typename System, typename... EntityAccessT, typename... Args>
        static auto BuildSystemCall(void(*callable)(Entity<EntityAccessT...>, Args...))
        {
            return [=](const ECSExecutionContext& context)
            {
                const auto& dependencies = ForEachAndCollect<Tuple<Args...>>([&]<typename FunctionArg>() -> FunctionArg
                {
                    return DependentFunctionArg<std::remove_cvref_t<FunctionArg>>::Get(context);
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
                    callable(DependentFunctionArg<std::remove_cvref_t<FunctionArg>>::Get(context)...);
                });
            };
        }


    private:

        EntityRegistry* m_EntityRegistry;
        // TODO: DAG?
        DynamicArray<InplaceFunction<void(const ECSExecutionContext&), 64>> m_Systems;
    };
}

namespace std
{
    template<typename... ComponentAccessDefintions>
    struct tuple_size<Onyx::Entity::Entity<ComponentAccessDefintions...>>
        : std::integral_constant<std::size_t, std::tuple_size_v<typename Onyx::Entity::Entity<ComponentAccessDefintions...>::IncludedMemoryAccessComponents>>
    {
    };

    template<std::size_t I, typename... ComponentAccessDefintions>
    struct tuple_element<I, Onyx::Entity::Entity<ComponentAccessDefintions...>>
    {
        using IncludedMemoryAccessComponents = typename Onyx::Entity::Entity<ComponentAccessDefintions...>::IncludedMemoryAccessComponents;
        using ComponentAccessDefinition = std::tuple_element_t<I, IncludedMemoryAccessComponents>; 

        using type = std::conditional_t<
            ComponentAccessDefinition::Access == Onyx::Entity::Access::MemoryAccess::Write,
            typename ComponentAccessDefinition::Type,
            const typename ComponentAccessDefinition::Type>; 
    };
}