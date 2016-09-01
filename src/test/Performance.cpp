#include "Performance.h"

#include "vector/Reference.h"
#include "vector/Intrinsic.h"
#include "vector/Aliased.h"

#include "vector/Intersect.h"

#include "trace/Trace.h"

#include "Platform.h"

#if defined(__GNUC__)
// GCC complains about initialization with multiple `*v++` which would normally
// be valid but the order doesn't actually matter here. As long as GCC doesn't
// completely elide the potentially undefined code there's no problem...
#pragma GCC diagnostic ignored "-Wsequence-point"
#endif // defined(__GNUC__)

template<typename M, typename V, typename S>
struct vectorElemReadT {
    static constexpr const char* name = "vectorElemRead";
    static constexpr const size_t size = 5;

    struct Args {
        V a;
        size_t idx;
    };

    vectorElemReadT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii].a = { *v++, *v++, *v++, *v++ };
            _input[ii].idx = (int)(*v++) % 4;
        }
        _output.resize(data.size() / size);
    }

    void operator()() {
        S* out = _output.data();

        for (auto const& in: _input) {
            *out++ = in.a[in.idx];
        }
    }

    std::vector<Args> _input;
    std::vector<S> _output;
};

template<typename M, typename V, typename S>
struct vectorElemWriteT {
    static constexpr const char* name = "vectorElemWrite";
    static constexpr const size_t size = 6;

    struct Args {
        S a;
        size_t idx;
    };

    vectorElemWriteT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        _output.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _output[ii] = { *v++, *v++, *v++, *v++ };
            _input[ii].a = *v++;
            _input[ii].idx = (int)(*v++) % 4;
        }
    }

    void operator()() {
        V* out = _output.data();

        for (auto const& in: _input) {
            (*out++)[in.idx] = in.a;
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename M, typename V, typename S>
struct vectorAddT {
    static constexpr const char* name = "vectorAdd";
    static constexpr const size_t size = 8;

    struct Args {
        V a;
        V b;
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

    void operator()() {
        V* out = _output.data();

        for (auto const& in: _input) {
            *out++ = in.a + in.b;
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename M, typename V, typename S>
struct vectorSubT {
    static constexpr const char* name = "vectorSub";
    static constexpr const size_t size = 8;

    struct Args {
        V a;
        V b;
    };

    vectorSubT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii].a = { *v++, *v++, *v++, *v++ };
            _input[ii].b = { *v++, *v++, *v++, *v++ };
        }
        _output.resize(data.size() / size);
    }

    void operator()() {
        V* out = _output.data();

        for (auto const& in: _input) {
            *out++ = in.a - in.b;
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename M, typename V, typename S>
struct scalarMulT {
    static constexpr const char* name = "scalarMul";
    static constexpr const size_t size = 5;

    struct Args {
        V v;
        S s;
    };

    scalarMulT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii] = {{*v++, *v++, *v++, *v++}, *v++};
        }
        _output.resize(data.size() / size);
    }

    void operator()() {
        V* out = _output.data();

        for (auto const& in: _input) {
            *out++ = in.v * in.s;
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename M, typename V, typename S>
struct scalarDivT {
    static constexpr const char* name = "scalarDiv";
    static constexpr const size_t size = 5;

    struct Args {
        V v;
        S s;
    };

    scalarDivT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii] = {{*v++, *v++, *v++, *v++}, *v++};
        }
        _output.resize(data.size() / size);
    }

    void operator()() {
        V* out = _output.data();

        for (auto const& in: _input) {
            *out++ = in.v / in.s;
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename M, typename V, typename S>
struct vectorLengthT {
    static constexpr const char* name = "vectorLength";
    static constexpr const size_t size = 4;

    vectorLengthT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii] = {*v++, *v++, *v++, *v++};
        }
        _output.resize(data.size() / size);
    }

    void operator()() {
        S* out = _output.data();

        for (auto const& in: _input) {
            *out++ = in.Length();
        }
    }

    std::vector<V> _input;
    std::vector<S> _output;
};

template<typename M, typename V, typename S>
struct vectorLengthFastT {
    static constexpr const char* name = "vectorLengthFast";
    static constexpr const size_t size = 4;

    vectorLengthFastT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii] = {*v++, *v++, *v++, *v++};
        }
        _output.resize(data.size() / size);
    }

    void operator()() {
        S* out = _output.data();

        for (auto const& in: _input) {
            *out++ = in.LengthFast();
        }
    }

    std::vector<V> _input;
    std::vector<S> _output;
};

