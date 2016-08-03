#include "Trace.h"

#include "vector/Default.h"
#include "vector/Intrinsic.h"

int main()
{
    constexpr size_t kSize = 1024;

    Trace<default::Matrix, default::Vector, default::Scalar>(kSize, kSize, "trace/default.bmp");
    Trace<intrinsic::Matrix, intrinsic::Vector, intrinsic::Scalar>(kSize, kSize, "trace/intrinsic.bmp");

    return 0;
}
