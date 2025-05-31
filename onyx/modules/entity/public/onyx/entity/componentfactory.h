#pragma once

#include <onyx/entity/componentmeta.h>

namespace Onyx::Entity
{
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
