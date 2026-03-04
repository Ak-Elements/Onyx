#pragma once

#include <onyx/entity/entity.h>

#include <entt/entity/registry.hpp>

namespace Onyx::Entity
{
    class ComponentFactory;
    
    class EntityRegistry
    {
    public:
        using EntityRegistryT = entt::basic_registry<EntityId>;

        // Both of those should be removed as we only need them because of the component factory dependency
        EntityRegistry() = default;
        EntityRegistry(ComponentFactory& componentFactory);

        EntityRegistry(const EntityRegistry& other);
        EntityRegistry(EntityRegistry&& other);
        
        EntityRegistry& operator=(const EntityRegistry& other);
        EntityRegistry& operator=(EntityRegistry&& other);
        
        //bool IsEmpty() const { return m_Registry.storage() }
        EntityId CreateEntity();
        EntityId CreateEntity(EntityId entity);
        void DeleteEntity(EntityId entity);

        EntityId CopyEntity(EntityId entity);

        template <typename T, typename... Args> requires (std::is_empty_v<T>)
        void AddComponent(EntityId entity)
        {
            ONYX_ASSERT(entity != EntityId::Invalid);
            m_Registry.emplace_or_replace<T>(entity);
        }

        template <typename T, typename... Args> requires (std::is_empty_v<T> == false)
        T& AddComponent(EntityId entity, Args&&... args)
        {
            ONYX_ASSERT(entity != EntityId::Invalid);
            return m_Registry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
        }

        template <typename T>
        void RemoveComponent(EntityId entity)
        {
            ONYX_ASSERT(entity != EntityId::Invalid);
            m_Registry.remove<T>(entity);
        }

        template <typename... Args>
        bool HasComponents(EntityId entity) const
        {
            ONYX_ASSERT(entity != EntityId::Invalid);
            return m_Registry.all_of<Args...>(entity);
        }

        template <typename T>
        T& GetComponent(EntityId entity)
        {
            ONYX_ASSERT(entity != EntityId::Invalid);
            return m_Registry.get<T>(entity);
        }
        
        template <typename T>
        const T& GetComponent(EntityId entity) const
        {
            ONYX_ASSERT(entity != EntityId::Invalid);
            return m_Registry.get<T>(entity);
        }

        template <typename T>
        T* TryGetComponent(EntityId entity)
        {
            ONYX_ASSERT(entity != EntityId::Invalid);
            return m_Registry.try_get<T>(entity);
        }
        
        template <typename T>
        const T* TryGetComponent(EntityId entity) const
        {
            ONYX_ASSERT(entity != EntityId::Invalid);
            return m_Registry.try_get<T>(entity);
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
        ONYX_NO_DISCARD EntityRegistryT::common_type* GetStorage(entt::id_type runtimeTypeId);

        EntityRegistryT& GetRegistry();
        const EntityRegistryT& GetRegistry() const;

        void Copy(EntityRegistry& toRegistry) const;

        void Clear();

    private:
        bool HasEntity(EntityId id) { return m_Registry.valid(id); }

    private:
        EntityRegistryT m_Registry;
        ComponentFactory* m_ComponentFactory = nullptr;
    };
}