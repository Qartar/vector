#include "Trace.h"

#include "math/Intrinsic.h"

template<typename M, typename V, typename S, size_t Size>
void test_models() {
    Trace<M, V, S, L_Separable<Kd_Lambert, Ks_BlinnPhong>>(Size, Size, "light/lambert-blinnphong.bmp");
    Trace<M, V, S, L_Separable<Kd_Disney, Ks_BlinnPhong>>(Size, Size, "light/disney-blinnphong.bmp");

    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_BlinnPhong, F_Schlick, G_None>>>(Size, Size, "light/disney-blinnphong-schlick.bmp");
    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_Beckmann, F_Schlick, G_None>>>(Size, Size, "light/disney-beckmann-schlick.bmp");
    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_GGX, F_Schlick, G_None>>>(Size, Size, "light/disney-ggx-schlick.bmp");

    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_BlinnPhong, F_Schlick, G_Implicit>>>(Size, Size, "light/disney-blinnphong-schlick-implicit.bmp");
    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_Beckmann, F_Schlick, G_Implicit>>>(Size, Size, "light/disney-beckmann-schlick-implicit.bmp");
    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_GGX, F_Schlick, G_Implicit>>>(Size, Size, "light/disney-ggx-schlick-implicit.bmp");

    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_BlinnPhong, F_Schlick, G_Neumann>>>(Size, Size, "light/disney-blinnphong-schlick-neumann.bmp");
    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_Beckmann, F_Schlick, G_Neumann>>>(Size, Size, "light/disney-beckmann-schlick-neumann.bmp");
    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_GGX, F_Schlick, G_Neumann>>>(Size, Size, "light/disney-ggx-schlick-neumann.bmp");

    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_BlinnPhong, F_Schlick, G_CookTorrance>>>(Size, Size, "light/disney-blinnphong-schlick-cook_torrance.bmp");
    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_Beckmann, F_Schlick, G_CookTorrance>>>(Size, Size, "light/disney-beckmann-schlick-cook_torrance.bmp");
    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_GGX, F_Schlick, G_CookTorrance>>>(Size, Size, "light/disney-ggx-schlick-cook_torrance.bmp");

    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_BlinnPhong, F_Schlick, G_Kelemen>>>(Size, Size, "light/disney-blinnphong-schlick-kelemen.bmp");
    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_Beckmann, F_Schlick, G_Kelemen>>>(Size, Size, "light/disney-beckmann-schlick-kelemen.bmp");
    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_GGX, F_Schlick, G_Kelemen>>>(Size, Size, "light/disney-ggx-schlick-kelemen.bmp");

    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_BlinnPhong, F_Schlick, G_GGX>>>(Size, Size, "light/disney-blinnphong-schlick-ggx.bmp");
    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_Beckmann, F_Schlick, G_GGX>>>(Size, Size, "light/disney-beckmann-schlick-ggx.bmp");
    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_GGX, F_Schlick, G_GGX>>>(Size, Size, "light/disney-ggx-schlick-ggx.bmp");

    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_BlinnPhong, F_Schlick, G_SmithHeightCorrelated>>>(Size, Size, "light/disney-blinnphong-schlick-smith_ggx_correlated.bmp");
    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_Beckmann, F_Schlick, G_SmithHeightCorrelated>>>(Size, Size, "light/disney-beckmann-schlick-smith_ggx_correlated.bmp");
    Trace<M, V, S, L_Separable<Kd_Disney, Ks_CookTorrance<D_GGX, F_Schlick, G_SmithHeightCorrelated>>>(Size, Size, "light/disney-ggx-schlick-smith_ggx_correlated.bmp");
}

int main()
{
    using M = intrinsic::Matrix;
    using V = intrinsic::Vector;
    using S = intrinsic::Scalar;
    constexpr size_t Size = 1024;

    test_models<M, V, S, Size>();

    return 0;
}
