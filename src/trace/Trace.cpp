#include "Trace.h"

#include "math/Default.h"
#include "math/Aligned.h"
#include "math/Intrinsic.h"

int main()
{
    Trace<default::Matrix, default::Vector, default::Scalar>("trace/default.bmp");
    Trace<aligned::Matrix, aligned::Vector, aligned::Scalar>("trace/aligned.bmp");
    Trace<intrinsic::Matrix, intrinsic::Vector, intrinsic::Scalar>("trace/intrinsic.bmp");

    return 0;
}
