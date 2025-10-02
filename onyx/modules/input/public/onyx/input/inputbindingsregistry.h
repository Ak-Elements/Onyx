#pragma once

#include <onyx/input/inputbinding.h>

namespace Onyx::Input
{
    struct InputBinding;

    class InputBindingsRegistry
    {
    public:
        static UniquePtr<InputBinding> CreateBinding(StringId32 bindingTypeId)
        {
            ONYX_ASSERT(ms_RegisteredBindings.contains(bindingTypeId), "Binding is not registered");
            return ms_RegisteredBindings.at(bindingTypeId)(); // call functor to create new node
        }

        static UniquePtr<InputBindingContext> CreateContext(ActionType actionType)
        {
            ONYX_ASSERT(ms_RegisteredBindingContexts.contains(actionType), "Context is not registered");
            return ms_RegisteredBindingContexts.at(actionType)(); // call functor to create new node
        }

        template <typename T> requires std::is_base_of_v<InputBindingContext, T>
        static void RegisterContext(ActionType actionType)
        {
            ONYX_ASSERT(ms_RegisteredBindingContexts.contains(actionType) == false, "Context is already registered.");
            ms_RegisteredBindingContexts[actionType] = []() { return MakeUnique<T>(); };
        }

        template <typename T> requires std::is_base_of_v<InputBinding, T> && HasTypeId<T>
        static void Register(ActionType actionType, StringView name)
        {
            constexpr StringId32 bindingTypeId = T::TypeId;
            ONYX_ASSERT(ms_RegisteredBindings.contains(bindingTypeId) == false, "Binding is already registered.");

            ms_RegisteredBindingsPerActionType[actionType].emplace(bindingTypeId);
            ms_RegisteredBindings[bindingTypeId] = []() { return MakeUnique<T>(); };
            ms_RegisteredBindingNames[bindingTypeId] = name;
        }

        static const HashSet<StringId32>& GetRegisteredBindings(ActionType type)
        {
            ONYX_ASSERT(ms_RegisteredBindingsPerActionType.contains(type), "Action type {} has no bindings registered.", Enums::ToString(type));
            return ms_RegisteredBindingsPerActionType.at(type);
        }

        static StringView GetBindingName(StringId32 bindingTypeId)
        {
            ONYX_ASSERT(ms_RegisteredBindingNames.contains(bindingTypeId), "Binding({}) is not registered.", bindingTypeId);
            return ms_RegisteredBindingNames.at(bindingTypeId);
        }

    protected:
        static HashMap<ActionType, HashSet<StringId32>> ms_RegisteredBindingsPerActionType;
        static HashMap<StringId32, InplaceFunction<UniquePtr<InputBinding>()>> ms_RegisteredBindings;
        static HashMap<ActionType, InplaceFunction<UniquePtr<InputBindingContext>()>> ms_RegisteredBindingContexts;
        static HashMap<StringId32, String> ms_RegisteredBindingNames;
    };

}
