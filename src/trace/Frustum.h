#pragma once

template<typename M, typename V, typename S>
class Frustum {
public:
    Frustum() {}
    Frustum(V const& origin, V const& forward, V const& left, V const& up, float znear, float zfar, float fov_x, float fov_y)
        : _origin(origin)
        , _znear(znear)
        , _zfar(zfar) {

        float dw = zfar * std::sin(0.5f * fov_x);
        float dh = zfar * std::sin(0.5f * fov_y);

        _forward = forward * zfar;
        _left = left * dw;
        _up = up * dh;
    }

    V const& Origin() const {
        return _origin;
    }

    V const& Forward() const {
        return _forward;
    }

    V const& Left() const {
        return _left;
    }
    
    V const& Up() const {
        return _up;
    }

    S const& Near() const {
        return _znear;
    }

    S const& Far() const {
        return _zfar;
    }

private:
    V _origin;
    V _forward; // forward vector * zfar
    V _left;    // left vector * far width / 2
    V _up;      // up vector * far height / 2
    S _znear;
    S _zfar;
};
