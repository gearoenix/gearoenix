#pragma once
#include <map>
#include <memory>

namespace gearoenix::physics::animation {
struct BoneChannelBuilder;
}

namespace gearoenix::render::gltf {
struct Context;
struct Animations final {
    const Context& context;
    std::map<int /*bone-node-index*/, std::unique_ptr<physics::animation::BoneChannelBuilder>> bones_channels;

    explicit Animations(const Context& context);
    ~Animations();
    void load();
};
}