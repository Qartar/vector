#include "Conformance.h"
#include "Performance.h"

#include <random>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    printf_s("Generating test data...\n");
    constexpr const size_t kIter = (1 << 20);
    std::vector<float> values(kIter * 32);

    std::uniform_real_distribution<float> r(0.0f, 16.0f);
    std::mt19937_64 gen;

    for (size_t ii = 0; ii < values.size(); ++ii) {
        values[ii] = r(gen);
    }

    printf_s("Testing conformance...\n");
    testComparison();
    testAlgebraic();
    testLength();
    testDotProduct();
    testCrossProduct();
    testMatrixProduct();

    printf_s("Testing performance...\n");
    SetProcessAffinityMask(GetCurrentProcess(), 1);
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    testVectorAdd(values, kIter);
    testVectorSub(values, kIter);
    testScalarMul(values, kIter);
    testScalarDiv(values, kIter);
    testVectorLength(values, kIter);
    testVectorLengthFast(values, kIter);
    testVectorNormalize(values, kIter);
    testVectorNormalizeFast(values, kIter);
    testVectorDot(values, kIter);
    testVectorCross(values, kIter);
    testVectorProject(values, kIter);
    testVectorReject(values, kIter);
    testVectorReflect(values, kIter);
    testMatrixScalar(values, kIter);
    testMatrixVector(values, kIter);
    testMatrixMatrix(values, kIter);
    testHitSphere(values, kIter);
    testHitCapsule(values, kIter);

    return 0;
}
