#pragma once

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
            { component.DrawImGuiEditor() } -> std::same_as<void>;
        };

    }

    struct IComponentMeta
    {
        virtual ~IComponentMeta() = default;

        virtual std::any Create() const = 0;
        virtual std::any Create(const Deserializer&) const = 0;

        virtual bool Serialize(const void* componentAny, Serializer&) const = 0;
        virtual bool Deserialize(void* componentAny, const Deserializer&) const = 0;

        virtual bool DrawPropertyGridEditor(void* componentAny) const = 0;

        virtual constexpr bool ShowInEditor() const = 0;
        virtual constexpr bool IsTransient() const = 0;
        virtual constexpr StringId32 GetTypeId() const = 0;
    };

    template <typename T>
    struct ComponentMeta : public IComponentMeta
    {
    public:
        static_assert(Details::IsTransient<T> || (Serializable<T> && Deserializable<T>), "Component needs to be either marked as transient or implement Serialize / Deserialize capabilities.");

        constexpr bool IsTransient() const override { return Details::IsTransient<T>; }
        constexpr bool ShowInEditor() const override { return Details::HasHideInEditor<T> == false; }
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

        std::any Create() const override
        {
            return T{};
        }

        
        std::any Create(const Deserializer& deserializer) const override
        {
            if constexpr (Deserializable<T>)
            {
                T component;
                Serialization<T>::Deserialize(deserializer, component);
                return component;
            }
            else
            {
                ONYX_ASSERT(false, "Not supported for component");
                return nullptr;
            }
        }

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
                component->DrawImGuiEditor();
            }
            else
            {
                ONYX_UNUSED(componentAny);
            }

            return true;
        }

        StringId32 TypeId;
    };
}
