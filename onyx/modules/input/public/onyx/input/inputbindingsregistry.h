#pragma once

#include <onyx/input/inputbinding.h>

namespace Onyx::Input
{
    struct InputBinding;

    class InputBindingsRegistry
    {
    public:
        static UniquePtr<InputBinding> CreateBinding(onyxU32 bindingTypeId)
        {
            ONYX_ASSERT(ms_RegisteredBindings.contains(bindingTypeId), "Binding is not registered");
            return ms_RegisteredBindings.at(bindingTypeId)(); // call functor to create new node
        }

        template <typename T> requires std::is_base_of_v<InputBinding, T>
        static void Register(ActionType actionType, const StringView& name)
        {
            constexpr onyxU32 bindingTypeId = TypeHash<T>();
            ONYX_ASSERT(ms_RegisteredBindings.contains(bindingTypeId) == false, "Binding is already registered.");

            ms_RegisteredBindingsPerActionType[actionType].emplace(bindingTypeId);
            ms_RegisteredBindings[bindingTypeId] = []() { return MakeUnique<T>(); };
            ms_RegisteredBindingNames[bindingTypeId] = name;
        }

        static const HashSet<onyxU32>& GetRegisteredBindings(ActionType type)
        {
            ONYX_ASSERT(ms_RegisteredBindingsPerActionType.contains(type), "Action type {} has no bindings registered.", Enums::ToString(type));
            return ms_RegisteredBindingsPerActionType.at(type);
        }

        static StringView GetBindingName(onyxU32 bindingTypeId) 
        {
            ONYX_ASSERT(ms_RegisteredBindingNames.contains(bindingTypeId), "Binding({}) is not registered.", bindingTypeId);
            return ms_RegisteredBindingNames.at(bindingTypeId);
        }

    protected:
        static HashMap<ActionType, HashSet<onyxU32>> ms_RegisteredBindingsPerActionType;
        static HashMap<onyxU32, InplaceFunction<UniquePtr<InputBinding>()>> ms_RegisteredBindings;
        static HashMap<onyxU32, String> ms_RegisteredBindingNames;
    };

}
