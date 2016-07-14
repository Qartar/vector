#pragma once

#define _F_MMX      (1<< 0)
#define _F_SSE      (1<< 1)
#define _F_SSE2     (1<< 2)
#define _F_SSE3     (1<< 3)
#define _F_SSSE3    (1<< 4)
#define _F_SSE4_1   (1<< 5)
#define _F_SSE4_2   (1<< 6)
#define _F_FMA      (1<< 7)
#define _F_AVX      (1<< 8)
#define _F_AVX2     (1<< 9)

#ifndef _F_BITS
    //! If feature bits are not defined assume all features are available.
    #define _F_BITS     0xffffffff
#endif //_F_BITS

#define _HAS_FEATURE(x) (!!(_F_BITS & (x)))

#define _HAS_MMX        _HAS_FEATURE( _F_MMX    )
#define _HAS_SSE        _HAS_FEATURE( _F_SSE    )
#define _HAS_SSE2       _HAS_FEATURE( _F_SSE2   )
#define _HAS_SSE3       _HAS_FEATURE( _F_SSE3   )
#define _HAS_SSSE3      _HAS_FEATURE( _F_SSSE3  )
#define _HAS_SSE4_1     _HAS_FEATURE( _F_SSE4_1 )
#define _HAS_SSE4_2     _HAS_FEATURE( _F_SSE4_2 )
#define _HAS_FMA        _HAS_FEATURE( _F_FMA    )
#define _HAS_AVX        _HAS_FEATURE( _F_AVX    )
#define _HAS_AVX2       _HAS_FEATURE( _F_AVX2   )
