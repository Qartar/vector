#include "Performance.h"

#include "Default.h"
#include "Aligned.h"
#include "Intrinsic.h"

#include "Intersect.h"

template<typename T>
struct hitSphereT {
    static constexpr const char* name = "hitSphere";
    static constexpr const size_t size = 10;

    void operator()(std::vector<float> const& data, size_t iterations) const {

        float const* v = data.data();

        for (size_t ii = 0; ii < iterations; ++ii) {
            Ray<T> ray = {
                { *v++, *v++, *v++, 1.0f },
                { *v++, *v++, *v++, 1.0f },
            };

            Sphere<T> sphere = {
                { *v++, *v++, *v++, 1.0f }, *v++,
            };

            Hit<T> hit;

            hitSphere<T>(ray, sphere, hit);
        }
    }
};

template<typename T>
struct hitCapsuleT {
    static constexpr const char* name = "hitCapsule";
    static constexpr const size_t size = 11;

    void operator()(std::vector<float> const& data, size_t iterations) const {

        float const* v = data.data();

        for (size_t ii = 0; ii < iterations; ++ii) {
            Ray<T> ray = {
                { *v++, *v++, *v++, 1.0f },
                { *v++, *v++, *v++, 1.0f },
            };

            Capsule<T> capsule = {
                { *v++, *v++, *v++, 1.0f },
                { *v++, *v++, *v++, 1.0f },
                *v++,
            };

            Hit<T> hit;

            hitCapsule<T>(ray, capsule, hit);
        }
    }
};

template<typename Func>
double testPerformanceSingle(std::vector<float> const& data, size_t iterations) {
    Timer t;

    t.Start();
    Func()(data, iterations);
    t.Stop();
    return t.Microseconds();
}

template<template<typename> typename Func, size_t kLoopCount = 16>
void testPerformance(std::vector<float> const& data, size_t iterations) {
    double loop_timing[3][kLoopCount];
    double timing[3];

    // Warm-up passes
    for (size_t ii = 0; ii < 4; ++ii) {
        testPerformanceSingle<Func<default::Vector>>(data, iterations);
        testPerformanceSingle<Func<aligned::Vector>>(data, iterations);
        testPerformanceSingle<Func<intrinsic::Vector>>(data, iterations);
    }

    // Measured passes
    for (size_t ii = 0; ii < kLoopCount; ++ii) {
        loop_timing[0][ii] = testPerformanceSingle<Func<default::Vector>>(data, iterations);
        loop_timing[1][ii] = testPerformanceSingle<Func<aligned::Vector>>(data, iterations);
        loop_timing[2][ii] = testPerformanceSingle<Func<intrinsic::Vector>>(data, iterations);
    }

    // Sort passes
    std::sort(&loop_timing[0][0], &loop_timing[0][kLoopCount]);
    std::sort(&loop_timing[1][0], &loop_timing[1][kLoopCount]);
    std::sort(&loop_timing[2][0], &loop_timing[2][kLoopCount]);

    // Select median
    timing[0] = loop_timing[0][kLoopCount/2];
    timing[1] = loop_timing[1][kLoopCount/2];
    timing[2] = loop_timing[2][kLoopCount/2];

    // Print results
    printf_s("  %-16s %12.4f \xc2\xb5s %12.4f \xc2\xb5s %12.4f \xc2\xb5s %7.2f%% %7.2f%%\n",
             Func<default::Vector>::name,
             timing[0], timing[1], timing[2],
             1.0e2 * timing[0] / timing[1],
             1.0e2 * timing[0] / timing[2]);
}

void testHitSphere(std::vector<float> const& data, size_t iterations) {
    return testPerformance<hitSphereT>(data, iterations);
}

void testHitCapsule(std::vector<float> const& data, size_t iterations) {
    return testPerformance<hitCapsuleT>(data, iterations);
}
