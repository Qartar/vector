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
    S t;
    V point;
    V normal;
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

    V splitPlane = rayVec % capsuleVec;

    S splitNum = splitPlane * (ray.start - capsule.start);
    S splitDen = splitPlane * splitPlane;

    S Dsqr = splitNum * splitNum / splitDen;
    S Rsqr = capsule.radius * capsule.radius;
    if (Dsqr > Rsqr) {
        return false;
    }

    V hitPlane = splitPlane % capsuleVec;

    S hitNum = hitPlane * (capsule.start - ray.start);
    S hitDen = hitPlane * rayVec;

    if (abs(hitDen) == 0.0f) {
        return false;
    }

    S H = sqrt(Rsqr - Dsqr);
    S t = (hitNum - H) / hitDen;
    V hitPoint = ray.start + rayVec * t;

    if (capsuleVec * (hitPoint - capsule.end) > 0.0f) {
        return hitSphere(ray, {capsule.end, capsule.radius}, hit);
    } else if (capsuleVec * (hitPoint - capsule.start) < 0.0f) {
        return hitSphere(ray, {capsule.start, capsule.radius}, hit);
    } else if (t < 0.0f || t > 1.0f) {
        return false;
    }

    V projected = capsuleVec * hitPoint * capsuleVec / (capsuleVec * capsuleVec);

    hit.t = t;
    hit.point = hitPoint;
    hit.normal = (hitPoint - projected).Normalize();

    return true;
}
