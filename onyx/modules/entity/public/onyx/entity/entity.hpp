#pragma once

namespace Onyx::Entity
{
    template <typename T>
    T& Entity::AddComponent()
    {
        return m_Registry->AddComponent<T>(m_EntityId);
    }

    template <typename T>
    T& Entity::GetComponent()
    {
        return m_Registry->GetComponent<T>(m_EntityId);
    }

    template <typename T>
    const T& Entity::GetComponent() const
    {
        return m_Registry->GetComponent<T>(m_EntityId);
    }
}