#include "Features.h"

#include <cctype>
#include <cstring>
#include <algorithm>
#include <set>
#include <string>

enum Register {
    EAX = 0,
    EBX = 1,
    ECX = 2,
    EDX = 3,
};

struct Feature {
    int         feature_bits;   //  internal feature bit mask
    char const* feature_name;
    int         function_index; //  cpuid function index
    Register    register_index; //  cpuid register index
    int         register_bits;  //  cpuid register bit mask

    bool operator<(Feature const& f) const {
        return feature_bits < f.feature_bits;
    }
};

//! List of features to be queried for support.
const std::set<Feature> features = {
    { _F_MMX,    "mmx",    1, EDX, (1<<23) },
    { _F_SSE,    "sse",    1, EDX, (1<<25) },
    { _F_SSE2,   "sse2",   1, EDX, (1<<26) },
    { _F_SSE3,   "sse3",   1, ECX, (1<< 0) },
    { _F_SSSE3,  "ssse3",  1, ECX, (1<< 9) },
    { _F_SSE4_1, "sse4.1", 1, ECX, (1<<19) },
    { _F_SSE4_2, "sse4.2", 1, ECX, (1<<20) },
    { _F_FMA,    "fma",    1, ECX, (1<<12) },
    { _F_AVX,    "avx",    1, ECX, (1<<28) },
    { _F_AVX2,   "avx2",   7, EBX, (1<< 5) },
};

////////////////////////////////////////////////////////////////////////////////
//! Check for the presence of an individual feature.
bool check_feature(Feature const& f)
{
#if defined(_WIN32)
    int registers[4];

    // get highest available function index
    __cpuid(registers, 0);
    int max_function = registers[0];

    // check if feature function is available
    if (max_function < f.function_index) {
        return false;
    }

    // check feature register bits
    __cpuidex(registers, f.function_index, 0);
    if (!(registers[f.register_index] & f.register_bits)) {
        return false;
    }

    return true;

#elif defined(__GNUC__)

    return !!__builtin_cpu_supports(f.feature_name);

#endif
}

////////////////////////////////////////////////////////////////////////////////
std::string to_upper(std::string const& str)
{
    std::string out(str);
    std::transform(out.begin(),
                   out.end(),
                   out.begin(),
                   std::toupper);
    return out;
}

////////////////////////////////////////////////////////////////////////////////
int main()
{
    int bits = 0;
    for (auto const& f: features) {
        if (check_feature(f)) {
            if (bits) {
                printf(", ");
            }
            printf("%s", to_upper(f.feature_name).c_str());
            bits |= f.feature_bits;
        }
    }
    return bits;
}
