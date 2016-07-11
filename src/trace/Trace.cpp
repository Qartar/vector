#include "Trace.h"

#include "math/Default.h"
#include "math/Aligned.h"
#include "math/Intrinsic.h"

int main()
{
    constexpr size_t kSize = 256;

    Trace<default::Matrix, default::Vector, default::Scalar>(kSize, kSize, "trace/default.bmp");
    Trace<aligned::Matrix, aligned::Vector, aligned::Scalar>(kSize, kSize, "trace/aligned.bmp");
    Trace<intrinsic::Matrix, intrinsic::Vector, intrinsic::Scalar>(kSize, kSize, "trace/intrinsic.bmp");

    return 0;
}
