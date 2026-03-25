#pragma once

#if ONYX_ASSERT_ENABLED

#include <onyx/debugging.h>
#include <onyx/inplacefunction.h>
#include <onyx/string/format.h>
#include <onyx/string/inplacestring.h>

namespace onyx {
inline InplaceFunction< void( StringView message ) > ErrorFunctor = []( StringView /*msg*/ ) {};

template < typename... Args > bool Assert( const char* file,
                                           int line,
                                           const char* conditionString,
                                           std::format_string< Args... > fmt,
                                           Args&&... args ) {
    InplaceString< 512 > finalMessage;

    constexpr uint32_t formatArgsCount = sizeof...( args );
    InplaceString< 250 > messageStr;
    if constexpr ( formatArgsCount > 0 ) {
        Format::FormatTo( messageStr, fmt, std::forward< Args >( args )... );
    }

    Format::FormatTo( finalMessage,
                      "{}({}) : Assert failed({}): {} \n",
                      file,
                      line,
                      conditionString,
                      messageStr.GetData() );

    return true;
}

template < typename... Args > bool Assert( const char* file, int line, const char* conditionString ) {
    InplaceString< 512 > finalMessage;
    Format::FormatTo( finalMessage, "{}({}) : Assert failed({}) \n", file, line, conditionString );
    ErrorFunctor( finalMessage.GetData() );
    return true;
}
} // namespace onyx

#define ONYX_ASSERT( condition, ... )                                                                                  \
    do {                                                                                                               \
        if ( !( condition ) ) {                                                                                        \
            if ( ::onyx::Assert( __FILE__, __LINE__, #condition, ##__VA_ARGS__ ) ) {                                   \
                ::onyx::breakpoint();                                                                                  \
                ::std::terminate();                                                                                    \
            }                                                                                                          \
        }                                                                                                              \
    } while ( false )
#else
#define ONYX_ASSERT( condition, ... )
#endif