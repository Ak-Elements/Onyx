#pragma once

#include <onyx/assert.h>

namespace Onyx
{
    enum class LogLevel : onyxU8
    {
        Trace,
        Debug,
        Information,
        Warning,
        Error,
        Fatal,
        Count
    };

    namespace LogPrivate
    {
        // Formatting codes
        static constexpr StringView reset = "\033[m";
        static constexpr StringView bold = "\033[1m";
        static constexpr StringView dark = "\033[2m";
        static constexpr StringView underline = "\033[4m";
        static constexpr StringView blink = "\033[5m";
        static constexpr StringView reverse = "\033[7m";
        static constexpr StringView concealed = "\033[8m";
        static constexpr StringView clear_line = "\033[K";

        // Foreground colors
        static constexpr StringView black = "\033[30m";
        static constexpr StringView red = "\033[31m";
        static constexpr StringView green = "\033[32m";
        static constexpr StringView yellow = "\033[33m";
        static constexpr StringView blue = "\033[34m";
        static constexpr StringView magenta = "\033[35m";
        static constexpr StringView cyan = "\033[36m";
        static constexpr StringView white = "\033[37m";

        /// Bold colors
        static constexpr StringView yellow_bold = "\033[33m\033[1m";
        static constexpr StringView red_bold = "\033[31m\033[1m";
        static constexpr StringView bold_on_red = "\033[1m\033[41m";
    }

    static constexpr StringView GetLogLevelConsoleColor(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Trace: return LogPrivate::white;
        case LogLevel::Debug: return LogPrivate::cyan;
        case LogLevel::Information: return LogPrivate::green;
        case LogLevel::Warning: return LogPrivate::yellow_bold;
        case LogLevel::Error: return LogPrivate::red_bold;
        case LogLevel::Fatal: return LogPrivate::bold_on_red;
        default: ONYX_ASSERT(false, "Invalid log level"); return "";
        }
    }

    static constexpr StringView GetLogLevelName(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Trace: return "Trace";
        case LogLevel::Debug: return "Debug";
        case LogLevel::Information: return "Information";
        case LogLevel::Warning: return "Warning";
        case LogLevel::Error: return "Error";
        case LogLevel::Fatal: return "Fatal";
        default: ONYX_ASSERT(false, "Invalid log level"); return "";
        }
    }
}
