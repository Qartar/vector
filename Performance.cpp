#include "Performance.h"

#include "Default.h"
#include "Aligned.h"
#include "Intrinsic.h"

#include "Intersect.h"

template<typename T>
struct vectorAddT {
    static constexpr const char* name = "vectorAdd";
    static constexpr const size_t size = 8;

    struct Args {
        T a;
        T b;
    };

    vectorAddT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii].a = { *v++, *v++, *v++, *v++ };
            _input[ii].b = { *v++, *v++, *v++, *v++ };
        }
        _output.resize(data.size() / size);
    }

    void operator()(size_t iterations) {
        Args const* in = _input.data();
        T* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in, ++out) {
            *out = in->a + in->b;
        }
    }

    std::vector<Args> _input;
    std::vector<T> _output;
};

template<typename T>
struct vectorDotT {
    static constexpr const char* name = "vectorDot";
    static constexpr const size_t size = 8;

    struct Args {
        T a;
        T b;
    };

    vectorDotT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii].a = { *v++, *v++, *v++, *v++ };
            _input[ii].b = { *v++, *v++, *v++, *v++ };
        }
        _output.resize(data.size() / size);
    }

    void operator()(size_t iterations) {
        Args const* in = _input.data();
        float* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in, ++out) {
            *out = in->a * in->b;
        }
    }

    std::vector<Args> _input;
    std::vector<float> _output;
};

template<typename T>
struct vectorCrossT {
    static constexpr const char* name = "vectorCross";
    static constexpr const size_t size = 6;

    struct Args {
        T a;
        T b;
    };

    vectorCrossT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii].a = { *v++, *v++, *v++, 0.0f };
            _input[ii].b = { *v++, *v++, *v++, 0.0f };
        }
        _output.resize(data.size() / size);
    }

    void operator()(size_t iterations) {
        Args const* in = _input.data();
        T* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in, ++out) {
            *out = in->a % in->b;
        }
    }

    std::vector<Args> _input;
    std::vector<T> _output;
};

template<typename T>
struct hitSphereT {
    static constexpr const char* name = "hitSphere";
    static constexpr const size_t size = 10;

    struct Args {
        Ray<T> ray;
        Sphere<T> sphere;
    };

    hitSphereT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii].ray = {
                { *v++, *v++, *v++, 1.0f },
                { *v++, *v++, *v++, 1.0f },
            };
            _input[ii].sphere = {
                { *v++, *v++, *v++, 1.0f }, *v++,
            };
        }
        _output.resize(data.size() / size);
    }

    void operator()(size_t iterations) {
        Args const* in = _input.data();
        Hit<T>* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in) {
            hitSphere<T>(in->ray, in->sphere, *out++);
        }
    }

    std::vector<Args> _input;
    std::vector<Hit<T>> _output;
};

template<typename T>
struct hitCapsuleT {
    static constexpr const char* name = "hitCapsule";
    static constexpr const size_t size = 13;

    struct Args {
        Ray<T> ray;
        Capsule<T> capsule;
    };

    hitCapsuleT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii].ray = {
                { *v++, *v++, *v++, 1.0f },
                { *v++, *v++, *v++, 1.0f },
            };
            _input[ii].capsule = {
                { *v++, *v++, *v++, 1.0f },
                { *v++, *v++, *v++, 1.0f },
                *v++,
            };
        }
        _output.resize(data.size() / size);
    }

    void operator()(size_t iterations) {
        Args const* in = _input.data();
        Hit<T>* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in) {
            hitCapsule<T>(in->ray, in->capsule, *out++);
        }
    }

    std::vector<Args> _input;
    std::vector<Hit<T>> _output;
};

template<typename Func>
double testPerformanceSingle(Func& fn, size_t iterations) {
    Timer t;

    t.Start();
    fn(iterations);
    t.Stop();
    return t.Microseconds();
}

template<template<typename> typename Func, size_t kLoopCount = 16>
void testPerformance(std::vector<float> const& data, size_t iterations) {
    double loop_timing[3][kLoopCount];
    double timing[3];

    Func<default::Vector> fn0(data);
    Func<aligned::Vector> fn1(data);
    Func<intrinsic::Vector> fn2(data);

    // Warm-up passes
    for (size_t ii = 0; ii < 4; ++ii) {
        testPerformanceSingle(fn0, iterations);
        testPerformanceSingle(fn1, iterations);
        testPerformanceSingle(fn2, iterations);
    }

    // Measured passes
    for (size_t ii = 0; ii < kLoopCount; ++ii) {
        loop_timing[0][ii] = testPerformanceSingle(fn0, iterations);
        loop_timing[1][ii] = testPerformanceSingle(fn1, iterations);
        loop_timing[2][ii] = testPerformanceSingle(fn2, iterations);
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
    printf_s("  %-24s %12.4f \xc2\xb5s %12.4f \xc2\xb5s %12.4f \xc2\xb5s %7.2f%% %7.2f%%\n",
             Func<default::Vector>::name,
             timing[0], timing[1], timing[2],
             1.0e2 * timing[0] / timing[1],
             1.0e2 * timing[0] / timing[2]);
}

void testVectorAdd(std::vector<float> const& data, size_t iterations) {
    return testPerformance<vectorAddT>(data, iterations);
}

void testVectorDot(std::vector<float> const& data, size_t iterations) {
    return testPerformance<vectorDotT>(data, iterations);
}

void testVectorCross(std::vector<float> const& data, size_t iterations) {
    return testPerformance<vectorCrossT>(data, iterations);
}

void testHitSphere(std::vector<float> const& data, size_t iterations) {
    return testPerformance<hitSphereT>(data, iterations);
}

void testHitCapsule(std::vector<float> const& data, size_t iterations) {
    return testPerformance<hitCapsuleT>(data, iterations);
}
