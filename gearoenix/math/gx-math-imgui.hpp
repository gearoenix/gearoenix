#ifndef GEAROENIX_MATH_IMGUI_HPP
#define GEAROENIX_MATH_IMGUI_HPP
#include "gx-math-vector-2d.hpp"
#include <imgui/imgui.h>

namespace gearoenix {
template <typename Element>
constexpr math::Vec2<Element> cast(const ImVec2& v)
{
    return { static_cast<Element>(v.x), static_cast<Element>(v.y) };
}

template <typename Element>
constexpr ImVec2 cast(const math::Vec2<Element>& v)
{
    return { static_cast<float>(v.x), static_cast<float>(v.y) };
}
}

#endif