template<typename M, typename V, typename S>
struct vectorNormalizeT {
    static constexpr const char* name = "vectorNormalize";
    static constexpr const size_t size = 4;

    vectorNormalizeT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii] = {*v++, *v++, *v++, *v++};
        }
        _output.resize(data.size() / size);
    }

    void operator()() {
        V* out = _output.data();

        for (auto const& in: _input) {
            *out++ = in.Normalize();
        }
    }

    std::vector<V> _input;
    std::vector<V> _output;
};

template<typename M, typename V, typename S>
struct vectorNormalizeFastT {
    static constexpr const char* name = "vectorNormalizeFast";
    static constexpr const size_t size = 4;

    vectorNormalizeFastT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii] = {*v++, *v++, *v++, *v++};
        }
        _output.resize(data.size() / size);
    }

    void operator()() {
        V* out = _output.data();

        for (auto const& in: _input) {
            *out++ = in.NormalizeFast();
        }
    }

    std::vector<V> _input;
    std::vector<V> _output;
};

template<typename M, typename V, typename S>
struct vectorDotT {
    static constexpr const char* name = "vectorDot";
    static constexpr const size_t size = 8;

    struct Args {
        V a;
        V b;
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

    void operator()() {
        S* out = _output.data();

        for (auto const& in: _input) {
            *out++ = in.a * in.b;
        }
    }

    std::vector<Args> _input;
    std::vector<S> _output;
};

template<typename M, typename V, typename S>
struct vectorCrossT {
    static constexpr const char* name = "vectorCross";
    static constexpr const size_t size = 6;

    struct Args {
        V a;
        V b;
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

    void operator()() {
        V* out = _output.data();

        for (auto const& in: _input) {
            *out++ = in.a % in.b;
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename M, typename V, typename S>
struct vectorProjectT {
    static constexpr const char* name = "vectorProject";
    static constexpr const size_t size = 8;

    struct Args {
        V a;
        V b;
    };

    vectorProjectT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii].a = { *v++, *v++, *v++, *v++ };
            _input[ii].b = { *v++, *v++, *v++, *v++ };
        }
        _output.resize(data.size() / size);
    }

    void operator()() {
        V* out = _output.data();

        for (auto const& in: _input) {
            *out++ = in.a.Project(in.b);
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename M, typename V, typename S>
struct vectorRejectT {
    static constexpr const char* name = "vectorReject";
    static constexpr const size_t size = 8;

    struct Args {
        V a;
        V b;
    };

    vectorRejectT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii].a = { *v++, *v++, *v++, *v++ };
            _input[ii].b = { *v++, *v++, *v++, *v++ };
        }
        _output.resize(data.size() / size);
    }

    void operator()() {
        V* out = _output.data();

        for (auto const& in: _input) {
            *out++ = in.a.Reject(in.b);
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename M, typename V, typename S>
struct vectorReflectT {
    static constexpr const char* name = "vectorReflect";
    static constexpr const size_t size = 8;

    struct Args {
        V a;
        V b;
    };

    vectorReflectT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii].a = { *v++, *v++, *v++, *v++ };
            _input[ii].b = { *v++, *v++, *v++, *v++ };
        }
        _output.resize(data.size() / size);
    }

    void operator()() {
        V* out = _output.data();

        for (auto const& in: _input) {
            *out++ = in.a.Reflect(in.b);
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename M, typename V, typename S>
struct matrixScalarT {
    static constexpr const char* name = "matrixScalar";
    static constexpr const size_t size = 17;

    struct Args {
        M a;
        S b;
    };

    matrixScalarT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii].a = {
                *v++, *v++, *v++, *v++,
                *v++, *v++, *v++, *v++,
                *v++, *v++, *v++, *v++,
                *v++, *v++, *v++, *v++,
            };
            _input[ii].b = *v++;
        }
        _output.resize(data.size() / size);
    }

