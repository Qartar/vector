#include "Trace.h"

#include "vector/Reference.h"
#include "vector/Intrinsic.h"

int main()
{
    constexpr size_t kSize = 1024;

    Trace<reference::Matrix, reference::Vector, reference::Scalar>(kSize, kSize, "trace/default.bmp");
    Trace<intrinsic::Matrix, intrinsic::Vector, intrinsic::Scalar>(kSize, kSize, "trace/intrinsic.bmp");

    return 0;
}
