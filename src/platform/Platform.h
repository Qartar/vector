#pragma once

////////////////////////////////////////////////////////////////////////////////
#if !defined(_WIN32)
#include <cstdarg>
#include <cstdio>

__attribute__((__format__(__printf__, 1, 2)))
inline void printf_s(char const* fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    vprintf(fmt, va);
    va_end(va);
}

#endif // !defined(_WIN32)

////////////////////////////////////////////////////////////////////////////////
#if defined(_WIN64)
#   define VECTORCALL __vectorcall
#else
#   define VECTORCALL
#endif

////////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
#define UNREACHABLE __assume(false)
#elif defined(__GNUC__)
#define UNREACHABLE __builtin_unreachable()
#else
#define UNREACHABLE (void )0
#endif
