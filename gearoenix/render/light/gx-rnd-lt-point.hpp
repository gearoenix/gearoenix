#pragma once
#include "gx-rnd-lt-light.hpp"

namespace gearoenix::render::light {
struct Point final : Light {
    constexpr static auto max_count = 16;
    constexpr static auto object_type_index = gearoenix_render_light_point_type_index;
    constexpr static std::array all_parent_object_type_indices { Light::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { Light::object_type_index };

    math::Vec3<float> position = { 0.0f, 0.0f, 0.0f };

    explicit Point(std::string&& name);
    ~Point() override;
};
}