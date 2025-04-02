#pragma once
#include <mutex>

namespace Onyx
{
    struct Guid
    {
        onyxU64 LowPart = 0;
        onyxU64 HighPart = 0;

        bool operator==(const Guid& other) const { return LowPart == other.LowPart && HighPart == other.HighPart; }
        bool operator!=(const Guid& other) const { return LowPart != other.LowPart || HighPart != other.HighPart; }
    };

    /*
     * Based on Snowflake algorithm from Twitter
     */
    struct Guid64
    {
        Guid64() = default;
        explicit Guid64(onyxU64 id)
            : m_Id(id)
        {
        }

        operator onyxU64() const { return m_Id; }

        void Reset() { m_Id = 0; }
        onyxU64 Get() const { return m_Id; }

        bool IsValid() const { return m_Id != 0; }

        bool operator==(onyxU64 other) const { return m_Id == other; }
        bool operator!=(onyxU64 other) const { return m_Id != other; }

        bool operator==(const Guid64& other) const { return m_Id == other.m_Id; }
        bool operator!=(const Guid64& other) const { return m_Id != other.m_Id; }
    private:
        onyxU64 m_Id = 0;
    };

    inline constexpr Guid64 INVALID_GUID64 = {};

    struct Guid64Generator
    {
    private:
        static constexpr onyxU64 EPOCH = 1534832906275L;
        static constexpr onyxU64 MACHINE_ID_BITS = 10;
        static constexpr onyxU64 SEQUENCE_BITS = 12;
        static constexpr onyxU64 MAX_MACHINE_ID = (1 << MACHINE_ID_BITS) - 1;
        static constexpr onyxU64 MAX_SEQUENCE_ID = (1 << SEQUENCE_BITS) - 1;

        static constexpr onyxU64 MACHINE_ID_SHIFT = SEQUENCE_BITS;
        static constexpr onyxU64 TIMESTAMP_LEFT_SHIFT = SEQUENCE_BITS + MACHINE_ID_BITS;


    public:
        static Guid64 GetGuid();

        explicit Guid64Generator(onyxU64 machineId);

    private:
        Guid64 Next();
        onyxS64 GetCurrentTime();
        onyxS64 WaitForNextMillisecond();

    private:
        std::mutex m_Mutex;
        onyxU64 m_MachineId;
        onyxU64 m_Sequence;
        onyxS64 m_LastTimestamp;
    };
}

namespace std
{
    template<>
    struct hash<Onyx::Guid64>
    {
        size_t operator()(const Onyx::Guid64& guid) const noexcept
        {
            return guid.Get();
        }
    };
}
