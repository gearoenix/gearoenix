#ifndef GEAROENIX_MATH_QUATERNION_HPP
#define GEAROENIX_MATH_QUATERNION_HPP
#include "../core/gx-cr-types.hpp"
#include "gx-math-matrix-4d.hpp"
#include <ostream>

namespace gearoenix::math {
template <typename Element>
struct Quat {
    Element x, y, z, w;

    constexpr explicit Quat(const Element e = static_cast<Element>(0)) noexcept
        : x(e)
        , y(e)
        , z(e)
        , w(e)
    {
    }

    constexpr Quat(const Element x, const Element y, const Element z, const Element w) noexcept
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }

    constexpr Mat4x4<Element> to_mat() const noexcept
    {
        const Element xx = x * x;
        const Element xy = x * y;
        const Element xz = x * z;
        const Element xw = x * w;
        const Element yy = y * y;
        const Element yz = y * z;
        const Element yw = y * w;
        const Element zz = z * z;
        const Element zw = z * w;
        Mat4x4<Element> m;
        m.data[0][0] = static_cast<Element>(1) - static_cast<Element>(2) * (yy + zz);
        m.data[0][1] = static_cast<Element>(2) * (xy + zw);
        m.data[0][2] = static_cast<Element>(2) * (xz - yw);
        m.data[1][0] = static_cast<Element>(2) * (xy - zw);
        m.data[1][1] = static_cast<Element>(1) - static_cast<Element>(2) * (xx + zz);
        m.data[1][2] = static_cast<Element>(2) * (yz + xw);
        m.data[2][0] = static_cast<Element>(2) * (xz + yw);
        m.data[2][1] = static_cast<Element>(2) * (yz - xw);
        m.data[2][2] = static_cast<Element>(1) - static_cast<Element>(2) * (xx + yy);
        return m;
    }

    friend std::ostream& operator<<(std::ostream& os, const Quat& q)
    {
        os << "\"Quat\" { \"x\": \"" << q.x << "\", \"y\": \"" << q.y << "\", \"z\": \"" << q.z << "\", \"w\": \"" << q.w << "\" }";
        return os;
    }
};
}
#endif
