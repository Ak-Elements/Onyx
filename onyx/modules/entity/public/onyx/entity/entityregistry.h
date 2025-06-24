#pragma once

#include <onyx/entity/entity.h>

#include <entt/entity/registry.hpp>

namespace Onyx::Entity
{
    class EntityRegistry
    {
    public:
        using EntityRegistryT = entt::basic_registry<EntityId>;

        EntityId CreateEntity();

        void DeleteEntity(EntityId entityId);

        EntityId CopyEntity(EntityId entity);

        template <typename T, typename... Args> requires (std::is_empty_v<T>)
        void AddComponent(EntityId entity)
        {
            m_Registry.emplace_or_replace<T>(entity);
        }

        template <typename T, typename... Args> requires (std::is_empty_v<T> == false)
        T& AddComponent(EntityId entity, Args&&... args)
        {
            return m_Registry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
        }

        template <typename T>
        void RemoveComponent(EntityId entity)
        {
            m_Registry.remove<T>(entity);
        }

        template <typename... Args>
        bool HasComponents(EntityId entity) const
        {
            return m_Registry.all_of<Args...>(entity);
        }

        template <typename T>
        T& GetComponent(EntityId entity)
        {
            return m_Registry.get<T>(entity);
        }

        template <typename T>
        const T& GetComponent(EntityId entity) const
        {
            return m_Registry.get<T>(entity);
        }

        template <typename Type, typename... Other, typename... Excludes>
        ONYX_NO_DISCARD auto GetView(Excludes&&... excludes)
        {
            return m_Registry.view<Type, Other...>(std::forward<Excludes>(excludes)...);
        }

        template <typename Type, typename... Other, typename... Excludes>
        ONYX_NO_DISCARD auto GetView(Excludes&&... excludes) const
        {
            return m_Registry.view<Type, Other...>(std::forward<Excludes>(excludes)...);
        }

        template<typename... Owned, typename... Get, typename... Exclude>
        ONYX_NO_DISCARD auto GetGroup(entt::get_t<Get...> observed = entt::get_t{}, entt::exclude_t<Exclude...> excludes = entt::exclude_t{})
        {
            return m_Registry.group<Owned...>(std::forward<entt::get_t<Get...>>(observed), std::forward<entt::exclude_t<Exclude...>>(excludes));
        }

        /*template<typename... Owned, typename... Get, typename... Exclude>
        ONYX_NO_DISCARD auto GetGroup(Includes&&... includes) const
        {
            return m_Registry.group<Type, Other...>(std::forward<Includes>(includes)...);
        }*/

        ONYX_NO_DISCARD EntityRegistryT::iterable GetStorage();
        ONYX_NO_DISCARD EntityRegistryT::const_iterable GetStorage() const;

        EntityRegistryT& GetRegistry();
        const EntityRegistryT& GetRegistry() const;

        void Clear();

    private:
        EntityRegistryT m_Registry;
    };
}