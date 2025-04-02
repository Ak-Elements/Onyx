#pragma once

#include <chrono>

namespace Onyx
{
    struct Time
    {
    private:
        template<typename T>
        static onyxU64 GetCurrentTime()
        {
            static std::chrono::time_point<std::chrono::high_resolution_clock> s_Time = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<T>(std::chrono::high_resolution_clock::now() - s_Time).count();
        }

    public:
        static onyxU64 GetCurrentNanoseconds()
        {
            static std::chrono::time_point<std::chrono::high_resolution_clock> s_Time = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - s_Time).count();
            //return GetCurrentTime<std::chrono::nanoseconds>();
        }

        static onyxU64 GetCurrentMilliseconds()
        {
            static std::chrono::time_point<std::chrono::high_resolution_clock> s_Time = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - s_Time).count();
            //return GetCurrentTime<std::chrono::milliseconds>();
        }
    };
}