#pragma once

#include <onyx/entity/entityregistry.h>

namespace Onyx::Entity
{
    namespace Details
    {
        template<typename T>
        concept HasHideInEditor = requires(T obj) { T::HideInEditor; };

        template<typename T>
        concept IsTransient = requires(T obj) { T::IsTransient; };

        template <typename T>
        concept HasDrawImGuiEditor = requires(T& component)
        {
            { component.DrawImGuiEditor() } -> std::same_as<bool>;
        };


        template <typename T>
        concept IsFlagComponent = std::is_empty_v<T>;

    }

    class EntityRegistry;

    struct IComponentMeta
    {
        virtual ~IComponentMeta() = default;

        virtual void Create(EntityRegistry& registry, EntityId entity) const = 0;
        virtual void Create(EntityRegistry& registry, EntityId entity, const Deserializer& deserializer) const = 0;
        virtual void Copy(EntityRegistry& registry, EntityId entity, void* componentPtr) const = 0;

        virtual bool HasFactory() const = 0;

        virtual bool Serialize(const void* componentAny, Serializer&) const = 0;
        virtual bool Deserialize(void* componentAny, const Deserializer&) const = 0;

        virtual bool DrawPropertyGridEditor(void* componentAny) const = 0;

        virtual constexpr bool ShowInEditor() const = 0;
        virtual constexpr bool IsTransient() const = 0;
        virtual constexpr bool IsFlag() const = 0;
        virtual constexpr StringId32 GetTypeId() const = 0;
    };

    template <typename T>
    using ComponentFactoryFunction = void(*)(EntityRegistry&, EntityId, T&& component);

    template <typename T> requires std::copy_constructible<T>
    struct ComponentMeta : public IComponentMeta
    {
    public:
        static_assert(Details::IsTransient<T> || Details::IsFlagComponent<T> || (Serializable<T> && Deserializable<T>), "Component needs to be either marked as transient or implement Serialize / Deserialize capabilities.");
        static_assert(Details::HasHideInEditor<T> || Details::HasDrawImGuiEditor<T> || Details::IsFlagComponent<T>, "Component needs to be either marked as hidden or have a PropertyGrid::Draw specialization.");

        ComponentMeta() = default;
        ComponentMeta(ComponentFactoryFunction<T> factory)
            : m_Factory(factory)
        {
        }

        constexpr bool IsTransient() const override { return Details::IsTransient<T>; }
        constexpr bool ShowInEditor() const override { return Details::HasHideInEditor<T> == false; }
        constexpr bool IsFlag() const override { return Details::IsFlagComponent<T>; }

        constexpr StringId32 GetTypeId() const override
        {
            if constexpr (HasTypeId<T>)
            {
                return T::TypeId;
            }
            else
            {
                ONYX_ASSERT(false, "Calling get typeid on a component that has no type id defined");
                return {};
            }
        }

        void Create(EntityRegistry& registry, EntityId entity) const override
        {
            if constexpr (Details::IsFlagComponent<T>)
            {
                registry.AddComponent<T>(entity);
            }
            else if constexpr (Deserializable<T>)
            {
                T component{};
                if (m_Factory)
                {
                    m_Factory(registry, entity, std::move(component));
                }
                else
                {
                    registry.AddComponent<T>(entity, component);
                }
            }
            else
            {
                ONYX_ASSERT(false, "Not supported for component");
            }
        }

        template <typename... Args>
        void Create(EntityRegistry& registry, EntityId entity, Args&&... args) const
        {
            if constexpr (Details::IsFlagComponent<T>)
            {
                registry.AddComponent<T>(entity, std::forward<Args>(args)...);
            }
            else if constexpr (Deserializable<T>)
            {
                T component(std::forward<Args>(args)...);
                if (m_Factory)
                {
                    m_Factory(registry, entity, std::move(component));
                }
                else
                {
                    registry.AddComponent<T>(entity, component);
                }
            }
            else
            {
                ONYX_ASSERT(false, "Not supported for component");
            }
            
        }

        void Create(EntityRegistry& registry, EntityId entity, const Deserializer& deserializer) const override
        {
            if constexpr (Details::IsFlagComponent<T>)
            {
                registry.AddComponent<T>(entity);
            }
            else if constexpr (Deserializable<T>)
            {
                T component{};
                Serialization<T>::Deserialize(deserializer, component);

                if (m_Factory)
                {
                    m_Factory(registry, entity, std::move(component));
                }
                else
                {
                    registry.AddComponent<T>(entity, component);
                }
            }
            else
            {
                ONYX_ASSERT(false, "Not supported for component");
            }
        }

        void Copy(EntityRegistry& registry, EntityId entity, void* componentPtr) const override
        {
            if constexpr (Details::IsFlagComponent<T>)
            {
                registry.AddComponent<T>(entity);
            }
            else
            {
                const T* component = static_cast<const T*>(componentPtr);
                if (m_Factory)
                {
                    T copied = *component;
                    m_Factory(registry, entity, std::move(copied));
                }
                else
                {
                    registry.AddComponent<T>(entity, *component);
                }
            }
        }

        bool HasFactory() const override { return m_Factory != nullptr;  }

        bool Serialize(const void* componentAny, Serializer& serializer) const override
        {
            if constexpr (Serializable<T>)
            {
                const T* component = static_cast<const T*>(componentAny);
                Serialization<T>::Serialize(serializer, *component);
                return true;
            }
            else
            {
                ONYX_ASSERT(false, "Not supported for component");
                return false;
            }
        }

        bool Deserialize(void* componentAny, const Deserializer& deserializer) const override
        {
            if constexpr (Deserializable<T>)
            {
                T* component = static_cast<T*>(componentAny);
                Serialization<T>::Deserialize(deserializer, *component);
                return true;
            }
            else
            {
                ONYX_ASSERT(false, "Not supported for component");
                return false;
            }
        }

        bool DrawPropertyGridEditor(void* componentAny) const override
        {
            if constexpr (Details::HasDrawImGuiEditor<T>)
            {
                T* component = static_cast<T*>(componentAny);
                return component->DrawImGuiEditor();
            }
            else
            {
                ONYX_UNUSED(componentAny);
                return false;
            }
        }

    private:
        StringId32 m_TypeId;
        InplaceFunction<void(EntityRegistry&, EntityId, T&&)> m_Factory;
    };
}
