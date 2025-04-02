#pragma once

#include <chrono>

#include <onyx/log/logger.h>

namespace Onyx
{
    template <typename UnitT = std::chrono::microseconds>
    struct HighPerformanceTimer
    {
    private:
        using time_point = std::chrono::time_point <std::chrono::high_resolution_clock>;
        using time_unit = typename UnitT::rep;
    public:
        HighPerformanceTimer() = default;

        HighPerformanceTimer(const char* functionName) : m_FunctionName(functionName)
        {
            Start();
        }

        HighPerformanceTimer(const HighPerformanceTimer& other)
            : m_Start(other.m_Start)
            , m_FunctionName(other.m_FunctionName)
        {
        }

        HighPerformanceTimer(HighPerformanceTimer&& other) noexcept
            : m_Start(other.m_Start)
            , m_FunctionName(other.m_FunctionName)
        {
        }

        ~HighPerformanceTimer()
        {
            long long elapsed = Stop();
            ONYX_LOG_INFO("{} in {} milliseconds", m_FunctionName, elapsed / 1000.0);
        }

        void Start()
        {
            using namespace std::chrono;
            m_Start = high_resolution_clock::now();
        }

        time_unit Stop()
        {
            using namespace std::chrono;
            return duration_cast<UnitT>(high_resolution_clock::now() - m_Start).count();
        }

    private:
        time_point m_Start;
        const char* m_FunctionName;
    };

}
