#pragma once

namespace Onyx
{
    class Serializer;
    class Deserializer;

    template <typename T>
    struct Serialization
    {
        static bool Serialize(Serializer&, const T&) = delete;
        static bool Deserialize(const Deserializer&, T&) = delete;
    };

    template<typename T>
    concept Serializable = requires(const T& obj, Serializer& serializer)
    {
        { Serialization<T>::Serialize(serializer, obj) } -> std::same_as<bool>;
    };

    template<typename T>
    concept Deserializable = requires(T& obj, const Deserializer& deserializer)
    {
        { Serialization<T>::Deserialize(deserializer, obj) } -> std::same_as<bool>;
    };

    struct SerializationScope
    {
        SerializationScope(Serializer& serializer, StringView scopeName);
        SerializationScope(Serializer& serializer, onyxU32 scopeIndex);
        ~SerializationScope();

    private:
        Serializer& m_Serializer;
    };
}
