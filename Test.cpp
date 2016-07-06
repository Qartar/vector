#include "Test.h"

#include "Default.h"
#include "Aligned.h"
#include "Intrinsic.h"

#include <cstdio>
#include <cmath>

template<typename T>
struct testComparisonT {
    static constexpr const char* name = "testComparison";

    bool operator()() const {
        T a(1.0f, 2.0f, 3.0f, 4.0f);
        T b(2.0f, 3.0f, 4.0f, 5.0f);

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

template<typename T>
struct testAlgebraicT {
    static constexpr const char* name = "testAlgebraic";

    bool operator()() const {
        T a(1.0f, 2.0f, 3.0f, 4.0f);
        T b(2.0f, 3.0f, 4.0f, 5.0f);
        float c = 0.5f;

        if (a + b != T(3.0f, 5.0f, 7.0f, 9.0f)) {
            return false;
        }

        if (a - b != T(-1.0f, -1.0f, -1.0f, -1.0f)) {
            return false;
        }

        if (a * c != T(0.5f, 1.0f, 1.5f, 2.0f)) {
            return false;
        }

        if (a / c != T(2.0f, 4.0f, 6.0f, 8.0f)) {
            return false;
        }

        return true;
    }
};

template<typename T>
struct testLengthT {
    static constexpr const char* name = "testLength";

    bool operator()() const {
        T a(1.0f, 2.0f, 3.0f, 4.0f);
        T b(2.0f, 3.0f, 4.0f, 5.0f);

        if (a.LengthSqr() != (1.0f + 4.0f + 9.0f + 16.0f)) {
            return false;
        }
        if (a.Length() != std::sqrt(1.0f + 4.0f + 9.0f + 16.0f)) {
            return false;
        }

        T c = a.Normalize();
        float csqr = c.LengthSqr();
        if (1.0f - 1e-6f > csqr || csqr > 1.0f + 1e-6f) {
            return false;
        }

        T d = b.NormalizeFast();
        float dsqr = d.LengthSqr();
        // Maximum relative error of rsqrtps is less than 1.5*2^-12
        if (1.0f - 3e-4f > dsqr || dsqr > 1.0f + 3e-4f) {
            return false;
        }

        return true;
    }
};

template<typename T>
struct testDotProductT {
    static constexpr const char* name = "testDotProduct";

    bool operator()() const {
        T a(1.0f, 2.0f, 3.0f, 4.0f);
        T b(2.0f, 3.0f, 4.0f, 5.0f);

        if (a * b != (2.0f + 6.0f + 12.0f + 20.0f)) {
            return false;
        }
        return true;
    }
};

template<typename T>
struct testCrossProductT {
    static constexpr const char* name = "testCrossProduct";

    bool operator()() const {
        T a(2.0f, 0.0f, 0.0f, 0.0f);
        T b(0.0f, 3.0f, 0.0f, 0.0f);

        if (a % b != T(0.0f, 0.0f, 6.0f, 0.0f)) {
            return false;
        }

        if (a % b + b % a != T(0.0f, 0.0f, 0.0f, 0.0f)) {
            return false;
        }
        return true;
    }
};

template<template<typename> typename Func>
bool testFunc() {
    bool mask = true;
    if (!Func<default::Vector>()()) {
        printf_s("%s<default> failed!\n", Func<default::Vector>::name);
        mask = false;
    }
    if (!Func<aligned::Vector>()()) {
        printf_s("%s<aligned> failed!\n", Func<aligned::Vector>::name);
        mask = false;
    }
    if (!Func<intrinsic::Vector>()()) {
        printf_s("%s<intrinsic> failed!\n", Func<intrinsic::Vector>::name);
        mask = false;
    }
    return mask;
}

bool testComparison() {
    return testFunc<testComparisonT>();
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
