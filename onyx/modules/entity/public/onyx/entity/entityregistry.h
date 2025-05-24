#pragma once

#include <onyx/entity/entity.h>

#include <entt/entity/registry.hpp>

#include <entt/meta/factory.hpp>

#include <onyx/filesystem/onyxfile.h>

#include <onyx/entity/componentfactory.h>

namespace Onyx::FileSystem
{
    struct JsonValue;
}

namespace Onyx
{
    namespace Entity
    {
        namespace Details
        {

            template <typename T>
            T& CreateComponent(entt::basic_registry<EntityId>& registry, EntityId entityId, const FileSystem::JsonValue& json)
            {
                T newComp;
                newComp.DeserializeJson(json);
                return registry.emplace_or_replace<T>(entityId, newComp);
            }

            template <typename T>
            bool SerializeComponent(const void* componentPtr, FileSystem::JsonValue& value)
            {
                const T* component = std::bit_cast<const T*>(componentPtr);
                component->SerializeJson(value);
                return true;
            }

            template <typename T>
            bool DeserializeComponent(void* componentPtr, const FileSystem::JsonValue& value)
            {
                T* component = std::bit_cast<T*>(componentPtr);
                component->DeserializeJson(value);
                return true;
            }

            template <typename T>
            bool DrawPropertyGridEditor(void* componentPtr)
            {
                T* component = std::bit_cast<T*>(componentPtr);
                component->DrawImGuiEditor();
                return true;
            }
        }

        class EntityRegistry
        {
        public:
            using EntityRegistryT = entt::basic_registry<EntityId>;

            template <typename T>
            using FactoryT = void(*)(EntityRegistry&, EntityId, T&& component);

            template <typename T>
            static void Factory()
            {
                m_Factories[T::TypeId] = [](EntityRegistry& registry, EntityId entity, std::any& anyComponent)
                {
                    T component = std::any_cast<T>(anyComponent);
                    registry.GetRegistry().emplace_or_replace<T>(entity, component);
                };
            }

            template <typename T>
            static void Factory(FactoryT<T> factory)
            {
                m_Factories[T::TypeId] = [=](EntityRegistry& registry, EntityId entity, std::any& anyComponent)
                {
                    T component = std::any_cast<T>(anyComponent);
                    factory(registry, entity, std::move(component));
                };
            }

        //private:
            using EntityRegistryT = entt::basic_registry<EntityId>;
            template <typename T>
            static void RegisterComponent()
            {
                using namespace entt::literals;

                //constexpr bool showInEditor = Details::HasHideInEditor<T> == false;
                constexpr bool isTransient = Details::IsTransient<T>;
                if constexpr (isTransient)
                {
                    //add flag components
                }
                else
                {
                    s_Factory.Register<T>();
                    Factory<T>();

                    constexpr auto typeHash = entt::type_hash<T>::value();
                    constexpr StringId32 typeId = T::TypeId;
                    
                    s_RuntimeTypeIdToStaticTypeId[typeHash] = typeId;
                }
            }

            static Optional<const IComponentMeta*> GetComponentMeta(StringId32 typeId)
            {
                return s_Factory.GetComponentMeta(typeId);
            }

            static Optional<const IComponentMeta*> GetComponentMeta(entt::id_type runtimeTypeId)
            {
                auto it = s_RuntimeTypeIdToStaticTypeId.find(runtimeTypeId);
                if (it == s_RuntimeTypeIdToStaticTypeId.end())
                {
                    return std::nullopt;
                }

                return s_Factory.GetComponentMeta(it->second);
            }

            EntityId CreateEntity()
            {
                return m_Registry.create();
            }

            void DeleteEntity(EntityId entityId)
            {
                m_Registry.destroy(entityId);
            }

            EntityId DuplicateEntity(EntityId entity)
            {
                EntityId newEntity = CreateEntity();

                // create a copy of an entity component by component
                for (const auto&& componentStorageIt : GetStorage())
                {
                    if (auto& componentStorage = componentStorageIt.second; componentStorage.contains(entity))
                    {
                        componentStorage.push(newEntity, componentStorage.value(entity));
                    }
                }

                return newEntity;
            }

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

            void AddComponent(EntityId entity, StringId32 componentId, std::any& anyComponent)
            {
                ONYX_ASSERT(m_Factories.contains(componentId));
                const auto& factory = m_Factories.at(componentId);
                factory(*this, entity, anyComponent);
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

            ONYX_NO_DISCARD EntityRegistryT::iterable GetStorage() { return m_Registry.storage(); }
            ONYX_NO_DISCARD EntityRegistryT::const_iterable GetStorage() const { return m_Registry.storage(); }

            EntityRegistryT& GetRegistry() { return m_Registry; }
            const EntityRegistryT& GetRegistry() const { return m_Registry; }

            void Clear() { m_Registry.clear(); }

            const Entity::ComponentRegistry& GetComponentRegistry() { return s_Factory; }

        public:
            static HashMap<entt::id_type, StringId32> s_RuntimeTypeIdToStaticTypeId;

            static HashMap<StringId32, InplaceFunction<void(EntityRegistry&, EntityId, std::any&)>> m_Factories;
            
            EntityRegistryT m_Registry;
            static ComponentRegistry s_Factory;
        };
    }
}