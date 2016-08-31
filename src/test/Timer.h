#pragma once

#if defined(_WIN32)

#define NOMINMAX
#include <Windows.h>    // QueryPerformanceCounter

class Timer {
public:
    Timer()
        : _accum(LARGE_INTEGER{{0}}) {}

    void Start() {
        QueryPerformanceCounter(&_start);
    }

    void Stop() {
        LARGE_INTEGER stop;
        QueryPerformanceCounter(&stop);
        _accum.QuadPart += std::max(0ll, (stop.QuadPart - _start.QuadPart));
    }

    void Reset() {
        _accum.QuadPart = 0;
    }

    double Microseconds() const {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return 1.0e6 * double(_accum.QuadPart) / double(freq.QuadPart);
    }

private:
    LARGE_INTEGER _start;
    LARGE_INTEGER _accum;
};

#else // defined(_WIN32)

#include <cstdint>
#include <ctime>

class Timer {
public:
    Timer()
        : _accum(0) {}

    void Start() {
        clock_gettime(CLOCK_MONOTONIC, &_start);
    }

    void Stop() {
        struct timespec stop;
        clock_gettime(CLOCK_MONOTONIC, &stop);
        if (stop.tv_nsec < _start.tv_nsec) {
            _accum += (1000000000ll + stop.tv_nsec - _start.tv_nsec)
                    + 1000000000ll * (stop.tv_sec - _start.tv_sec - 1);
        } else {
            _accum += 1000000000ll * (stop.tv_sec - _start.tv_sec)
                + (stop.tv_nsec - _start.tv_nsec);
        }
    }

    void Reset() {
        _accum = 0;
    }

    double Microseconds() const {
        return _accum * 1e-3;
    }

private:
    struct timespec _start;
    int64_t _accum;
};

#endif // !defined(_WIN32)
