#pragma once
#include "entityregistry.h"

namespace Onyx::FileSystem
{
    struct JsonValue;
}

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

        template<typename T>
        concept IsJsonSerializable = requires(const T& obj, FileSystem::JsonValue & outStream)
        {
            { obj.SerializeJson(outStream) } -> std::same_as<void>;
        };

        template<typename T>
        concept IsJsonDeserializable = requires(T& obj, const FileSystem::JsonValue & inStream)
        {
            { obj.DeserializeJson(inStream) } -> std::same_as<void>;
        };
    }

    struct IComponentMeta
    {
        virtual ~IComponentMeta() = default;

        virtual std::any Create() const = 0;
        virtual std::any Create(const FileSystem::JsonValue&) const = 0;

        virtual bool Serialize(const void* componentAny, FileSystem::JsonValue&) const = 0;
        virtual bool Deserialize(void* componentAny, const FileSystem::JsonValue&) const = 0;

        virtual bool DrawPropertyGridEditor(void* componentAny) const = 0;

        virtual constexpr bool ShowInEditor() const = 0;
        virtual constexpr bool IsTransient() const = 0;
        virtual constexpr StringId32 GetTypeId() const = 0;
    };

    template <typename T>
    struct ComponentMeta : public IComponentMeta
    {
    public:
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

        std::any Create(const FileSystem::JsonValue& json) const override
        {
            if constexpr (Details::IsJsonDeserializable<T>)
            {
                T component;
                component.DeserializeJson(json);
                return component;
            }
            else
            {
                ONYX_ASSERT(false, "Not supported for component");
                return nullptr;
            }
        }

        bool Serialize(const void* componentAny, FileSystem::JsonValue& json) const override
        {
            if constexpr (Details::IsJsonSerializable<T>)
            {
                const T* component = static_cast<const T*>(componentAny);
                component->SerializeJson(json);
                return true;
            }
            else
            {
                ONYX_ASSERT(false, "Not supported for component");
                return false;
            }
        }

        bool Deserialize(void* componentAny, const FileSystem::JsonValue& json) const override
        {
            if constexpr (Details::IsJsonDeserializable<T>)
            {
                T* component = static_cast<T*>(componentAny);
                component->DeserializeJson(json);
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

    class ComponentRegistry
    {
    public:
        template <typename T>
        void Register()
        {
            if (const IComponentMeta* componentMeta = GetComponentMeta<T>().value_or(nullptr))
            {
                //TODO add some sanity check that the components registered are indeed the same
                ONYX_UNUSED(componentMeta);
                return;
            }

            m_ComponentMeta[T::TypeId] = MakeUnique<ComponentMeta<T>>();
        }

        template <typename T>
        Optional<const IComponentMeta*> GetComponentMeta() const
        {
            auto it = m_ComponentMeta.find(T::TypeId);
            if (it == m_ComponentMeta.end())
            {
                return std::nullopt;
            }

            return it->second.get();
        }

        Optional<const IComponentMeta*> GetComponentMeta(StringId32 typeId) const
        {
            auto it = m_ComponentMeta.find(typeId);
            if (it == m_ComponentMeta.end())
            {
                return std::nullopt;
            }

            return it->second.get();
        }

        const HashMap<StringId32, UniquePtr<IComponentMeta>>& GetComponentMeta() const { return m_ComponentMeta; }

    private:
        HashMap<StringId32, UniquePtr<IComponentMeta>> m_ComponentMeta;
    };
}
