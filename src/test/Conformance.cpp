#include "Conformance.h"

#include "vector/Reference.h"
#include "vector/Intrinsic.h"
#include "vector/Aliased.h"

#include "Platform.h"

#include <cstdio>
#include <cmath>

template<typename M, typename V, typename S>
struct testComparisonT {
    static constexpr const char* name = "testComparison";

    bool operator()() const {
        V a(1.0f, 2.0f, 3.0f, 4.0f);
        V b(1.0f, 1.0f, 2.0f, 3.0f);

        if (a != a) {
            return false;
        }
        if (!(a == a)) {
            return false;
        }
        if (a == b) {
            return false;
        }
        if (!(a != b)) {
            return false;
        }
        return true;
    }
};

template<typename M, typename V, typename S>
struct testElementsT {
    static constexpr const char* name = "testElements";

    bool operator()() const {
        V a(1.0f, 2.0f, 3.0f, 4.0f);
        V b(1.0f, 1.0f, 2.0f, 3.0f);

        if (a[0] != S(1.0f)) {
            return false;
        }
        if (a[1] != S(2.0f)) {
            return false;
        }
        if (a[2] != S(3.0f)) {
            return false;
        }
        if (a[3] != S(4.0f)) {
            return false;
        }

        b[0] = -1.0f;
        if (b != V(-1.0f, 1.0f, 2.0f, 3.0f)) {
            return false;
        }

        b[1] = -2.0f;
        if (b != V(-1.0f, -2.0f, 2.0f, 3.0f)) {
            return false;
        }

        b[2] = -3.0f;
        if (b != V(-1.0f, -2.0f, -3.0f, 3.0f)) {
            return false;
        }

        b[3] = -4.0f;
        if (b != V(-1.0f, -2.0f, -3.0f, -4.0f)) {
            return false;
        }

        return true;
    }
};

template<typename M, typename V, typename S>
struct testAlgebraicT {
    static constexpr const char* name = "testAlgebraic";

    bool operator()() const {
        V a(1.0f, 2.0f, 3.0f, 4.0f);
        V b(2.0f, 3.0f, 4.0f, 5.0f);
        S c = 0.5f;
        S d = 3.0f;

        if (a + b != V(3.0f, 5.0f, 7.0f, 9.0f)) {
            return false;
        }

        if (a - b != V(-1.0f, -1.0f, -1.0f, -1.0f)) {
            return false;
        }

        if (a * c != V(0.5f, 1.0f, 1.5f, 2.0f)) {
            return false;
        }

        if (a / c != V(2.0f, 4.0f, 6.0f, 8.0f)) {
            return false;
        }

        if (c * (a + b) != c * a + c * b) {
            return false;
        }

        if ((c + d) * a != c * a + d * a) {
            return false;
        }

        return true;
    }
};

template<typename M, typename V, typename S>
struct testLengthT {
    static constexpr const char* name = "testLength";

    bool operator()() const {
        V a(1.0f, 2.0f, 3.0f, 4.0f);
        V b(2.0f, 3.0f, 4.0f, 5.0f);

        if (a.LengthSqr() != (1.0f + 4.0f + 9.0f + 16.0f)) {
            return false;
        }
        if (a.Length() != std::sqrt(1.0f + 4.0f + 9.0f + 16.0f)) {
            return false;
        }

        S lerr = a.Length() / a.LengthFast();
        // Maximum relative error of rsqrtps is less than 1.5*2^-12
        if (1.0f - 3e4f > lerr || lerr > 1.0f + 3e-4f) {
            return false;
        }

        S asqr = a.Normalize().LengthSqr();
        if (1.0f - 1e-6f > asqr || asqr > 1.0f + 1e-6f) {
            return false;
        }

        S bsqr = b.NormalizeFast().LengthSqr();
        // Maximum relative error of rsqrtps is less than 1.5*2^-12
        if (1.0f - 3e-4f > bsqr || bsqr > 1.0f + 3e-4f) {
            return false;
        }

        return true;
    }
};

template<typename M, typename V, typename S>
struct testDotProductT {
    static constexpr const char* name = "testDotProduct";

    bool operator()() const {
        V a(1.0f, 2.0f, 3.0f, 4.0f);
        V b(2.0f, 3.0f, 4.0f, 5.0f);

        if (a * b != (2.0f + 6.0f + 12.0f + 20.0f)) {
            return false;
        }
        return true;
    }
};

template<typename M, typename V, typename S>
struct testCrossProductT {
    static constexpr const char* name = "testCrossProduct";

    bool operator()() const {
        V a(2.0f, 0.0f, 0.0f, 0.0f);
        V b(0.0f, 3.0f, 0.0f, 0.0f);

        if (a % b != V(0.0f, 0.0f, 6.0f, 0.0f)) {
            return false;
        }

        if (a % b + b % a != V(0.0f, 0.0f, 0.0f, 0.0f)) {
            return false;
        }
        return true;
    }
};

template<typename M, typename V, typename S>
struct testMatrixScalarProductT {
    static constexpr const char* name = "testMatrixScalarProduct";

