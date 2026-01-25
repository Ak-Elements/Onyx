#pragma once

#include <onyx/inputactions/modifiers/inputmodifier.h>

namespace Onyx::InputActions
{
    class InputModifiersFactory
    {
    public:
        struct MetaData
        {
            InplaceFunction<UniquePtr<InputModifier>()> CreateFunctor;
            InplaceFunction<bool(Serializer&, const UniquePtr<InputModifier>&)> SerializeFunctor;
            InplaceFunction<bool(const Deserializer&, UniquePtr<InputModifier>&)> DeserializeFunctor;
        };

        template <typename T> requires std::is_base_of_v<InputModifier, T>
        static void Register()
        {
            s_RegisteredModifiers[T::TypeId] = MetaData
            {
                .CreateFunctor = []() { return MakeUnique<T>(); },
                .SerializeFunctor = [](Serializer& serializer, const UniquePtr<InputModifier>& instance)
                {
                    const T* typedInstance = static_cast<const T*>(instance.get());
                    return Serialization<T>::Serialize(serializer, *typedInstance);
                },
                .DeserializeFunctor = [](const Deserializer& deserializer, UniquePtr<InputModifier>& instance)
                {
                    T* typedInstance = static_cast<T*>(instance.get());
                    return Serialization<T>::Deserialize(deserializer, *typedInstance);
                },
            };
        }

        static UniquePtr<InputModifier> Create(StringId32 typeId)
        {
            return s_RegisteredModifiers[typeId].CreateFunctor();
        }

        static const MetaData& GetBindingMeta(StringId32 typeId) { ONYX_ASSERT(s_RegisteredModifiers.contains(typeId)); return s_RegisteredModifiers.at(typeId); }
        static const HashMap<StringId32, MetaData>& GetModifiersMeta() { return s_RegisteredModifiers; }

    private:
        static HashMap<StringId32, MetaData> s_RegisteredModifiers;
    };
}
