#pragma once

#include <onyx/input/triggers/inputtrigger.h>

namespace Onyx::Input
{
    class InputTriggersFactory
    {
    public:
        struct MetaData
        {
            InplaceFunction<UniquePtr<InputTrigger>()> CreateFunctor;
            InplaceFunction<bool(Serializer&, const UniquePtr<InputTrigger>&)> SerializeFunctor;
            InplaceFunction<bool(const Deserializer&, const UniquePtr<InputTrigger>&)> DeserializeFunctor;
        };

        template <typename T> requires std::is_base_of_v<InputTrigger, T>
        static void Register()
        {
            s_RegisteredTriggers[T::TypeId] = MetaData
            {
                .CreateFunctor = []() { return MakeUnique<T>(); },
                .SerializeFunctor = [](Serializer& serializer, const UniquePtr<InputTrigger>& instance)
                {
                    const T* typedInstance = static_cast<const T*>(instance.get());
                    return Serialization<T>::Serialize(serializer, *typedInstance);
                },
                .DeserializeFunctor = [](const Deserializer& deserializer, const UniquePtr<InputTrigger>& instance)
                {
                    T* typedInstance = static_cast<T*>(instance.get());
                    return Serialization<T>::Deserialize(deserializer, *typedInstance);
                },
            };
        }

        static UniquePtr<InputTrigger> Create(StringId32 typeId)
        {
            return s_RegisteredTriggers[typeId].CreateFunctor();
        }

        static const MetaData& GetBindingMeta(StringId32 typeId) { ONYX_ASSERT(s_RegisteredTriggers.contains(typeId)); return s_RegisteredTriggers.at(typeId); }
        static const HashMap<StringId32, MetaData>& GetTriggersMeta() { return s_RegisteredTriggers; }

    private:
        static HashMap<StringId32, MetaData> s_RegisteredTriggers;
    };
}
