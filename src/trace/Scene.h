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

    //! Calculate the illuminated surface color at the nearest intersection of
    //! an object in the scene with the ray from `start` to `end`.
    bool TraceColor(V const& start, V const& end, Color& color, int hit_count = 4) const
    {
        TraceHit hit;

        if (Trace(start, end, hit)) {
            V view = (start - hit.point).Normalize();
            V hitpoint = hit.point + hit.normal * kEpsilon;
            color = Shade(hit.material, hitpoint, hit.normal, view, hit_count);
            return true;
        }
        return false;
    }

protected:
    static constexpr float kEpsilon = 1e-5f;

    std::vector<Light> _lights;
    std::vector<TraceSphere> _spheres;

protected:
    //! Find the nearest surface intersection between start and end.
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

    //! Calculate the indirect illumination at a point from the given direction.
    Color ShadeIndirect(Material const& material, V const& origin, V const& normal, V const& view, V const& direction, int hit_count) const
    {
        TraceHit hit;

        if (Trace(origin + normal * kEpsilon, origin + direction * 1e3f, hit)) {
            // Determine the lit color of the object at the intersection point
            // from the perspective of the original surface.
            V hitpoint = hit.point + hit.normal * kEpsilon;
            V new_view = (origin - hit.point).Normalize();

            Color hitcolor = Shade(hit.material, hitpoint, hit.normal, new_view, hit_count - 1);

            // Use the equivalence of direct and indirect illumination to add
            // the indirect contribution of the intersected surface to the
            // lighting of the original surface.
            Light light = {
                hitpoint,   //  origin
                hitcolor,   //  color
                1.f,        //  intensity
            };

            return ShadeLight(material, normal, light, origin, view);
        }

        // No intersection.
        return {0.f, 0.f, 0.f, 0.f};
    }

    //! Calculate the direct and indirect illumination at a point from the entire scene.
    Color Shade(Material const& material, V const& origin, V const& normal, V const& view, int hit_count) const
    {
        Color color = {0.f, 0.f, 0.f, 0.f};

        // Add the direct illumination of each light in the scene.
        for (auto const& light: _lights) {
            TraceHit hit;

            if (Trace(origin, light.origin, hit)) {
                continue;
            }

            color += ShadeLight(material, normal, light, origin, view);
        }

        // Add indirect illumination from other surfaces in the scene.
        if (hit_count > 0) {
            //  Specular reflection
            color += ShadeIndirect(material, origin, normal, view, normal.Reflect(-view), hit_count);

            //  Diffuse reflection
            //  ...
        }

        return color;
    }

    //! Calculate the direct illumination at a point from a single light source.
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
};
