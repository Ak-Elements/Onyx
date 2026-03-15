#pragma once

#include <onyx/entity/dependantfunctionargument.h>
#include <onyx/entity/entityregistry.h>
#include <onyx/entity/ecsexecutioncontext.h>

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
    private:
        template<typename... Included, typename... Excluded>
        static consteval auto MakeViewType(std::tuple<Included...>, std::tuple<Excluded...>)
        {
            return entt::basic_view<entt::get_t<EntityRegistry::EntityRegistryT::storage_for_type<
                std::conditional_t<Included::Access == Access::MemoryAccess::Write, typename Included::Type, const typename Included::Type>>...>,
                entt::exclude_t<typename Excluded::Type...>>();
        }

    public:
        using IncludedComponents = decltype(std::tuple_cat(
            std::conditional_t< ( ComponentAccessDefinitions::Filter == Access::Filter::Include ),
                std::tuple<ComponentAccessDefinitions>, std::tuple<>>{}...));

        using ExcludedComponents = decltype(std::tuple_cat(
            std::conditional_t< ( ComponentAccessDefinitions::Filter == Access::Filter::Exclude ),
                std::tuple<ComponentAccessDefinitions>, std::tuple<>>{}...));

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
                (entt::exclude_t<typename Excluded::Type...>{});
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

    template <typename... ComponentAccessDefinitions>
    class DependantFunctionArg<EntityQuery<ComponentAccessDefinitions...>>
    {
    public:
        static EntityQuery<ComponentAccessDefinitions...> Get(const ECSExecutionContext& context)
        {
            return EntityQuery<ComponentAccessDefinitions...>(context.Registry);
        }
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