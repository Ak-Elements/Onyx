#include <onyx/guid.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx {
Guid64Generator::Guid64Generator( uint64_t machineId )
    : m_machineId( machineId )
    , m_lastTimestamp( getCurrentTime() ) {}

Guid64 Guid64Generator::getGuid() {
    static thread_local Guid64Generator GuidGeneratorTls( 1 );
    return GuidGeneratorTls.next();
}

Guid64 Guid64Generator::next() {
    int64_t now = 0;
    {
        std::lock_guard lock( m_mutex );
        now = getCurrentTime();

        if ( now == m_lastTimestamp ) {
            m_sequence = ( m_sequence + 1 ) & MaxSequenceId;

            if ( m_sequence == 0 ) {
                now = waitForNextMillisecond();
            }
        } else {
            m_sequence = 0;
        }

        m_lastTimestamp = now;
    }

    const uint64_t id = ( now - Epoch ) << TimestampLeftShift | m_machineId << MachineIdShift | m_sequence;
    return Guid64{ id };
}

int64_t Guid64Generator::getCurrentTime() {
    return std::chrono::duration_cast< std::chrono::milliseconds >(
               std::chrono::system_clock::now().time_since_epoch() )
        .count();
}

int64_t Guid64Generator::waitForNextMillisecond() {
    int64_t timestamp = getCurrentTime();

    while ( timestamp <= m_lastTimestamp )
        timestamp = getCurrentTime();

    return timestamp;
}

bool Serialization< Guid64 >::serialize( Serializer& serializer, const Guid64& id ) {
    return serializer.write( id.get(), 16 );
}

bool Serialization< Guid64 >::deserialize( const Deserializer& deserializer, Guid64& outId ) {
    uint64_t id;
    bool success = deserializer.read( id, 16 );
    outId = Guid64( id );
    return success;
}
} // namespace onyx
