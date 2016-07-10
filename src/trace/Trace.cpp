#include "Trace.h"

#include "math/Default.h"
#include "math/Aligned.h"
#include "math/Intrinsic.h"

int main()
{
    Trace<default::Matrix, default::Vector, default::Scalar>("a.bmp");
    Trace<aligned::Matrix, aligned::Vector, aligned::Scalar>("b.bmp");
    Trace<intrinsic::Matrix, intrinsic::Vector, intrinsic::Scalar>("c.bmp");

    return 0;
}
