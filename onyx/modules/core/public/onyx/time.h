#pragma once

#include <chrono>

namespace onyx {
struct Time {
  private:
    template < typename T >
    static uint64_t GetCurrentTime() {
        static std::chrono::time_point< std::chrono::high_resolution_clock >
            s_Time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast< T >( std::chrono::high_resolution_clock::now() - s_Time ).count();
    }

  public:
    static uint64_t GetCurrentNanoseconds() {
        static std::chrono::time_point< std::chrono::high_resolution_clock >
            s_Time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast< std::chrono::nanoseconds >( std::chrono::high_resolution_clock::now() -
                                                                       s_Time )
            .count();
        // return GetCurrentTime<std::chrono::nanoseconds>();
    }

    static uint64_t GetCurrentMilliseconds() {
        static std::chrono::time_point< std::chrono::high_resolution_clock >
            s_Time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::high_resolution_clock::now() -
                                                                        s_Time )
            .count();
        // return GetCurrentTime<std::chrono::milliseconds>();
    }
};
} // namespace onyx