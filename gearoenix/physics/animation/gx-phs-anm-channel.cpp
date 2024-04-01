#include "gx-phs-anm-channel.hpp"
#include "gx-phs-anm-interpolation.hpp"

namespace gearoenix::physics::animation {

template <typename T>
static bool keyframe_is_interpolated(
    const std::pair<double, T>& ks,
    const std::pair<double, T>& km,
    const std::pair<double, T>& ke)
{
    const auto i = interpolate(ks, ke, km.first);
    return i.equal(get_key(km.second), 0.001);
}

template <typename T>
static bool optimise_keyframe(std::vector<std::pair<double, T>>& samples)
{
    auto iter1 = samples.begin();
    if (samples.end() == iter1)
        return false;
    auto iter2 = iter1 + 1;
    if (samples.end() == iter2)
        return false;
    auto iter3 = iter2 + 1;
    if (samples.end() == iter3)
        return false;

    bool optimised = false;
    while (samples.end() != iter3) {
        const auto b = keyframe_is_interpolated(*iter1, *iter2, *iter3);
        optimised |= b;
        if (b) {
            iter2 = samples.erase(iter2);
            if (samples.end() == iter2)
                return optimised;
            iter3 = iter2 + 1;
        } else {
            iter1 = iter2;
            iter2 = iter3;
            ++iter3;
        }
    }
    return optimised;
}
}

void gearoenix::physics::animation::BoneChannelBuilder::optimise()
{
    optimise_keyframe(scale_samples);
    optimise_keyframe(rotation_samples);
    optimise_keyframe(translation_samples);
}