    bool operator()() const {
        M A = {
            1.f, 2.f, 3.f, 4.f,
            2.f, 3.f, 4.f, 3.f,
            3.f, 4.f, 3.f, 2.f,
            4.f, 3.f, 2.f, 1.f,
        };

        M B = {
            2.f, 4.f, 6.f, 8.f,
            4.f, 6.f, 8.f, 6.f,
            6.f, 8.f, 6.f, 4.f,
            8.f, 6.f, 4.f, 2.f,
        };

        M C = A * 2.f;

        if (C != B) {
            return false;
        }

        M D = B / 2.0f;

        if (D != A) {
            return false;
        }

        return true;
    }
};

template<typename M, typename V, typename S>
struct testMatrixVectorProductT {
    static constexpr const char* name = "testMatrixVectorProduct";

    bool operator()() const {
        M I = {
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f,
        };

        V x = {
            1.f, 2.f, 3.f, 4.f,
        };

        if (I * x != x) {
            return false;
        }

        M A = {
            0.f, 0.f, 0.f, 1.f,
            0.f, 0.f, 1.f, 0.f,
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
        };

        V y = {
            4.f, 3.f, 1.f, 2.f,
        };

        if (A * x != y) {
            return false;
        }

        if (A * x + A * y != A * (x + y)) {
            return false;
        }

        S z = 0.5f;

        if (A * (x * z) != (A * x) * z) {
            return false;
        }

        return true;
    }
};

template<typename M, typename V, typename S>
struct testMatrixMatrixProductT {
    static constexpr const char* name = "testMatrixMatrixProduct";

    bool operator()() const {
        M A = {
            1.f, 2.f, 3.f, 4.f,
            2.f, 3.f, 4.f, 3.f,
            3.f, 4.f, 3.f, 2.f,
            4.f, 3.f, 2.f, 1.f,
        };

        M B = {
            0.f, 0.f, 0.f, 1.f,
            0.f, 0.f, 1.f, 0.f,
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
        };

        M C = {
            3.f, 4.f, 2.f, 1.f,
            4.f, 3.f, 3.f, 2.f,
            3.f, 2.f, 4.f, 3.f,
            2.f, 1.f, 3.f, 4.f,
        };

        if (A * B != C) {
            return false;
        }

        V x = {
            1.f, 2.f, 3.f, 4.f,
        };

        if ((B * A) * x != B * (A * x)) {
            return false;
        }

        return true;
    }
};

template<typename M, typename V, typename S>
struct testMatrixTransposeT {
    static constexpr const char* name = "testMatrixTranspose";

    bool operator()() const {
        M A = {
            1.f, 2.f, 3.f, 4.f,
            2.f, 3.f, 4.f, 5.f,
            3.f, 4.f, 5.f, 6.f,
            4.f, 5.f, 6.f, 7.f,
        };

        M At = {
            1.f, 2.f, 3.f, 4.f,
            2.f, 3.f, 4.f, 5.f,
            3.f, 4.f, 5.f, 6.f,
            4.f, 5.f, 6.f, 7.f,
        };

        M B = {
            3.f, 4.f, 2.f, 1.f,
            4.f, 3.f, 3.f, 2.f,
            3.f, 2.f, 4.f, 3.f,
            2.f, 1.f, 3.f, 4.f,
        };

        if (A.Transpose() != At) {
            return false;
        }

        if (A.Transpose().Transpose() != A) {
            return false;
        }

        if (A.Transpose() * B.Transpose() != (B * A).Transpose()) {
            return false;
        }

        return true;
    }
};

template<template<typename, typename, typename> class Func>
bool testFunc() {
    constexpr char const* result_strings[] = {
        "failed", "ok",
    };

    bool b0 = Func<reference::Matrix, reference::Vector, reference::Scalar>()();
    bool b1 = Func<intrinsic::Matrix, intrinsic::Vector, intrinsic::Scalar>()();
    bool b2 = Func<aliased::Matrix, aliased::Vector, aliased::Scalar>()();

    // Print results
    printf_s("  %-24s %-15s %-15s %-15s\n",
             Func<reference::Matrix, reference::Vector, reference::Scalar>::name,
             result_strings[b0],
             result_strings[b1],
             result_strings[b2]);

    return b0 && b1 && b2;
}

bool testComparison() {
    return testFunc<testComparisonT>();
}

bool testElements() {
    return testFunc<testElementsT>();
}

bool testAlgebraic() {
    return testFunc<testAlgebraicT>();
}

bool testLength() {
    return testFunc<testLengthT>();
}

bool testDotProduct() {
    return testFunc<testDotProductT>();
}

bool testCrossProduct() {
    return testFunc<testCrossProductT>();
}

bool testMatrixProduct() {
    bool b1 = testFunc<testMatrixScalarProductT>();
    bool b2 = testFunc<testMatrixVectorProductT>();
    bool b3 = testFunc<testMatrixMatrixProductT>();
    return b1 && b2 && b3;
}

bool testMatrixTranspose() {
    return testFunc<testMatrixTransposeT>();
}
