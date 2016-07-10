#pragma once

#include <vector>

#include "Color.h"

#include "math/Intersect.h"

struct Material {
    Color   color;
    float   roughness;
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

            color += Phong(material, origin, normal, outgoing, light);
        }
    }

    Color Phong(Material const& material, V const& origin, V const& normal, V const& outgoing, Light const& light) const
    {
        V light_direction = (light.origin - origin).Normalize();
        S diffuse_frac = light_direction * normal;

        if (diffuse_frac < 0.f) {
            return {0.f, 0.f, 0.f, 0.f};
        }

        Color diffuse_color = material.color * light.color * diffuse_frac;

        V reflect_direction = normal.Reflect(-light_direction);
        S specular_frac = reflect_direction * outgoing;

        Color specular_color = {0.f, 0.f, 0.f, 0.f};

        if (specular_frac > 0.f) {
            float k = (1.f - material.roughness);
            float p = 128.f * k;
            specular_color = light.color * k * pow(specular_frac, p);
        }

        return diffuse_color + specular_color;
    }

    template<typename T>
    float pow(T const& x, float p) const {
        return std::pow(x.ToFloat(), p);
    }

    template<>
    float pow(float const& x, float p) const {
        return std::pow(x, p);
    }

    std::vector<Light> _lights;
    std::vector<TraceSphere> _spheres;
};