    void operator()() {
        M* out = _output.data();

        for (auto const& in: _input) {
            *out++ = in.a * in.b;
        }
    }

    std::vector<Args> _input;
    std::vector<M> _output;
};

template<typename M, typename V, typename S>
struct matrixVectorT {
    static constexpr const char* name = "matrixVector";
    static constexpr const size_t size = 20;

    struct Args {
        M a;
        V b;
    };

    matrixVectorT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii].a = {
                *v++, *v++, *v++, *v++,
                *v++, *v++, *v++, *v++,
                *v++, *v++, *v++, *v++,
                *v++, *v++, *v++, *v++,
            };
            _input[ii].b = {
                *v++, *v++, *v++, *v++,
            };
        }
        _output.resize(data.size() / size);
    }

    void operator()() {
        V* out = _output.data();

        for (auto const& in: _input) {
            *out++ = in.a * in.b;
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename M, typename V, typename S>
struct matrixMatrixT {
    static constexpr const char* name = "matrixMatrix";
    static constexpr const size_t size = 32;

    struct Args {
        M a;
        M b;
    };

    matrixMatrixT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii].a = {
                *v++, *v++, *v++, *v++,
                *v++, *v++, *v++, *v++,
                *v++, *v++, *v++, *v++,
                *v++, *v++, *v++, *v++,
            };
            _input[ii].b = {
                *v++, *v++, *v++, *v++,
                *v++, *v++, *v++, *v++,
                *v++, *v++, *v++, *v++,
                *v++, *v++, *v++, *v++,
            };
        }
        _output.resize(data.size() / size);
    }

    void operator()() {
        M* out = _output.data();

        for (auto const& in: _input) {
            *out++ = in.a * in.b;
        }
    }

    std::vector<Args> _input;
    std::vector<M> _output;
};

template<typename M, typename V, typename S>
struct matrixTransposeT {
    static constexpr const char* name = "matrixTranspose";
    static constexpr const size_t size = 16;

    matrixTransposeT(std::vector<float> const& data) {
        _input.resize(data.size() / size);
        float const* v = data.data();
        for (size_t ii = 0; ii < _input.size(); ++ii) {
            _input[ii] = {
                *v++, *v++, *v++, *v++,
                *v++, *v++, *v++, *v++,
                *v++, *v++, *v++, *v++,
                *v++, *v++, *v++, *v++,
            };
        }
        _output.resize(data.size() / size);
    }

    void operator()() {
        M* out = _output.data();

        for (auto const& in : _input) {
            *out++ = in.Transpose();
        }
    }

    std::vector<M> _input;
    std::vector<M> _output;
};

template<typename M, typename V, typename S>
struct hitSphereT {
    static constexpr const char* name = "hitSphere";
    static constexpr const size_t size = 10;

    struct Args {
        Ray<V, S> ray;
        Sphere<V, S> sphere;
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

    void operator()() {
        Hit<V, S>* out = _output.data();

        for (auto const& in: _input) {
            hitSphere<V, S>(in.ray, in.sphere, *out++);
        }
    }

    std::vector<Args> _input;
    std::vector<Hit<V, S>> _output;
};

template<typename M, typename V, typename S>
struct hitCapsuleT {
    static constexpr const char* name = "hitCapsule";
    static constexpr const size_t size = 13;

    struct Args {
        Ray<V, S> ray;
        Capsule<V, S> capsule;
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

    void operator()() {
        Hit<V, S>* out = _output.data();

        for (auto const& in: _input) {
            hitCapsule<V, S>(in.ray, in.capsule, *out++);
        }
    }

    std::vector<Args> _input;
    std::vector<Hit<V, S>> _output;
};

template<typename M, typename V, typename S>
struct traceSceneT {
    static constexpr const char* name = "traceScene";

    traceSceneT(std::vector<float> const&) {}

