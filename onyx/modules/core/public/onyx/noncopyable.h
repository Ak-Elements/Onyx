#pragma once

namespace Onyx
{
    class NonCopyable
    {
    public:
        NonCopyable() = default;

        NonCopyable(const NonCopyable&) = delete;
        const NonCopyable& operator=(const NonCopyable&) = delete;

        NonCopyable(NonCopyable&&) = default;
        NonCopyable& operator= (NonCopyable&&) = default;

    protected:
        virtual ~NonCopyable() = default;
    };

}