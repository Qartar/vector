#pragma once

////////////////////////////////////////////////////////////////////////////////
#if !defined(_MSC_VER)
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

#endif // !defined(_MSC_VER)

////////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER) && defined(_WIN64)
#   define VECTORCALL __vectorcall
#else
#   define VECTORCALL
#endif

////////////////////////////////////////////////////////////////////////////////
#if defined(__GNUC__)
#define UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE __assume(false)
#else
#define UNREACHABLE (void )0
#endif

////////////////////////////////////////////////////////////////////////////////
// Workaround for Clang/C2 which defines _DEBUG and NDEBUG at the same time
#if _DEBUG && NDEBUG
#   undef NDEBUG
#endif
