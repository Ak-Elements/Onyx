#pragma once

namespace onyx {
class Serializer;
class Deserializer;

template < typename T >
struct Serialization {
    static bool serialize( Serializer&, const T& ) = delete;
    static bool deserialize( const Deserializer&, T& ) = delete;
};

template < typename T >
concept Serializable = requires( const T& obj, Serializer& serializer ) {
    { Serialization< T >::serialize( serializer, obj ) } -> std::same_as< bool >;
};

template < typename T >
concept Deserializable = requires( T& obj, const Deserializer& deserializer ) {
    { Serialization< T >::deserialize( deserializer, obj ) } -> std::same_as< bool >;
};

struct SerializationScope {
    SerializationScope( Serializer& serializer, StringView scopeName );
    SerializationScope( Serializer& serializer, uint32_t scopeIndex );
    ~SerializationScope();

  private:
    Serializer& m_serializer;
};
} // namespace onyx
