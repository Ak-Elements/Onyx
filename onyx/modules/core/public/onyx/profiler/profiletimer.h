#pragma once

#include <chrono>
#include <onyx/log/logger.h>

namespace onyx {
template < typename UnitT = std::chrono::microseconds >
struct HighPerformanceTimer {
  private:
    using TimePoint = std::chrono::time_point< std::chrono::high_resolution_clock >;
    using TimeUnit = typename UnitT::rep;

  public:
    HighPerformanceTimer() = default;

    explicit HighPerformanceTimer( const char* functionName )
        : m_functionName( functionName ) {
        start();
    }

    HighPerformanceTimer( const HighPerformanceTimer& other ) = default;
    HighPerformanceTimer( HighPerformanceTimer&& other ) noexcept
        : m_start( other.m_start )
        , m_functionName( other.m_functionName ) {}

    ~HighPerformanceTimer() {
        long long elapsed = stop();
        ONYX_LOG_INFO( "{} in {} milliseconds", m_functionName, elapsed / 1000.0 );
    }

    void start() {
        using namespace std::chrono;
        m_start = high_resolution_clock::now();
    }

    TimeUnit stop() {
        using namespace std::chrono;
        return duration_cast< UnitT >( high_resolution_clock::now() - m_start ).count();
    }

  private:
    TimePoint m_start;
    const char* m_functionName = nullptr;
};

} // namespace onyx
