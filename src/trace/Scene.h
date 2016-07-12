#pragma once

#include <algorithm>
#include <vector>

#include "Color.h"
#include "Light.h"

#include "math/Intersect.h"

template<typename M, typename V, typename S>
struct Object {
    M transform;
    Material<M, V, S> material;
};

template<typename M, typename V, typename S>
struct TraceHit : Hit<V, S> {
    Material<M, V, S> material;
};

template<typename M, typename V, typename S>
struct TraceSphere : Sphere<V, S> {
    Material<M, V, S> material;
};

template<typename M, typename V, typename S, typename L = L_BlinnPhong>
class Scene {
public:
    using Color = Color<M, V, S>;
    using Material = Material<M, V, S>;
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

    bool TraceColor(V const& start, V const& end, Color& color, int hit_count = 4) const
    {
        TraceHit hit;

        if (Trace(start, end, hit)) {
            V outgoing = (start - hit.point).Normalize();
            V hitpoint = hit.point + hit.normal * 1e-3f;
            Shade(hit.material, hitpoint, hit.normal, outgoing, color, hit_count);
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

    Color Reflect(Material const& material, V const& origin, V const& normal, V const& outgoing, int hit_count) const {
        TraceHit hit;
        Light light;

        light.origin = origin;
        light.intensity = 1.f;

        V reflect = normal.Reflect(-outgoing);

        if (Trace(origin, reflect * 1e1f, hit)) {
            V hitpoint = hit.point + hit.normal * 1e-3f;
            V new_outgoing = (origin - hit.point).Normalize();
            Shade(hit.material, hitpoint, hit.normal, new_outgoing, light.color, hit_count - 1);
            return ShadeLight(hit.material, hit.normal, light, hitpoint, new_outgoing);
        }
        return {0.f, 0.f, 0.f, 0.f};
    }

    void Shade(Material const& material, V const& origin, V const& normal, V const& outgoing, Color& color, int hit_count) const
    {
        color = {0.f, 0.f, 0.f, 0.f};

        for (auto const& light: _lights) {
            TraceHit hit;

            if (Trace(origin, light.origin, hit)) {
                continue;
            }

            color += ShadeLight(material, normal, light, origin, outgoing);
        }

        if (hit_count > 0) {
            //  Specular reflection
            color += Reflect(material, origin, normal, outgoing, hit_count);

            //  Diffuse reflection
            //  ...
        }
    }

    Color ShadeLight(Material const& material, V const& normal, Light const& light, V const& point, V const& vector) const
    {
        V n = normal;
        V l = light.origin - point;
        V v = vector;

        if (l * n <= 0.f) {
            return {0.f, 0.f, 0.f, 0.f};
        }

        l = l.Normalize();
        v = v.Normalize();
        V h = (l + v).Normalize();

        S K  = l * n * light.intensity / (light.origin - point).LengthSqr();
        S Kd = L::Kd(material, n, l, v, h);
        S Ks = L::Ks(material, n, l, v, h);

        return K * ( Kd * light.color * material.diffuse_color
                   + Ks * light.color * material.specular_color);
    }

    std::vector<Light> _lights;
    std::vector<TraceSphere> _spheres;
};
