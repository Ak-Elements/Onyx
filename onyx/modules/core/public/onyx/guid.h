#pragma once
#include <mutex>

#include <onyx/serialize/serialization.h>

namespace onyx {
class Serializer;

struct Guid {
    uint64_t LowPart = 0;
    uint64_t HighPart = 0;

    bool operator==( const Guid& other ) const { return LowPart == other.LowPart && HighPart == other.HighPart; }
    bool operator!=( const Guid& other ) const { return LowPart != other.LowPart || HighPart != other.HighPart; }
};

/*
 * Based on Snowflake algorithm from Twitter
 */
struct Guid64 {
    Guid64() = default;
    explicit Guid64( uint64_t id )
        : m_id( id ) {}

    explicit operator uint64_t() const { return m_id; }

    void reset() { m_id = 0; }
    ONYX_NO_DISCARD uint64_t get() const { return m_id; }

    ONYX_NO_DISCARD bool isValid() const { return m_id != 0; }

    bool operator==( uint64_t other ) const { return m_id == other; }
    bool operator!=( uint64_t other ) const { return m_id != other; }

    bool operator==( const Guid64& other ) const { return m_id == other.m_id; }
    bool operator!=( const Guid64& other ) const { return m_id != other.m_id; }

  private:
    uint64_t m_id = 0;
};

inline constexpr Guid64 InvalidGuiD64 = {};

struct Guid64Generator {
  private:
    static constexpr uint64_t Epoch = 1534832906275L;
    static constexpr uint64_t MachineIdBits = 10;
    static constexpr uint64_t SequenceBits = 12;
    static constexpr uint64_t MaxMachineId = ( 1 << MachineIdBits ) - 1;
    static constexpr uint64_t MaxSequenceId = ( 1 << SequenceBits ) - 1;

    static constexpr uint64_t MachineIdShift = SequenceBits;
    static constexpr uint64_t TimestampLeftShift = SequenceBits + MachineIdBits;

  public:
    static Guid64 getGuid();

    explicit Guid64Generator( uint64_t machineId );

  private:
    Guid64 next();
    int64_t getCurrentTime();
    int64_t waitForNextMillisecond();

  private:
    std::mutex m_mutex;
    uint64_t m_machineId = 0;
    uint64_t m_sequence = 0;
    int64_t m_lastTimestamp = 0;
};

template < typename T >
constexpr bool IsGuid64 = std::is_same_v< Guid64, T >;

template <>
struct Serialization< Guid64 > {
    static bool serialize( Serializer& serializer, const Guid64& id );
    static bool deserialize( const Deserializer& deserializer, Guid64& outId );
};
} // namespace onyx

namespace std {
template <>
struct hash< onyx::Guid64 > {
    size_t operator()( const onyx::Guid64& guid ) const noexcept { return guid.get(); }
};
} // namespace std
