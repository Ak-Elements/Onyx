#pragma once

namespace Onyx
{
    // RefCounted
    inline RefCounted::~RefCounted() = default;

    inline onyxU32 RefCounted::GetRefCount() const
    {
        return m_RefCount;
    }

    inline void RefCounted::AddReference()
    {
        m_RefCount.fetch_add(1);
    }

    inline bool RefCounted::RemoveReference()
    {
        return (m_RefCount.fetch_sub(1, std::memory_order::memory_order_acq_rel) == 1);
    }

    // Reference
    template <typename T, typename D>
    T* Reference<T, D>::Raw()
    {
        ONYX_ASSERT(IsValid()); return static_cast<T*>(m_Object);
    }

    template <typename T, typename D>
    const T* Reference<T, D>::Raw() const
    {
        ONYX_ASSERT(IsValid()); return 
        static_cast<T*>(m_Object);
    }

    template <typename T, typename D>
    void Reference<T, D>::Reset(T* ptr)
    {
        DecreaseRefCount();
        m_Object = ptr;
        IncreaseRefCount();
    }

    template <typename T, typename D>
    T* Reference<T, D>::Release()
    {
        T* ptr = Raw();
        m_Object = nullptr;
        return ptr;
    }

    template <typename T, typename D>
    bool Reference<T, D>::IsValid()
    {
        return m_Object != nullptr;
    }

    template <typename T, typename D>
    bool Reference<T, D>::IsValid() const
    {
        return m_Object != nullptr;
    }

    template <typename T, typename D>
    template <typename U>
    const U& Reference<T, D>::As() const
    {
        static_assert(std::is_base_of_v<T, U> || std::is_base_of_v<U, T>, "Classes are not convertible or inherited from each other");
        ONYX_ASSERT(IsValid());
        return *static_cast<U*>(m_Object);
    }

    template <typename T, typename D>
    template <typename U>
    U& Reference<T, D>::As()
    {
        static_assert(std::is_base_of_v<T, U> || std::is_base_of_v<U, T>, "Classes are not convertible or inherited from each other");
        ONYX_ASSERT(IsValid());
        return *static_cast<U*>(m_Object);
    }

    template <typename T, typename D>
    template <typename ... Args>
    Reference<T, D> Reference<T, D>::Create(Args&&... args)
    {
        return Reference<T, D>(new T(std::forward<Args>(args)...));
    }

    template <typename T, typename D>
    bool Reference<T, D>::operator==(const Reference<T>& other) const
    {
        return m_Object == other.m_Object;
    }

    template <typename T, typename D>
    bool Reference<T, D>::operator!=(const Reference<T>& other) const
    {
        return !(*this == other);
    }

    template <typename T, typename D>
    constexpr Reference<T, D> Reference<T, D>::Invalid()
    {
        static Reference invalidRef(nullptr);
        return invalidRef;
    }

    template <typename T, typename D>
    void Reference<T, D>::IncreaseRefCount() const
    {
        if (m_Object)
            m_Object->AddReference();
    }

    template <typename T, typename D>
    void Reference<T, D>::DecreaseRefCount() const
    {
        ONYX_ASSERT((m_Object == nullptr) || (m_Object->m_RefCount != 0));
        if (m_Object && m_Object->RemoveReference())
        {
            m_Deleter(std::remove_const_t<T*>(Raw()));
            m_Object = nullptr;
        }
    }
}