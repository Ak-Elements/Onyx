#pragma once

#include <onyx/onyx_types.h>

namespace Onyx
{
	template <typename T, typename D>
	class Reference;

    class RefCounted
    {
		template <typename U, typename D>
        friend class Reference;

    public:
        virtual ~RefCounted();
        onyxU32 GetRefCount() const;

        void AddReference();
        bool RemoveReference();

    private:
        Atomic<onyxU32> m_RefCount = 0;
    };

	// TODO: Add Invalid handle
    template <typename T, typename D = std::default_delete<T>>
	class Reference
	{
		using ReferenceT = T;
		using DeleterT = D;

		template<typename U>
		struct RefCountedBase
		{
			typedef RefCounted Type;
		};

		template<typename U>
		struct RefCountedBase<U*>
		{
			typedef RefCounted* Type;
		};

		template<typename U>
		struct RefCountedBase<const U>
		{
			typedef const RefCounted Type;
		};

		template<typename U>
		struct RefCountedBase<const U*>
		{
			typedef const RefCounted* Type;
		};

		typedef typename RefCountedBase<T>::Type BaseT;

		template<typename U, typename F>
		friend class Reference;

	public:
        constexpr Reference() = default;

        constexpr Reference(std::nullptr_t ptr)
            : m_Object(ptr)
        {
        }

        Reference(T* obj)
            : m_Object(obj)
        {
            IncreaseRefCount();
        }

        Reference(T* obj, DeleterT deleter)
	        : m_Object(obj)
            , m_Deleter(std::move(deleter))
        {
            IncreaseRefCount();
        }

        Reference(const T* obj)
            : m_Object(obj)
        {
            IncreaseRefCount();
        }

        template <typename U>
        Reference(U* obj)
            : Reference(static_cast<T*>(obj))
        {
            static_assert(std::is_base_of_v<T, U> || std::is_base_of_v<U, T>, "Classes are not convertible or inherited from each other");
        }

        template <typename U>
        Reference(const U* obj)
            : Reference(static_cast<T*>(std::remove_const_t<U*>(obj)))
        {
            static_assert(std::is_base_of_v<T, U> || std::is_base_of_v<U, T>, "Classes are not convertible or inherited from each other");
        }

        template <typename U>
        Reference(const Reference<U>& other)
        {
            static_assert(std::is_base_of_v<T, U> || std::is_base_of_v<U, T>, "Classes are not convertible or inherited from each other");
            m_Object = static_cast<T*>(other.m_Object);
            m_Deleter = other.m_Deleter;
            IncreaseRefCount();
        }
        
        template <typename U>
        Reference(Reference<U>&& other)
        {
            // TODO: Move ctor does not seem to work properly
            static_assert(std::is_base_of_v<T, U> || std::is_base_of_v<U, T>, "Classes are not convertible or inherited from each other");
            std::swap(m_Object, other.m_Object);
            //m_Deleter = DeleterT(other.m_Deleter);
            other.m_Object = nullptr;
        }

        ~Reference()
        {
            DecreaseRefCount();
        }

        Reference(const Reference& other)
	        : m_Object(other.m_Object)
            , m_Deleter(other.m_Deleter)
        {
            IncreaseRefCount();
        }

        Reference(Reference&& other) noexcept
            : m_Object(std::move(other.m_Object))
            , m_Deleter(std::move(other.m_Deleter))
        {
            other.m_Object = nullptr;
        }


        Reference& operator=(std::nullptr_t)
        {
            DecreaseRefCount();
            m_Object = nullptr;
            return *this;
        }

        Reference& operator=(const Reference& other)
        {
            if (this == &other)
                return *this;

            other.IncreaseRefCount();
            DecreaseRefCount();

            m_Object = other.m_Object;
            m_Deleter = other.m_Deleter;
            return *this;
        }

        Reference& operator=(Reference&& other) noexcept
        {
            DecreaseRefCount();

            m_Object = std::move(other.m_Object);
            m_Deleter = std::move(other.m_Deleter);

            other.m_Object = nullptr;
            
            return *this;
        }

        template <typename U, typename UDeleter>
        Reference& operator=(const Reference<U, UDeleter>& other)
        {
            static_assert(std::is_base_of_v<T, U> || std::is_base_of_v<U, T>, "Classes are not convertible or inherited from each other");
            other.IncreaseRefCount();
            DecreaseRefCount();

            m_Object = other.m_Object;
            return *this;
        }

        template <typename U, typename UDeleter>
        Reference& operator=(Reference<U, UDeleter>&& other)
        {
            static_assert(std::is_base_of_v<T, U> || std::is_base_of_v<U, T>, "Classes are not convertible or inherited from each other");
            DecreaseRefCount();

            m_Object = std::move(other.m_Object);
            m_Deleter = std::move(other.m_Deleter);

            other.m_Object = nullptr;

            return *this;
        }

        operator bool()
        { 
            return IsValid();
        }

        operator bool() const
        {
            return IsValid();
        }

        T* operator->()
        {
            ONYX_ASSERT(IsValid()); return Raw();
        }

        const T* operator->() const
        {
            ONYX_ASSERT(IsValid()); return Raw();
        }

        T& operator*()
        {
            ONYX_ASSERT(IsValid()); return *Raw();
        }

        const T& operator*() const
        {
            ONYX_ASSERT(IsValid()); return *Raw();
        }

        T* Raw();
        const T* Raw() const;

        void Reset(T* ptr = nullptr);

        T* Release();

        bool IsValid();
        bool IsValid() const;

        template<typename U>
		const U& As() const;

        template<typename U>
		U& As();

        template<typename... Args>
		static Reference Create(Args&&... args);

        bool operator==(const Reference<T>& other) const;
        bool operator!=(const Reference<T>& other) const;

        static constexpr Reference Invalid();

    private:
        void IncreaseRefCount() const;
        void DecreaseRefCount() const;

    private:
        mutable BaseT* m_Object = nullptr;
        // Use EBO to store the deleter
        [[no_unique_address]] DeleterT m_Deleter {};
    };
}

#include <onyx/reference.hpp>