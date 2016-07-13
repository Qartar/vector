#pragma once

#include <algorithm>

#include "Color.h"

static constexpr float kPi = 3.1415926535897932f;

////////////////////////////////////////////////////////////////////////////////
//! Surface material
template<typename M, typename V, typename S>
struct Material {
    //! Diffuse color
    Color<M, V, S> diffuse_color;

    //! RMS of microfacet slope
    float roughness;

    //! Fresnel factor at direct incidence.
    //! Equal to ((n1-n2)/(n1+n2))^2
    //!     where n1 and n2 are the respective refractive indices of the
    //!     originating and participating mediums.
    //! For dielectrics (non-metals) this is generally around 0.04
    //! For metals this value should range from .7 to 1.
    float reflectance;

    //! Specular color
    //! For dielectrics (non-metals) this should be white.
    Color<M, V, S> specular_color;
};

////////////////////////////////////////////////////////////////////////////////
//! Point-light source
template<typename M, typename V, typename S>
struct Light {
    V origin;
    Color<M, V, S> color;
    S intensity;
};

////////////////////////////////////////////////////////////////////////////////
/**
 * Diffuse functions
 */

//------------------------------------------------------------------------------
class D_Lambert {
public:
    template<typename M, typename V, typename S>
    static S D(Material<M, V, S> const& mtr, V const& n, V const& l, V const& v, V const& h) {
        return 1.f;
    }
};

//------------------------------------------------------------------------------
class D_Disney {
public:
    template<typename M, typename V, typename S>
    static S D(Material<M, V, S> const& mtr, V const& n, V const& l, V const& v, V const& h) {
        S NdotV = n * v;
        S NdotL = n * l;
        S VdotH = v * h;

        S Fd0 = 0.5f + 2.f * VdotH * VdotH * mtr.roughness;
        return Schlick<M, V, S>(Fd0, NdotL) * Schlick<M, V, S>(Fd0, NdotV);
    }

protected:
    template<typename M, typename V, typename S>
    static S Schlick(S const& F0, S const& cos_t) {
        S k = 1.f - cos_t;
        S k2 = k * k;
        return (1.f + (F0 - 1.f) * (k2 * k2 * k));
    }
};

////////////////////////////////////////////////////////////////////////////////
/**
 * Microfacet distribution functions
 */

//------------------------------------------------------------------------------
class D_BlinnPhong {
public:
    template<typename M, typename V, typename S>
    static S D(Material<M, V, S> const& mtr, V const& n, V const& h) {
        S msqr = mtr.roughness * mtr.roughness;
        return pow(n * h, 2.f / msqr - 2.f) / (kPi * msqr);
    }
};

//------------------------------------------------------------------------------
class D_Beckmann {
public:
    template<typename M, typename V, typename S>
    static S D(Material<M, V, S> const& mtr, V const& n, V const& h) {
        S msqr = mtr.roughness * mtr.roughness;
        S cos2alpha = (h * n) * (h * n);
        S tan2alpha = (1.f - cos2alpha) / cos2alpha;

        return exp(-tan2alpha / msqr) / (kPi * msqr * cos2alpha * cos2alpha);
    }
};

//------------------------------------------------------------------------------
class D_GGX {
public:
    template<typename M, typename V, typename S>
    static S D(Material<M, V, S> const& mtr, V const& n, V const& h) {
        S msqr = mtr.roughness * mtr.roughness;
        S ndoth = (n * h) * (n * h);
        S den = (1.f - ndoth * ndoth * (1.f - msqr));
        return msqr / (kPi * den * den);
    }
};

////////////////////////////////////////////////////////////////////////////////
/**
 * Fresnel functions
 */

//------------------------------------------------------------------------------
class F_None {
public:
    template<typename M, typename V, typename S>
    static S F(Material<M, V, S> const& mtr, V const& n, V const& v, V const& h) {
        return mtr.reflectance;
    }
};

//------------------------------------------------------------------------------
class F_Schlick {
public:
    template<typename M, typename V, typename S>
    static S F(Material<M, V, S> const& mtr, V const& n, V const& v, V const& h) {
        S F0 = mtr.reflectance;
        S k = 1.f - v * h;
        S ksqr = k * k;
        return F0 + (1.f - F0) * ksqr * ksqr * k;
    }
};

////////////////////////////////////////////////////////////////////////////////
/**
 * Geometric attenuation functions
 */

//------------------------------------------------------------------------------
class G_None {
public:
    template<typename M, typename V, typename S>
    static S G(Material<M, V, S> const& mtr, V const& n, V const& l, V const& v, V const& h) {
        return 1.f;
    }
};

//------------------------------------------------------------------------------
class G_Implicit {
public:
    template<typename M, typename V, typename S>
    static S G(Material<M, V, S> const& mtr, V const& n, V const& l, V const& v, V const& h) {
        return (n * l) * (n * v);
    }
};

