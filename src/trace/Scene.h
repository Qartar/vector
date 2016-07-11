#pragma once

#include <vector>

#include "Color.h"

#include "math/Intersect.h"

struct Material {
    Color   color;
    float   roughness;
    float   refractive_index;
};

template<typename M, typename V, typename S>
struct Light {
    V origin;
    Color color;
    S intensity;
};

template<typename M, typename V, typename S>
struct Object {
    M transform;
    Material material;
};

template<typename M, typename V, typename S>
struct TraceHit : Hit<V, S> {
    Material material;
};

template<typename M, typename V, typename S>
struct TraceSphere : Sphere<V, S> {
    Material material;
};

template<typename M, typename V, typename S>
class Scene {
public:
    using Light = Light<M, V, S>;
    using TraceHit = TraceHit<M, V, S>;
    using TraceSphere = TraceSphere<M, V, S>;

public:
    Scene() {}

    template<size_t Size>
    Scene(TraceSphere const (&spheres)[Size])
        : _spheres(spheres, spheres + Size) {}

    template<size_t NumLights, size_t NumSpheres>
    Scene(Light const (&lights)[NumLights],
          TraceSphere const (&spheres)[NumSpheres])
        : _lights(lights, lights + NumLights)
        , _spheres(spheres, spheres + NumSpheres) {}

    bool TraceColor(V const& start, V const& end, Color& color) const
    {
        TraceHit hit;

        if (Trace(start, end, hit)) {
            V outgoing = (start - hit.point).Normalize();
            V hitpoint = hit.point + hit.normal * 1e-3f;
            Shade(hit.material, hitpoint, hit.normal, outgoing, color);
            return true;
        }
        return false;
    }

private:
    bool Trace(V const& start, V const& end, TraceHit& hit) const
    {
        S mindist = 1.0f;
        for (auto const& sphere: _spheres) {
            TraceHit tmp;

            if (hitSphere({start, end}, sphere, tmp) && tmp.t < mindist) {
                hit = tmp;
                hit.material = sphere.material;
                mindist = hit.t;
            }
        }

        return (mindist < 1.0f);
    }

    void Shade(Material const& material, V const& origin, V const& normal, V const& outgoing, Color& color) const
    {
        color = {0.f, 0.f, 0.f, 0.f};

        for (auto const& light: _lights) {
            TraceHit hit;

            if (Trace(origin, light.origin, hit)) {
                continue;
            }

            //color += Phong(material, origin, normal, outgoing, light);
            color += CookTorrance(material, origin, normal, outgoing, light);
        }
    }

    Color Phong(Material const& material, V const& origin, V const& normal, V const& outgoing, Light const& light) const
    {
        V light_direction = (light.origin - origin).Normalize();
        V half_direction = (light_direction + outgoing).Normalize();

        S diffuse_frac = light_direction * normal;

        if (diffuse_frac < 0.f) {
            return {0.f, 0.f, 0.f, 0.f};
        }

        Color diffuse_color = material.color * light.color * diffuse_frac;

        S specular_frac = half_direction * normal;

        Color specular_color = {0.f, 0.f, 0.f, 0.f};

        if (specular_frac > 0.f) {
            float msqr = material.roughness * material.roughness;
            float k = 1.f / (kPi * msqr);
            float p = 2.f / msqr - 2.f;
            specular_color = light.color * k * pow(specular_frac, p) * diffuse_frac;
        }

        return diffuse_color + specular_color;
    }

    Color CookTorrance(Material const& material, V const& origin, V const& normal, V const& outgoing, Light const& light) const
    {
        V light_direction = (light.origin - origin).Normalize();
        V half_direction = (light_direction + outgoing).Normalize();

        //  Vector coefficients
        S VdotN = outgoing * normal;
        S LdotN = light_direction * normal;
        S HdotN = half_direction * normal;
        S VdotH = outgoing * half_direction;

        if (LdotN < 0.f) {
            return {.0f, .0f, .0f, .0f};
        }

#if 0
        return material.color * light.color * (1.f - F_Schlick(material, LdotN));
#elif 0
        //return light.color * G_GGX(material, VdotN) * G_GGX(material, LdotN);
        return light.color * G_CookTorrance(HdotN, VdotN, LdotN, VdotH);
#else
        //  Microfacet distribution
        //S D = D_Beckmann(material, HdotN);
        S D = D_GGX(material, HdotN);

        //  Fresnel coefficient
        S F = F_Schlick(material, VdotN);

        //  Geometric attenuation
        //S G = G_CookTorrance(HdotN, VdotN, LdotN, VdotH);
        //S G = G_GGX(material, VdotN) * G_GGX(material, LdotN);
        S G = G_Kelemen(LdotN, VdotN, VdotH);

        //  Diffuse coefficient
        S Kd = 1.f - F_Schlick(material, LdotN);

        //  Specular coefficient
        S Ks = D * F * G / (4.f * VdotN);

        return material.color * light.color * Kd * LdotN
                              + light.color * Ks;
#endif
    }

    S D_BlinnPhong(Material const& material, S const& HdotN) const {
        S msqr = material.roughness * material.roughness;
        return HdotN * pow(2.f / msqr - 2.f) / (kPi * msqr);
    }

    S D_Beckmann(Material const& material, S const& HdotN) const {
        S msqr = material.roughness * material.roughness;
        S bsqr = HdotN * HdotN;

        S num = exp((bsqr - 1.f) / (msqr * bsqr));
        S den = kPi * msqr * bsqr * bsqr;

        return num / den;
    }

    S D_GGX(Material const& material, S const& HdotN) const {
        S msqr = material.roughness * material.roughness;
        S d = HdotN * HdotN * (msqr - 1.f) + 1.f;

        return msqr / (kPi * d * d);
    }

    S F_Schlick(Material const& material, S const& VdotN) const {
        S n = (1.f - material.refractive_index) / (1.f + material.refractive_index);
        S R0 = n * n;
        S k = (1.f - VdotN);
        S k2 = k * k;
        S k5 = k2 * k2 * k;

        return R0 + (1.f - R0) * k5;
    }

    S G_Implicit(S const& NdotL, S const& NdotV) const {
        return NdotL * NdotV;
    }

    S G_Neumann(S const& NdotL, S const& NdotV) const {
        return NdotL * NdotV / max(NdotL, NdotV);
    }

    S G_CookTorrance(S const& NdotH, S const& NdotV, S const& NdotL, S const& VdotH) const {
        return min3(S(1.f),
                    2.f * NdotH * NdotV / VdotH,
                    2.f * NdotH * NdotL / VdotH);
    }

    S G_Kelemen(S const& NdotL, S const& NdotV, S const& VdotH) const {
        return NdotL * NdotV / (VdotH * VdotH);
    }

    S G_GGX(Material const& material, S const& NdotV) const {
        S msqr = material.roughness * material.roughness;
        return 2.f * NdotV / (NdotV + sqrt(msqr + (1.f - msqr) * NdotV * NdotV));
    }

    constexpr static const float kPi = 3.1415926535897932f;

    template<typename T>
    T const& min3(T const& x, T const& y, T const& z) const {
        return min(x, min(y, z));
    }

    std::vector<Light> _lights;
    std::vector<TraceSphere> _spheres;
};
