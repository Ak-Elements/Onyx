#pragma once

#include <onyx/assert.h>
#include <onyx/inplacefunction.h>

namespace Onyx
{
	template <typename... Args>
	class EventDispatcher
	{
		using EventHandlerFunction = InplaceFunction<void(Args...)>;
	public:
#if ONYX_IS_DEBUG
		~EventDispatcher()
		{
			ONYX_ASSERT(m_EventHandlers.empty(), "Event Handlers did not get released");
		}
#endif

		void AddDelegate(EventHandlerFunction&& delegate)
		{
			m_EventHandlers.push_back(delegate);
		}

		template<class Obj>
        void AddDelegate(Obj* obj, void(Obj::* memberFunction)(Args...))
		{
			m_EventHandlers.emplace_back(obj, memberFunction);
		}

		void RemoveDelegate(EventHandlerFunction delegate)
		{
			for (auto it = m_EventHandlers.cbegin(); it != m_EventHandlers.cend(); ++it)
			{
				const EventHandlerFunction& handler = *it;
				if (delegate == handler)
				{
					m_EventHandlers.erase(it);
					return;
				}
			}

			ONYX_ASSERT(false, "Failed removing event handler");
		}

		template<class Obj>
		void RemoveDelegate(Obj* obj, void(Obj::* memberFunction)(Args...))
		{
			RemoveDelegate({ obj, memberFunction });
		}

		void RemoveAll()
		{
			m_EventHandlers.clear();
		}

		void operator()(Args... args) const
	    {
			for (const EventHandlerFunction& eventHandler : m_EventHandlers)
			{
				eventHandler(std::forward<Args>(args)...);
			}
	    }

	private:
		DynamicArray<EventHandlerFunction> m_EventHandlers;
	};

#define ONYX_EVENT(name, ...)																\
	private:																				\
	    using name##EventHandler = InplaceFunction<void(__VA_ARGS__)>;						\
	public:																					\
	    void Add##name##Handler(name##EventHandler handler)									\
		{																					\
	        m_##name.AddDelegate(std::forward<name##EventHandler>(handler));				\
	    }																					\
		template<class Obj>																	\
		void Add##name##Handler(Obj* obj, void(Obj::* memberFunction)(__VA_ARGS__))			\
		{																					\
	        m_##name.AddDelegate(obj, memberFunction);										\
	    }																					\
        void Remove##name##Handler(name##EventHandler handler)								\
		{																					\
	        m_##name.RemoveDelegate(std::forward<name##EventHandler>(handler));				\
	    }																					\
		template<class Obj>																	\
		void Remove##name##Handler(Obj* obj, void(Obj::* memberFunction)(__VA_ARGS__))		\
		{																					\
	        m_##name.RemoveDelegate(obj, memberFunction);									\
	    }																					\
                                                                                            \
        void RemoveAll##name##Handlers()													\
        {																					\
            m_##name.RemoveAll();															\
        }																					\
	private:																				\
	    EventDispatcher<__VA_ARGS__> m_##name;												
} 

