#pragma once

#include <algorithm> // std::max
#include <vector> // std::vector

#define NOMINMAX
#include <Windows.h>    // QueryPerformanceCounter

class Timer {
public:
    Timer()
        : _accum(LARGE_INTEGER{0}) {}

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

void testVectorAdd(std::vector<float> const& data, size_t iterations);
void testVectorSub(std::vector<float> const& data, size_t iterations);
void testScalarMul(std::vector<float> const& data, size_t iterations);
void testScalarDiv(std::vector<float> const& data, size_t iterations);
void testVectorLength(std::vector<float> const& data, size_t iterations);
void testVectorLengthFast(std::vector<float> const& data, size_t iterations);
void testVectorNormalize(std::vector<float> const& data, size_t iterations);
void testVectorNormalizeFast(std::vector<float> const& data, size_t iterations);
void testVectorDot(std::vector<float> const& data, size_t iterations);
void testVectorCross(std::vector<float> const& data, size_t iterations);
void testVectorProject(std::vector<float> const& data, size_t iterations);
void testVectorReject(std::vector<float> const& data, size_t iterations);
void testVectorReflect(std::vector<float> const& data, size_t iterations);
void testMatrixScalar(std::vector<float> const& data, size_t iterations);
void testMatrixVector(std::vector<float> const& data, size_t iterations);
void testMatrixMatrix(std::vector<float> const& data, size_t iterations);
void testHitSphere(std::vector<float> const& data, size_t iterations);
void testHitCapsule(std::vector<float> const& data, size_t iterations);
