#include "Conformance.h"
#include "Performance.h"

#include <random>

#if defined(_WIN32)

////////////////////////////////////////////////////////////////////////////////

#include <Windows.h>

void EnableUnicodeOutput()
{
    SetConsoleOutputCP(CP_UTF8);
}

void EnablePerformanceProfiling()
{
    SetProcessAffinityMask(GetCurrentProcess(), 1);
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
}

#else // defined(_WIN32)

////////////////////////////////////////////////////////////////////////////////

#include <sys/time.h>
#include <sys/resource.h>
#include <sched.h>

void EnableUnicodeOutput() {} // no-op

void EnablePerformanceProfiling()
{
   cpu_set_t affinity[] = {1};
   sched_setaffinity(0, sizeof(affinity), affinity);
   setpriority(0, 0, -20);
}

#endif

////////////////////////////////////////////////////////////////////////////////
int main()
{
    EnableUnicodeOutput();

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
    testElements();
    testAlgebraic();
    testLength();
    testDotProduct();
    testCrossProduct();
    testMatrixProduct();

    printf_s("Testing performance...\n");
    EnablePerformanceProfiling();

    testVectorElemRead(values);
    testVectorElemWrite(values);
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
    testTraceScene(values);

    return 0;
}
