#pragma once
#include "gx-cr-ev-button.hpp"
#include "gx-cr-ev-touch.hpp"
#include <array>
#include <chrono>
#include <utility>

namespace gearoenix::core::event::gesture {
struct Click {
    GX_GET_REFC_PRV(Point2D, point);

public:
    explicit Click(const Point2D& p)
        : point(p)
    {
    }
};

struct Drag2D {
    GX_GET_REFC_PRV(Point2D, point);

public:
    explicit Drag2D(const Point2D& p)
        : point(p)
    {
    }
};

struct Scale {
    GX_GET_CARR_PRV(Point2D, points, 2);
    GX_GET_CVAL_PRV(double, start_scale);
    GX_GET_CVAL_PRV(double, previous_scale);
    GX_GET_CVAL_PRV(double, current_scale);
    GX_GET_CVAL_PRV(double, delta_previous_scale);
    GX_GET_CVAL_PRV(double, delta_start_scale);

public:
    Scale(const Point2D& p1, const Point2D& p2)
        : points { p1, p2 }
        , start_scale((p1.get_start_position() - p2.get_start_position()).length())
        , previous_scale((p1.get_previous_position() - p2.get_previous_position()).length())
        , current_scale((p1.get_current_position() - p2.get_current_position()).length())
        , delta_previous_scale(current_scale - previous_scale)
        , delta_start_scale(current_scale - start_scale)
    {
    }
};

struct MouseDrag {
    GX_GET_REFC_PRV(Drag2D, base);
    GX_GET_CVAL_PRV(platform::key::Id, key);

public:
    MouseDrag(Drag2D d, const platform::key::Id k)
        : base(std::move(d))
        , key(k)
    {
    }
};

struct TouchClick {
    GX_GET_REFC_PRV(Click, base);
    GX_GET_CVAL_PRV(platform::FingerId, finger_id);

public:
    TouchClick(Click c, const platform::FingerId fi)
        : base(std::move(c))
        , finger_id(fi)
    {
    }
};

struct TouchDrag {
    GX_GET_REFC_PRV(Drag2D, base);
    GX_GET_CVAL_PRV(platform::FingerId, finger_id);

public:
    TouchDrag(Drag2D d, const platform::FingerId fi)
        : base(std::move(d))
        , finger_id(fi)
    {
    }
};

struct TouchScale {
    GX_GET_REFC_PRV(Scale, base);
    GX_GET_CARR_PRV(platform::FingerId, finger_ids, 2);

public:
    TouchScale(Scale s, const platform::FingerId fi1, const platform::FingerId fi2)
        : base(std::move(s))
        , finger_ids { fi1, fi2 }
    {
    }
};
}