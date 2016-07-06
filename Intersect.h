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
struct Capsule {
    T start;
    T end;
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

template<typename T>
bool hitCapsule(Ray<T> const& ray,
                Capsule<T> const& capsule,
                Hit<T>& hit)
{
    T rayVec = ray.end - ray.start;
    T capsuleVec = capsule.end - capsule.start;

    T splitPlane = rayVec % capsuleVec;

    float splitNum = splitPlane * (ray.start - capsule.start);
    float splitDen = splitPlane * splitPlane;

    float Dsqr = splitNum * splitNum / splitDen;
    float Rsqr = capsule.radius * capsule.radius;
    if (Dsqr > Rsqr) {
        return false;
    }

    T hitPlane = splitPlane % capsuleVec;

    float hitNum = hitPlane * (capsule.start - ray.start);
    float hitDen = hitPlane * rayVec;

    if (std::abs(hitDen) == 0.0f) {
        return false;
    }

    float H = std::sqrt(Rsqr - Dsqr);
    float t = (hitNum - H) / hitDen;
    T hitPoint = ray.start + rayVec * t;

    if (capsuleVec * (hitPoint - capsule.end) > 0.0f) {
        return hitSphere(ray, {capsule.end, capsule.radius}, hit);
    } else if (capsuleVec * (hitPoint - capsule.start) < 0.0f) {
        return hitSphere(ray, {capsule.start, capsule.radius}, hit);
    } else if (t < 0.0f || t > 1.0f) {
        return false;
    }

    T projected = capsuleVec * hitPoint * capsuleVec / (capsuleVec * capsuleVec);

    hit.t = t;
    hit.point = hitPoint;
    hit.normal = (hitPoint - projected).Normalize();

    return true;
}
