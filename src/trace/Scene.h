#pragma once

#include <vector>

#include "Color.h"

#include "math/Intersect.h"

struct Material {
    Color   color;
};

template<typename M, typename V, typename S>
struct Light {
    V origin;
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
    using TraceHit = TraceHit<M, V, S>;

public:
    Scene() {}

    template<size_t Size>
    Scene(TraceSphere<M, V, S> const (&spheres)[Size])
        : _spheres(spheres, spheres + Size) {}

    bool TraceColor(V const& start, V const& end, Color& color) const
    {
        TraceHit hit;

        if (Trace(start, end, hit)) {
            V outgoing = (start - hit.point).Normalize();
            Shade(hit.material, hit.point, hit.normal, outgoing, color);
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
        color = material.color;
    }

    std::vector<Light<M, V, S>> _lights;
    std::vector<TraceSphere<M, V, S>> _spheres;
};
