#pragma once

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