//------------------------------------------------------------------------------
class G_Neumann {
public:
    template<typename M, typename V, typename S>
    static S G(Material<M, V, S> const& mtr, V const& n, V const& l, V const& v, V const& h) {
        S NdotL = n * l;
        S NdotV = n * v;
        return NdotL * NdotV / std::max(NdotL, NdotV);
    }
};

//------------------------------------------------------------------------------
class G_CookTorrance {
public:
    template<typename M, typename V, typename S>
    static S G(Material<M, V, S> const& mtr, V const& n, V const& l, V const& v, V const& h) {
        S k = 2.f * (n * h) / (v * h);
        return min3(S(1.f), k * (n * v), k * (n * l));
    }

protected:
    template<typename T>
    static T const& min3(T const& x, T const& y, T const& z) {
        return std::min(x, std::min(y, z));
    }
};

//------------------------------------------------------------------------------
class G_Kelemen {
public:
    template<typename M, typename V, typename S>
    static S G(Material<M, V, S> const& mtr, V const& n, V const& l, V const& v, V const& h) {
        return (n * l) * (n * v) / ((v * h) * (v * h));
    }
};

//------------------------------------------------------------------------------
class G_GGX {
public:
    template<typename M, typename V, typename S>
    static S G(Material<M, V, S> const& mtr, V const& n, V const& l, V const& v, V const& h) {
        S msqr = mtr.roughness * mtr.roughness;
        S ndotv = n * v;
        S rad = 1.f - msqr + msqr / (ndotv * ndotv);
        return 2.f / (1.f + sqrt(rad));
    }
};

//------------------------------------------------------------------------------
class G_SmithHeightCorrelated {
public:
    template<typename M, typename V, typename S>
    static S G(Material<M, V, S> const& mtr, V const& n, V const& l, V const& v, V const& h) {
        S msqr = mtr.roughness * mtr.roughness;
        S NdotV = n * v;
        S NdotL = n * l;
        S NdotV2 = NdotV * NdotV;
        S NdotL2 = NdotL * NdotL;

        S lambda_v = .5f * (sqrt(msqr * (1.f - NdotL2) / NdotL2 + 1.f) - 1.f);
        S lambda_l = .5f * (sqrt(msqr * (1.f - NdotV2) / NdotV2 + 1.f) - 1.f);
        return 1.f / (1.f + lambda_v + lambda_l);
    }
};

////////////////////////////////////////////////////////////////////////////////
/**
 * Diffuse coefficient functions
 */

//------------------------------------------------------------------------------
class Kd_Lambert {
public:
    template<typename M, typename V, typename S>
    static S Kd(Material<M, V, S> const& mtr, V const& n, V const& l, V const& v, V const& h) {
        return D_Lambert::D<M, V, S>(mtr, n, l, v, h);
    }
};

//------------------------------------------------------------------------------
class Kd_Disney {
public:
    template<typename M, typename V, typename S>
    static S Kd(Material<M, V, S> const& mtr, V const& n, V const& l, V const& v, V const& h) {
        return D_Disney::D<M, V, S>(mtr, n, l, v, h);
    }
};

////////////////////////////////////////////////////////////////////////////////
/**
 * Specular coefficient functions
 */

//------------------------------------------------------------------------------
class Ks_BlinnPhong {
public:
    template<typename M, typename V, typename S>
    static S Ks(Material<M, V, S> const& mtr, V const& n, V const& l, V const& v, V const& h) {
        return D_BlinnPhong::D<M, V, S>(mtr, n, h) * F_None::F<M, V, S>(mtr, n, v, h);
    }
};

//------------------------------------------------------------------------------
template<typename D, typename F, typename G>
class Ks_CookTorrance {
public:
    template<typename M, typename V, typename S>
    static S Ks(Material<M, V, S> const& mtr, V const& n, V const& l, V const& v, V const& h) {
        return D::D<M, V, S>(mtr, n, h)
             * F::F<M, V, S>(mtr, n, v, h)
             * G::G<M, V, S>(mtr, n, l, v, h)
             / (4.f * (n * l) * (n * v));
    }
};

////////////////////////////////////////////////////////////////////////////////
/**
 * Light models
 */

//------------------------------------------------------------------------------
template<typename Diffuse, typename Specular>
class L_Separable {
public:
    template<typename M, typename V, typename S>
    static S Kd(Material<M, V, S> const& mtr, V const& n, V const& l, V const& v,  V const& h) {
        return Diffuse::Kd<M, V, S>(mtr, n, l, v, h);
    }

    template<typename M, typename V, typename S>
    static S Ks(Material<M, V, S> const& mtr, V const& n, V const& l, V const& v,  V const& h) {
        return Specular::Ks<M, V, S>(mtr, n, l, v, h);
    }
};

//------------------------------------------------------------------------------
using L_BlinnPhong = L_Separable<Kd_Lambert, Ks_BlinnPhong>;
