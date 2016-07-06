#include "Default.h"
#include "Aligned.h"
#include "Intrinsic.h"

#include "Intersect.h"

#include "Test.h"

#include <cstdint>
#include <algorithm>
#include <random>
#define NOMINMAX
#include <Windows.h>

int64_t Timestamp() {
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return li.QuadPart;
}

int64_t Accumulate(int64_t accum, int64_t start, int64_t end) {
    return accum + std::max<int64_t>(0i64, end - start);
}

double Microseconds(int64_t accum) {
    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);
    return 1.0e6 * double(accum) / double(li.QuadPart);
}

template<class T, size_t kIter>
float test(std::vector<float> const& values) {
    float noalias = 0.0f;
    auto accum = 0i64;

    float const* v = values.data();

    auto start = Timestamp();
    for (size_t ii = 0; ii < kIter; ++ii) {
        Ray<T> ray = {
            { *v++, *v++, *v++, 1.0f },
            { *v++, *v++, *v++, 1.0f },
        };

        Sphere<T> sphere = {
            { *v++, *v++, *v++, 1.0f }, *v++,
        };

        Hit<T> hit;

        if (hitSphere<T>(ray, sphere, hit)) {
            noalias += hit.t;
        }
    }
    accum = Accumulate(accum, start, Timestamp());
    auto microseconds = Microseconds(accum);

    printf_s("%12.4f : %12.4f us\n", noalias, microseconds);

    return noalias;
}

int main() {
    testComparison();
    testAlgebraic();
    testLength();
    testDotProduct();
    testCrossProduct();

    printf_s("Generating test data...\n");
    constexpr const size_t kIter = (1 << 20);
    std::vector<float> values(kIter * 10);

    std::uniform_real_distribution<float> r(0.0f, 16.0f);
    std::mt19937_64 gen;

    for (size_t ii = 0; ii < values.size(); ++ii) {
        values[ii] = r(gen);
    }

    int result = 0;
    for (size_t ii = 0; ii < 4; ++ii) {
        printf_s("\n");
        result += (int )test<default::Vector, kIter>(values);
        result += (int )test<aligned::Vector, kIter>(values);
        result += (int )test<intrinsic::Vector, kIter>(values);
    }
    return result;
}
