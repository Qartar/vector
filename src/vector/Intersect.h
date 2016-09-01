#pragma once

template<typename V, typename S>
struct Ray {
    V start;
    V end;
};

template<typename V, typename S>
struct Sphere {
    V origin;
    S radius;
};

template<typename V, typename S>
struct Capsule {
    V start;
    V end;
    S radius;
};

template<typename V, typename S>
struct Hit {
    V point;
    V normal;
    S t;
};

template<typename V, typename S>
bool hitSphere(Ray<V, S> const& ray,
               Sphere<V, S> const& sphere,
               Hit<V, S>& hit)
{
    V rayVec = ray.end - ray.start;
    V sphereVec = ray.start - sphere.origin;

    S A = rayVec * rayVec;
    S B = 2.0f * rayVec * sphereVec;
    S C = sphereVec * sphereVec - sphere.radius * sphere.radius;

    S Dsqr = B * B - 4.0f * A * C;

    if (Dsqr < 0.0f) {
        return false;
    }

    S D = sqrt(Dsqr);

    S t0 = 0.5f * (-B - D) / A;
    S t1 = 0.5f * (-B + D) / A;

    if (t0 >= 0.0f && t0 <= 1.0f) {
        hit.t = t0;
    } else if (t1 >= 0.0f && t1 <= 1.0f) {
        hit.t = t1;
    } else {
        return false;
    }

    hit.point = ray.start + rayVec * t0;
    hit.normal = t0 >= 0.0f ? V(hit.point - sphere.origin).Normalize()
                            : V(sphere.origin - hit.point).Normalize();
    return true;
}

template<typename V, typename S>
bool hitCapsule(Ray<V, S> const& ray,
                Capsule<V, S> const& capsule,
                Hit<V, S>& hit)
{
    V rayVec = ray.end - ray.start;
    V capsuleVec = capsule.end - capsule.start;

    V projVec = capsuleVec.Reject(rayVec);
    V capsuleOffset = capsuleVec.Reject(ray.start - capsule.start);

    S A = projVec * projVec;
    S B = 2.0f * projVec * capsuleOffset;
    S C = capsuleOffset * capsuleOffset - capsule.radius * capsule.radius;

    S Dsqr = B * B - 4.0f * A * C;

    if (Dsqr < 0.0f) {
        return false;
    }

    S D = sqrt(Dsqr);

    S t0 = 0.5f * (-B - D) / A;
    S t1 = 0.5f * (-B + D) / A;

    S t = t0 >= 0.0f ? t0 : t1;
    V hitPoint = ray.start + rayVec * t;

    if (capsuleVec * (hitPoint - capsule.end) > 0.0f) {
        return hitSphere(ray, {capsule.end, capsule.radius}, hit);
    } else if (capsuleVec * (hitPoint - capsule.start) < 0.0f) {
        return hitSphere(ray, {capsule.start, capsule.radius}, hit);
    } else if (t < 0.0f || t > 1.0f) {
        return false;
    }

    hit.t = t;
    hit.point = hitPoint;
    hit.normal = t0 >= 0.0f ? capsuleVec.Reject(hitPoint - capsule.start).Normalize()
                            : capsuleVec.Reject(capsule.start - hitPoint).Normalize();
    return true;
}
