#include <onyx/guid.h>

namespace Onyx
{
    Guid64Generator::Guid64Generator(onyxU64 machineId)
        : m_MachineId(machineId)
        , m_LastTimestamp(GetCurrentTime())
    {}

    Guid64 Guid64Generator::GetGuid()
    {
        static thread_local Guid64Generator gs_GuidGeneratorTLS(1);
        return gs_GuidGeneratorTLS.Next();
    }

    Guid64 Guid64Generator::Next()
    {
        onyxS64 now = 0;
        {
            std::lock_guard lock(m_Mutex);
            now = GetCurrentTime();

            if (now == m_LastTimestamp)
            {
                m_Sequence = (m_Sequence + 1) & MAX_SEQUENCE_ID;

                if (m_Sequence == 0)
                {
                    now = WaitForNextMillisecond();
                }
            }
            else
            {
                m_Sequence = 0;
            }

            m_LastTimestamp = now;
        }

        const onyxU64 id = (now - EPOCH) << TIMESTAMP_LEFT_SHIFT
            | m_MachineId << MACHINE_ID_SHIFT
            | m_Sequence;
        return Guid64{ id };
    }

    onyxS64 Guid64Generator::GetCurrentTime()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }

    onyxS64 Guid64Generator::WaitForNextMillisecond()
    {
        onyxS64 timestamp = GetCurrentTime();

        while (timestamp <= m_LastTimestamp)
            timestamp = GetCurrentTime();

        return timestamp;
    }
}
