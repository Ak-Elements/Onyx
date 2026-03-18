#pragma once 


namespace onyx::input_actions
{
    template <typename T>
    class InputTypeFactory
    {
    public:
        struct MetaData
        {
            InplaceFunction<UniquePtr<T>()> CreateFunctor;
            InplaceFunction<UniquePtr<T>(const T&)> CopyFunctor;
            InplaceFunction<bool(Serializer&, const UniquePtr<T>&)> SerializeFunctor;
            InplaceFunction<bool(const Deserializer&, const UniquePtr<T>&)> DeserializeFunctor;

            onyxU32 RuntimeTypeId;
        };

        template <typename U> requires std::is_base_of_v<T, U>
        static void Register()
        {
            s_RegisteredTypes[U::TypeId] = MetaData
            {   
                .CreateFunctor = []() { return MakeUnique<U>(); },
                .CopyFunctor = [](const T& trigger) { return MakeUnique<U>(static_cast<const U&>(trigger)); },
                .SerializeFunctor = [](Serializer& serializer, const UniquePtr<T>& instance)
                {
                    const U* typedInstance = static_cast<const U*>(instance.get());
                    return Serialization<U>::Serialize(serializer, *typedInstance);
                },
                .DeserializeFunctor = [](const Deserializer& deserializer, const UniquePtr<T>& instance)
                {
                    U* typedInstance = static_cast<U*>(instance.get());
                    return Serialization<U>::Deserialize(deserializer, *typedInstance);
                },
                .RuntimeTypeId = TypeHash<U>(),
            };
        }

        static UniquePtr<T> Create(StringId32 typeId)
        {
            return s_RegisteredTypes[typeId].CreateFunctor();
        }

        static UniquePtr<T> Copy(const T& other)
        {
            return s_RegisteredTypes[other.GetTypeId()].CopyFunctor(other);
        }

        static const MetaData& GetMetaData(StringId32 typeId) { ONYX_ASSERT(s_RegisteredTypes.contains(typeId)); return s_RegisteredTypes.at(typeId); }
        static const HashMap<StringId32, MetaData>& GetTypes() { return s_RegisteredTypes; }

    private:
        inline static HashMap<StringId32, MetaData> s_RegisteredTypes;
    };
}