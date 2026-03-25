#pragma once

#include <onyx/assert.h>

namespace onyx {
enum class LogLevel : uint8_t { Trace, Debug, Information, Warning, Error, Fatal, Count };

namespace log_private {
// Formatting codes
static constexpr StringView Reset = "\033[m";
static constexpr StringView Bold = "\033[1m";
static constexpr StringView Dark = "\033[2m";
static constexpr StringView Underline = "\033[4m";
static constexpr StringView Blink = "\033[5m";
static constexpr StringView Reverse = "\033[7m";
static constexpr StringView Concealed = "\033[8m";
static constexpr StringView ClearLine = "\033[K";

// Foreground colors
static constexpr StringView Black = "\033[30m";
static constexpr StringView Red = "\033[31m";
static constexpr StringView Green = "\033[32m";
static constexpr StringView Yellow = "\033[33m";
static constexpr StringView Blue = "\033[34m";
static constexpr StringView Magenta = "\033[35m";
static constexpr StringView Cyan = "\033[36m";
static constexpr StringView White = "\033[37m";

/// Bold colors
static constexpr StringView YellowBold = "\033[33m\033[1m";
static constexpr StringView RedBold = "\033[31m\033[1m";
static constexpr StringView BoldOnRed = "\033[1m\033[41m";
} // namespace log_private

static constexpr StringView getLogLevelConsoleColor( LogLevel level ) {
    switch ( level ) {
    case LogLevel::Trace:
        return log_private::White;
    case LogLevel::Debug:
        return log_private::Cyan;
    case LogLevel::Information:
        return log_private::Green;
    case LogLevel::Warning:
        return log_private::YellowBold;
    case LogLevel::Error:
        return log_private::RedBold;
    case LogLevel::Fatal:
        return log_private::BoldOnRed;
    default:
        ONYX_ASSERT( false, "Invalid log level" );
        return "";
    }
}

static constexpr StringView getLogLevelName( LogLevel level ) {
    switch ( level ) {
    case LogLevel::Trace:
        return "Trace";
    case LogLevel::Debug:
        return "Debug";
    case LogLevel::Information:
        return "Information";
    case LogLevel::Warning:
        return "Warning";
    case LogLevel::Error:
        return "Error";
    case LogLevel::Fatal:
        return "Fatal";
    default:
        ONYX_ASSERT( false, "Invalid log level" );
        return "";
    }
}
} // namespace onyx
