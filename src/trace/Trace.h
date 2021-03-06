#pragma once

#include "Frustum.h"
#include "Scene.h"
#include "Image.h"

template<typename M, typename V, typename S, typename L>
void TraceView(Frustum<M, V, S> const& view, Scene<M, V, S, L> const& scene, Image<M, V, S>& image)
{
    Color<M, V, S> default_color = {.1f, .1f, .1f, 1.f};
    S zscale = view.Near() / view.Far();

    V dz = view.Forward();
    for (size_t ii = 0; ii < image.Height(); ++ii) {
        V dh = view.Up() * (1.0f - 2.0f * (float(ii) + 0.5f) / float(image.Height()));
        for (size_t jj = 0; jj < image.Width(); ++jj) {
            V dw = view.Left() * (1.0f - 2.0f * (float(jj) + 0.5f) / float(image.Width()));

            V dfar = dz + dh + dw;
            V end = view.Origin() + dfar;
            V start = view.Origin() + dfar * zscale;

            if (!scene.TraceColor(start, end, image[ii][jj])) {
                image[ii][jj] = default_color;
            }
        }
    }
}

template<typename M, typename V, typename S, typename L = L_BlinnPhong>
void Trace(size_t width, size_t height, char const* filename = nullptr) {
    Light<M, V, S>          lights[2];

    lights[0].origin        = V(2.f, 0.f, 4.f, 1.f);
    lights[0].color         = {1.f, 1.f, 1.f, 1.f};
    lights[0].intensity     = 10.f;

    lights[1].origin        = V(6.f, -4.f, -2.f, 1.f);
    lights[1].color         = {.2f, 1.f, 1.f, 1.f};
    lights[1].intensity     = 10.f;

    TraceSphere<M, V, S>    spheres[2];

    spheres[0].origin       = V(3.f, 0.f, 0.f, 1.f);
    spheres[0].radius       = .5f;
    spheres[0].material     = Material<M, V, S>{
        Color<M, V, S>{.2f, .05f, .02f, 1.f},
        .4f,        // roughness
        .04f,       // reflectance
        Color<M, V, S>{1.f, 1.f, 1.f, 1.f},
    };

    spheres[1].origin       = V(6.f, 1.f, 1.f, 1.f);
    spheres[1].radius       = 1.5f;
    spheres[1].material     = Material<M, V, S>{
        Color<M, V, S>{.05f, .02f, .2f, 1.f},
        .02f,       // roughness
        .04f,       // reflectance
        Color<M, V, S>{1.f, 1.f, 1.f, 1.f},
    };

    Frustum<M, V, S>    view(
        V(.5f, .5f, .4f, 1.f),
        V(1.f, 0.f, 0.f, 0.f),
        V(0.f, 1.f, 0.f, 0.f),
        V(0.f, 0.f, 1.f, 0.f),
        .1f, 8.f, 1.f, 1.f);

    Image<M, V, S>      image(width, height);
    Scene<M, V, S, L>   scene(lights, spheres);

    TraceView(view, scene, image);

    if (filename) {
        image.Save(filename);
    }
}
