#include "Performance.h"

#include "math/Default.h"
#include "math/Aligned.h"
#include "math/Intrinsic.h"

#include "math/Intersect.h"

template<typename V, typename S>
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

    void operator()(size_t iterations) {
        Args const* in = _input.data();
        V* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in, ++out) {
            *out = in->a + in->b;
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename V, typename S>
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

    void operator()(size_t iterations) {
        Args const* in = _input.data();
        V* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in, ++out) {
            *out = in->a - in->b;
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename V, typename S>
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

    void operator()(size_t iterations) {
        Args const* in = _input.data();
        V* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in, ++out) {
            *out = in->v * in->s;
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename V, typename S>
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

    void operator()(size_t iterations) {
        Args const* in = _input.data();
        V* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in, ++out) {
            *out = in->v / in->s;
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename V, typename S>
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

    void operator()(size_t iterations) {
        V const* in = _input.data();
        S* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in, ++out) {
            *out = in->Length();
        }
    }

    std::vector<V> _input;
    std::vector<S> _output;
};

template<typename V, typename S>
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

    void operator()(size_t iterations) {
        V const* in = _input.data();
        S* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in, ++out) {
            *out = in->LengthFast();
        }
    }

    std::vector<V> _input;
    std::vector<S> _output;
};

template<typename V, typename S>
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

    void operator()(size_t iterations) {
        V const* in = _input.data();
        V* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in, ++out) {
            *out = in->Normalize();
        }
    }

    std::vector<V> _input;
    std::vector<V> _output;
};

template<typename V, typename S>
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

    void operator()(size_t iterations) {
        V const* in = _input.data();
        V* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in, ++out) {
            *out = in->NormalizeFast();
        }
    }

    std::vector<V> _input;
    std::vector<V> _output;
};

template<typename V, typename S>
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

    void operator()(size_t iterations) {
        Args const* in = _input.data();
        S* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in, ++out) {
            *out = in->a * in->b;
        }
    }

    std::vector<Args> _input;
    std::vector<S> _output;
};

template<typename V, typename S>
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

    void operator()(size_t iterations) {
        Args const* in = _input.data();
        V* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in, ++out) {
            *out = in->a % in->b;
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename V, typename S>
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

    void operator()(size_t iterations) {
        Args const* in = _input.data();
        V* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in, ++out) {
            *out = in->a.Project(in->b);
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename V, typename S>
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

    void operator()(size_t iterations) {
        Args const* in = _input.data();
        V* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in, ++out) {
            *out = in->a.Reject(in->b);
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename V, typename S>
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

    void operator()(size_t iterations) {
        Args const* in = _input.data();
        V* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in, ++out) {
            *out = in->a.Reflect(in->b);
        }
    }

    std::vector<Args> _input;
    std::vector<V> _output;
};

template<typename V, typename S>
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

    void operator()(size_t iterations) {
        Args const* in = _input.data();
        Hit<V, S>* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in) {
            hitSphere<V, S>(in->ray, in->sphere, *out++);
        }
    }

    std::vector<Args> _input;
    std::vector<Hit<V, S>> _output;
};

template<typename V, typename S>
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

    void operator()(size_t iterations) {
        Args const* in = _input.data();
        Hit<V, S>* out = _output.data();

        for (size_t ii = 0; ii < iterations; ++ii, ++in) {
            hitCapsule<V, S>(in->ray, in->capsule, *out++);
        }
    }

    std::vector<Args> _input;
    std::vector<Hit<V, S>> _output;
};

template<typename Func>
double testPerformanceSingle(Func& fn, size_t iterations) {
    Timer t;

    t.Start();
    fn(iterations);
    t.Stop();
    return t.Microseconds();
}

template<template<typename, typename> typename Func, size_t kLoopCount = 16>
void testPerformance(std::vector<float> const& data, size_t iterations) {
    double loop_timing[3][kLoopCount];
    double timing[3];

    Func<default::Vector, default::Scalar> fn0(data);
    Func<aligned::Vector, aligned::Scalar> fn1(data);
    Func<intrinsic::Vector, intrinsic::Scalar> fn2(data);

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
             Func<default::Vector, default::Scalar>::name,
             timing[0], timing[1], timing[2],
             1.0e2 * timing[0] / timing[1],
             1.0e2 * timing[0] / timing[2]);
}

void testVectorAdd(std::vector<float> const& data, size_t iterations) {
    return testPerformance<vectorAddT>(data, iterations);
}

void testVectorSub(std::vector<float> const& data, size_t iterations) {
    return testPerformance<vectorSubT>(data, iterations);
}

void testScalarMul(std::vector<float> const& data, size_t iterations) {
    return testPerformance<scalarMulT>(data, iterations);
}

void testScalarDiv(std::vector<float> const& data, size_t iterations) {
    return testPerformance<scalarDivT>(data, iterations);
}

void testVectorLength(std::vector<float> const& data, size_t iterations) {
    return testPerformance<vectorLengthT>(data, iterations);
}

void testVectorLengthFast(std::vector<float> const& data, size_t iterations) {
    return testPerformance<vectorLengthFastT>(data, iterations);
}

void testVectorNormalize(std::vector<float> const& data, size_t iterations) {
    return testPerformance<vectorNormalizeT>(data, iterations);
}

void testVectorNormalizeFast(std::vector<float> const& data, size_t iterations) {
    return testPerformance<vectorNormalizeFastT>(data, iterations);
}

void testVectorDot(std::vector<float> const& data, size_t iterations) {
    return testPerformance<vectorDotT>(data, iterations);
}

void testVectorCross(std::vector<float> const& data, size_t iterations) {
    return testPerformance<vectorCrossT>(data, iterations);
}

void testVectorProject(std::vector<float> const& data, size_t iterations) {
    return testPerformance<vectorProjectT>(data, iterations);
}

void testVectorReject(std::vector<float> const& data, size_t iterations) {
    return testPerformance<vectorRejectT>(data, iterations);
}

void testVectorReflect(std::vector<float> const& data, size_t iterations) {
    return testPerformance<vectorReflectT>(data, iterations);
}

void testHitSphere(std::vector<float> const& data, size_t iterations) {
    return testPerformance<hitSphereT>(data, iterations);
}

void testHitCapsule(std::vector<float> const& data, size_t iterations) {
    return testPerformance<hitCapsuleT>(data, iterations);
}
