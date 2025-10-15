#pragma once

#include <onyx/input/bindings/inputbinding.h>

namespace Onyx::Input
{
    class InputBindingsFactory
    {
    public:
        struct MetaData
        {
            InplaceFunction<UniquePtr<InputBinding>()> CreateFunctor;
            InplaceFunction<bool(Serializer&, const UniquePtr<InputBinding>&)> SerializeFunctor;
            InplaceFunction<bool(const Deserializer&, const UniquePtr<InputBinding>&)> DeserializeFunctor;
        };

        template <typename T> requires std::is_base_of_v<InputBinding, T>
        static void Register()
        {
            MetaData metaData;
            
            metaData.CreateFunctor = []() { return MakeUnique<T>(); };
            metaData.SerializeFunctor = [](Serializer& serializer, const UniquePtr<InputBinding>& instance)
            {
                const InputBinding& binding = *instance;
                const T& typedInstance = static_cast<const T&>(binding);
                return Serialization<T>::Serialize(serializer, typedInstance);
            };
            metaData.DeserializeFunctor = [](const Deserializer& deserializer, const UniquePtr<InputBinding>& instance)
            {
                InputBinding& binding = *instance;
                T& typedInstance = static_cast<T&>(binding);
                return Serialization<T>::Deserialize(deserializer, typedInstance);
            };

            s_RegisteredBindings[T::TypeId] = metaData;
        }

        static UniquePtr<InputBinding> Create(StringId32 typeId)
        {
            return s_RegisteredBindings[typeId].CreateFunctor();
        }

        static const MetaData& GetBindingMeta(StringId32 typeId) { ONYX_ASSERT(s_RegisteredBindings.contains(typeId)); return s_RegisteredBindings.at(typeId); }
        static const HashMap<StringId32, MetaData>& GetBindingsMeta() { return s_RegisteredBindings; }

    private:
        static HashMap<StringId32, MetaData> s_RegisteredBindings;
    };

}
