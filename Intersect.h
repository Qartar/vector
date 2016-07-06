#pragma once

template<typename T>
struct Ray {
    T start;
    T end;
};

template<typename T>
struct Sphere {
    T origin;
    float radius;
};

template<typename T>
struct Hit {
    float t;
    T point;
    T normal;
};

template<typename T>
bool hitSphere(Ray<T> const& ray,
               Sphere<T> const& sphere,
               Hit<T>& hit)
{
    T rayVec = ray.end - ray.start;
    T sphereVec = ray.start - sphere.origin;

    float A = rayVec * rayVec;
    float B = 2.0f * rayVec * sphereVec;
    float C = sphereVec * sphereVec - sphere.radius * sphere.radius;

    float Dsqr = B * B - 4.0f * A * C;

    if (Dsqr < 0.0f) {
        return false;
    }

    float D = std::sqrt(Dsqr);

    float t0 = 0.5f * (-B - D) / A;
    float t1 = 0.5f * (-B + D) / A;

    if (t0 >= 0.0f && t0 <= 1.0f) {
        hit.t = t0;
    } else if (t1 >= 0.0f && t1 <= 1.0f) {
        hit.t = t1;
    } else {
        return false;
    }

    hit.point = ray.start + rayVec * t0;
    hit.normal = t0 >= 0.0f ? T(hit.point - sphere.origin).Normalize()
                            : T(sphere.origin - hit.point).Normalize();
    return true;
}
