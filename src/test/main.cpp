#include "Conformance.h"
#include "Performance.h"

#include <random>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    printf_s("Generating test data...\n");
    constexpr const size_t kIter = (1 << 24);
    std::vector<float> values(kIter);

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

    testVectorAdd(values);
    testVectorSub(values);
    testScalarMul(values);
    testScalarDiv(values);
    testVectorLength(values);
    testVectorLengthFast(values);
    testVectorNormalize(values);
    testVectorNormalizeFast(values);
    testVectorDot(values);
    testVectorCross(values);
    testVectorProject(values);
    testVectorReject(values);
    testVectorReflect(values);
    testMatrixScalar(values);
    testMatrixVector(values);
    testMatrixMatrix(values);
    testHitSphere(values);
    testHitCapsule(values);

    return 0;
}
