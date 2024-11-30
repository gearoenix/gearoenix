#pragma once
#include "gx-phs-anm-keyframe.hpp"
#include <string>

namespace gearoenix::physics::animation {
struct Manager;

struct BoneChannelBuilder final {
    std::string target_bone;
    std::vector<std::pair<double, Keyframe<math::Vec3<double>>>> scale_samples;
    std::vector<std::pair<double, Keyframe<math::Quat<double>>>> rotation_samples;
    std::vector<std::pair<double, Keyframe<math::Vec3<double>>>> translation_samples;

    void optimise();
};

struct BoneChannel final {
    std::uint32_t target_bone_index = static_cast<std::uint32_t>(-1);
    std::uint32_t scale_samples_count = static_cast<std::uint32_t>(-1);
    std::uint32_t scale_samples_first_keyframe_index = static_cast<std::uint32_t>(-1);
    std::uint32_t scale_samples_end_keyframe_index = static_cast<std::uint32_t>(-1);
    std::uint32_t rotation_samples_count = static_cast<std::uint32_t>(-1);
    std::uint32_t rotation_samples_first_keyframe_index = static_cast<std::uint32_t>(-1);
    std::uint32_t rotation_samples_end_keyframe_index = static_cast<std::uint32_t>(-1);
    std::uint32_t translation_samples_count = static_cast<std::uint32_t>(-1);
    std::uint32_t translation_samples_first_keyframe_index = static_cast<std::uint32_t>(-1);
    std::uint32_t translation_samples_end_keyframe_index = static_cast<std::uint32_t>(-1);
};
}