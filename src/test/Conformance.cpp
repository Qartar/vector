#include "Conformance.h"

#include "vector/Reference.h"
#include "vector/Intrinsic.h"
#include "vector/Aliased.h"

#include "Platform.h"

#include <cstdio>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////
//! Macro for defining a conformance test.
#define TEST(NAME)                                                              \
template<typename M, typename V, typename S>                                    \
struct NAME##T {                                                                \
    static constexpr const char* name = #NAME;                                  \
    void operator()() const;                                                    \
};                                                                              \
template<typename M, typename V, typename S>                                    \
inline void NAME##T<M, V, S>::operator()() const

//------------------------------------------------------------------------------
struct TestFailure {};

//------------------------------------------------------------------------------
#define EXPECT_TRUE(expr)                                                       \
    if (!(expr)) { throw TestFailure{}; }

//------------------------------------------------------------------------------
#define EXPECT_FALSE(expr)                                                      \
    if ((expr)) { throw TestFailure{}; }

//------------------------------------------------------------------------------
#define _EXPECT_OP(lhs, op, rhs) EXPECT_TRUE((lhs) op (rhs))

//------------------------------------------------------------------------------
#define EXPECT_EQ(lhs, rhs) _EXPECT_OP(lhs, ==, rhs)
#define EXPECT_NE(lhs, rhs) _EXPECT_OP(lhs, !=, rhs)

//------------------------------------------------------------------------------
#define EXPECT_EQ_EPS(lhs, rhs, eps) {                                          \
    auto del = lhs - rhs;                                                       \
    EXPECT_TRUE(-eps < del && del < eps);                                       \
}

