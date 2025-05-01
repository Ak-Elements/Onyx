#pragma once

#include <onyx/entity/entity.h>

#include <entt/entity/registry.hpp>

#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/resolve.hpp>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/log/logger.h>

namespace Onyx::GameCore
{
    struct TransformComponent;
}

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
            concept HasDrawImGuiEditor = requires(T& component)
            {
                { component.DrawImGuiEditor() } -> std::same_as<void>;
            };

            template<typename T>
            concept HasHideInEditor = requires(T obj) { T::HideInEditor; };

            template<typename T>
            concept IsTransient = requires(T obj) { T::IsTransient; };

            template<typename T>
            concept IsJsonSerializable = requires(const T & obj, FileSystem::JsonValue& outStream)
            {
                { obj.SerializeJson(outStream) } -> std::same_as<void>;
            };

            template<typename T>
            concept IsJsonDeserializable = requires(T& obj, const FileSystem::JsonValue& inStream)
            {
                { obj.DeserializeJson(inStream) } -> std::same_as<void>;
            };

            template <typename T>
            T& CreateComponent(entt::basic_registry<EntityId>& registry, EntityId entityId, const FileSystem::JsonValue& json)
            {
                T newComp;
                newComp.DeserializeJson(json);
                return registry.emplace_or_replace<T>(entityId, newComp);
            }
        }

        inline constexpr onyxU32 SHOW_IN_EDITOR_PROPERTY_HASH = entt::hashed_string("ShowInEditor").value();
        inline constexpr onyxU32 TRANSIENT_PROPERTY_HASH = entt::hashed_string("Transient").value();

        // serialize / deserialize functors
        inline constexpr onyxU32 SERIALIZE_FUNCTION_HASH = entt::hashed_string("Serialize").value();
        inline constexpr onyxU32 SERIALIZE_JSON_FUNCTION_HASH = entt::hashed_string("SerializeJson").value();
        inline constexpr onyxU32 DESERIALIZE_FUNCTION_HASH = entt::hashed_string("Deserialize").value();
        inline constexpr onyxU32 DESERIALIZE_JSON_FUNCTION_HASH = entt::hashed_string("DeserializeJson").value();

        inline constexpr onyxU32 DRAW_IMGUI_FUNCTION_HASH = entt::hashed_string("DrawImGuiEditor").value();

        class EntityRegistry
        {
        public:
            using EntityRegistryT = entt::basic_registry<EntityId>;
            template <typename T> requires HasTypeId<T>
            static void RegisterComponent()
            {
                using namespace entt::literals;
                constexpr StringId32 typeId = T::TypeId;
                auto metaClass = entt::meta<T>();
                auto metaType = metaClass.type(typeId.Id);

                registeredComps[typeId] = T::TypeId.IdString;

                metaClass.template ctor<&EntityRegistryT::emplace_or_replace<T>, entt::as_ref_t>();

                if constexpr (Details::HasHideInEditor<T>)
                    metaType.prop(SHOW_IN_EDITOR_PROPERTY_HASH, false);
                else
                    metaType.prop(SHOW_IN_EDITOR_PROPERTY_HASH, true);

                if constexpr (Details::IsTransient<T>)
                {
                    metaType.prop(TRANSIENT_PROPERTY_HASH, true);
                }
                else
                {
                    static_assert(HasSerialize<T> && HasDeserialize<T>, "Non transient component needs Serialize & Deserialize functions.");
                    metaType.prop(TRANSIENT_PROPERTY_HASH, false);
                    metaClass.template ctor<&Details::CreateComponent<T>, entt::as_ref_t>();
                }

                if constexpr (Details::HasDrawImGuiEditor<T>)
                    metaClass.template func<&T::DrawImGuiEditor>(DRAW_IMGUI_FUNCTION_HASH);
                
                if constexpr (HasSerialize<T>)
                    metaClass.template func<&T::Serialize>(SERIALIZE_FUNCTION_HASH);

                if constexpr (HasDeserialize<T>)
                    metaClass.template func<&T::Deserialize>(DESERIALIZE_FUNCTION_HASH);

                if constexpr (Details::IsJsonSerializable<T>)
                {
                    ONYX_LOG_INFO("Registering json serialize function");
                    metaClass.template func<&T::SerializeJson>(SERIALIZE_JSON_FUNCTION_HASH);
                }
                if constexpr (Details::IsJsonDeserializable<T>)
                    metaClass.template func<&T::DeserializeJson>(DESERIALIZE_JSON_FUNCTION_HASH);
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
             
            template <typename T, typename... Args>
            decltype(auto) AddComponent(EntityId entity, Args&&... args)
            {
                return m_Registry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
            }

            void AddComponent(EntityId entity, onyxU32 componentId, const FileSystem::JsonValue& json)
            {
                std::ignore = entt::resolve(componentId).construct(entt::forward_as_meta(m_Registry), entity, entt::forward_as_meta(json));
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

        private:
            EntityRegistryT m_Registry;
        public:
            static HashMap<StringId32, String> registeredComps;
        };
    }
}