    void operator()() {
        Trace<M, V, S>(1024, 1024);
    }
};

template<typename Func>
double testPerformanceSingle(Func& fn) {
    Timer t;

    t.Start();
    fn();
    t.Stop();
    return t.Microseconds();
}

template<template<typename, typename, typename> class Func, size_t kLoopCount = 16>
void testPerformance(std::vector<float> const& data) {
    double loop_timing[3][kLoopCount];
    double timing[3];

    Func<reference::Matrix, reference::Vector, reference::Scalar> fn0(data);
    Func<intrinsic::Matrix, intrinsic::Vector, intrinsic::Scalar> fn1(data);
    Func<aliased::Matrix, aliased::Vector, aliased::Scalar> fn2(data);

    // Warm-up passes
    for (size_t ii = 0; ii < 4; ++ii) {
        testPerformanceSingle(fn0);
        testPerformanceSingle(fn1);
        testPerformanceSingle(fn2);
    }

    // Measured passes
    for (size_t ii = 0; ii < kLoopCount; ++ii) {
        loop_timing[0][ii] = testPerformanceSingle(fn0);
        loop_timing[1][ii] = testPerformanceSingle(fn1);
        loop_timing[2][ii] = testPerformanceSingle(fn2);
    }

    // Sort passes
    std::sort(&loop_timing[0][0], &loop_timing[0][kLoopCount]);
    std::sort(&loop_timing[1][0], &loop_timing[1][kLoopCount]);
    std::sort(&loop_timing[2][0], &loop_timing[2][kLoopCount]);

    // Select median
    timing[0] = loop_timing[0][kLoopCount/2];
    timing[1] = loop_timing[1][kLoopCount / 2];
    timing[2] = loop_timing[2][kLoopCount / 2];

    // Print results
    printf_s("  %-24s %12.4f \xc2\xb5s %12.4f \xc2\xb5s %12.4f \xc2\xb5s %7.2f%% %7.2f%%\n",
             Func<reference::Matrix, reference::Vector, reference::Scalar>::name,
             timing[0], timing[1], timing[2],
             1.0e2 * timing[0] / timing[1],
             1.0e2 * timing[0] / timing[2]);
}

void testVectorElemRead(std::vector<float> const& data) {
    return testPerformance<vectorElemReadT>(data);
}

void testVectorElemWrite(std::vector<float> const& data) {
    return testPerformance<vectorElemWriteT>(data);
}

void testVectorAdd(std::vector<float> const& data) {
    return testPerformance<vectorAddT>(data);
}

void testVectorSub(std::vector<float> const& data) {
    return testPerformance<vectorSubT>(data);
}

void testScalarMul(std::vector<float> const& data) {
    return testPerformance<scalarMulT>(data);
}

void testScalarDiv(std::vector<float> const& data) {
    return testPerformance<scalarDivT>(data);
}

void testVectorLength(std::vector<float> const& data) {
    return testPerformance<vectorLengthT>(data);
}

void testVectorLengthFast(std::vector<float> const& data) {
    return testPerformance<vectorLengthFastT>(data);
}

void testVectorNormalize(std::vector<float> const& data) {
    return testPerformance<vectorNormalizeT>(data);
}

void testVectorNormalizeFast(std::vector<float> const& data) {
    return testPerformance<vectorNormalizeFastT>(data);
}

void testVectorDot(std::vector<float> const& data) {
    return testPerformance<vectorDotT>(data);
}

void testVectorCross(std::vector<float> const& data) {
    return testPerformance<vectorCrossT>(data);
}

void testVectorProject(std::vector<float> const& data) {
    return testPerformance<vectorProjectT>(data);
}

void testVectorReject(std::vector<float> const& data) {
    return testPerformance<vectorRejectT>(data);
}

void testVectorReflect(std::vector<float> const& data) {
    return testPerformance<vectorReflectT>(data);
}

void testMatrixScalar(std::vector<float> const& data) {
    return testPerformance<matrixScalarT>(data);
}

void testMatrixVector(std::vector<float> const& data) {
    return testPerformance<matrixVectorT>(data);
}

void testMatrixMatrix(std::vector<float> const& data) {
    return testPerformance<matrixMatrixT>(data);
}

void testMatrixTranspose(std::vector<float> const& data) {
    return testPerformance<matrixTransposeT>(data);
}

void testHitSphere(std::vector<float> const& data) {
    return testPerformance<hitSphereT>(data);
}

void testHitCapsule(std::vector<float> const& data) {
    return testPerformance<hitCapsuleT>(data);
}

void testTraceScene(std::vector<float> const& data) {
    return testPerformance<traceSceneT>(data);
}