//------------------------------------------------------------------------------
#define EXPECT_NE_EPS(lhs, rhs, eps) {                                          \
    auto del = lhs - rhs;                                                       \
    EXPECT_TRUE(del < -eps || eps < del);                                       \
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
TEST(testComparison) {
    V a(1.0f, 2.0f, 3.0f, 4.0f);
    V b(1.0f, 1.0f, 2.0f, 3.0f);

    EXPECT_TRUE(a == a);
    EXPECT_FALSE(a != a);
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

//------------------------------------------------------------------------------
TEST(testElements) {
    V a(1.0f, 2.0f, 3.0f, 4.0f);
    V b(1.0f, 1.0f, 2.0f, 3.0f);

    EXPECT_EQ(a[0], S(1.0f));
    EXPECT_EQ(a[1], S(2.0f));
    EXPECT_EQ(a[2], S(3.0f));
    EXPECT_EQ(a[3], S(4.0f));

    b[0] = -1.0f;
    EXPECT_EQ(b, V(-1.0f, 1.0f, 2.0f, 3.0f));
    b[1] = -2.0f;
    EXPECT_EQ(b, V(-1.0f, -2.0f, 2.0f, 3.0f));
    b[2] = -3.0f;
    EXPECT_EQ(b, V(-1.0f, -2.0f, -3.0f, 3.0f));
    b[3] = -4.0f;
    EXPECT_EQ(b, V(-1.0f, -2.0f, -3.0f, -4.0f));
}

//------------------------------------------------------------------------------
TEST(testAlgebraic) {
    V a(1.0f, 2.0f, 3.0f, 4.0f);
    V b(2.0f, 3.0f, 4.0f, 5.0f);
    S c = 0.5f;
    S d = 3.0f;

    EXPECT_EQ(a + b, V(3.0f, 5.0f, 7.0f, 9.0f));
    EXPECT_EQ(a - b, V(-1.0f, -1.0f, -1.0f, -1.0f));
    EXPECT_EQ(a * c, V(0.5f, 1.0f, 1.5f, 2.0f));
    EXPECT_EQ(a / c, V(2.0f, 4.0f, 6.0f, 8.0f));
    EXPECT_EQ(c * (a + b), c * a + c * b);
    EXPECT_EQ((c + d) * a, c * a + d * a);
}

//------------------------------------------------------------------------------
TEST(testLength) {
    V a(1.0f, 2.0f, 3.0f, 4.0f);
    V b(2.0f, 3.0f, 4.0f, 5.0f);

    EXPECT_EQ(a.LengthSqr(), 1.0f + 4.0f + 9.0f + 16.0f);
    EXPECT_EQ(a.Length(), std::sqrt(1.0f + 4.0f + 9.0f + 16.0f));

    S lerr = a.Length() / a.LengthFast();
    // Maximum relative error of rsqrtps is less than 1.5*2^-12
    EXPECT_EQ_EPS(lerr, 1.0f, 3e-4f);

    S asqr = a.Normalize().LengthSqr();
    EXPECT_EQ_EPS(asqr, 1.0f, 1e-6f);

    S bsqr = b.NormalizeFast().LengthSqr();
    // Maximum relative error of rsqrtps is less than 1.5*2^-12
    EXPECT_EQ_EPS(bsqr, 1.0f, 3e-4f);
}

//------------------------------------------------------------------------------
TEST(testDotProduct) {
    V a(1.0f, 2.0f, 3.0f, 4.0f);
    V b(2.0f, 3.0f, 4.0f, 5.0f);

    EXPECT_EQ(a * b, 2.0f + 6.0f + 12.0f + 20.0f);
}

//------------------------------------------------------------------------------
TEST(testCrossProduct) {
    V a(2.0f, 0.0f, 0.0f, 0.0f);
    V b(0.0f, 3.0f, 0.0f, 0.0f);

    EXPECT_EQ(a % b, V(0.0f, 0.0f, 6.0f, 0.0f));
    EXPECT_EQ(a % b + b % a, V(0.0f, 0.0f, 0.0f, 0.0f));
}

//------------------------------------------------------------------------------
TEST(testMatrixScalarProduct) {
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
    EXPECT_EQ(C, B);

    M D = B / 2.0f;
    EXPECT_EQ(D, A);
}

//------------------------------------------------------------------------------
TEST(testMatrixVectorProduct) {
    M I = {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f,
    };

    V x = {
        1.f, 2.f, 3.f, 4.f,
    };

    M A = {
        0.f, 0.f, 0.f, 1.f,
        0.f, 0.f, 1.f, 0.f,
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
    };

    V y = {
        4.f, 3.f, 1.f, 2.f,
    };

    S z = 0.5f;

    EXPECT_EQ(I * x, x);
    EXPECT_EQ(A * x, y);
    EXPECT_EQ(A * x + A * y, A * (x + y));
    EXPECT_EQ(A * (x * z), (A * x) * z);
}

//------------------------------------------------------------------------------
TEST(testMatrixMatrixProduct) {
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

    V x = {
        1.f, 2.f, 3.f, 4.f,
    };

    EXPECT_EQ(A * B, C);
    EXPECT_EQ((B * A) * x, B * (A * x));
}

//------------------------------------------------------------------------------
TEST(testMatrixTranspose) {
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

    EXPECT_EQ(A.Transpose(), At);
    EXPECT_EQ(A.Transpose().Transpose(), A);
    EXPECT_EQ(A.Transpose() * B.Transpose(), (B * A).Transpose());
}

////////////////////////////////////////////////////////////////////////////////
//! Helper function for executing a conformance test for one implementation.
template<typename Func>
bool testFuncImpl() {
    try {
        Func()();
        return true;
    }
    catch (TestFailure const&)
    {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////
//! Helper function for executing conformance tests on each implementation.
template<template<typename, typename, typename> class Func>
bool testFunc() {
    constexpr char const* result_strings[] = {
        "failed", "ok",
    };

    bool b0 = testFuncImpl<Func<reference::Matrix, reference::Vector, reference::Scalar>>();
    bool b1 = testFuncImpl<Func<intrinsic::Matrix, intrinsic::Vector, intrinsic::Scalar>>();
    bool b2 = testFuncImpl<Func<aliased::Matrix, aliased::Vector, aliased::Scalar>>();

    // Print results
    printf_s("  %-24s %-15s %-15s %-15s\n",
             Func<reference::Matrix, reference::Vector, reference::Scalar>::name,
             result_strings[b0],
             result_strings[b1],
             result_strings[b2]);

    return b0 && b1 && b2;
}

//------------------------------------------------------------------------------
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
