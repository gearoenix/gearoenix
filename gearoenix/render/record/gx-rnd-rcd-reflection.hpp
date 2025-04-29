#pragma once
#include <boost/container/flat_map.hpp>

namespace gearoenix::core::ecs {
struct Entity;
}

namespace gearoenix::physics::accelerator {
template <typename T>
struct Bvh;
}

namespace gearoenix::render::reflection {
struct Probe;
}

namespace gearoenix::render::record {
struct Model;
struct Models;

struct Reflections final {
    struct Data final {
        reflection::Probe* probe = nullptr;
    };

    boost::container::flat_map<core::ecs::Entity*, Data> data;

    void update(core::ecs::Entity* scene_entity);
    static void update_models(const Data& d, physics::accelerator::Bvh<Model>& bvh);
    void update_models(physics::accelerator::Bvh<Model>& bvh);
    void update_static_models(Models& models);
    void update_dynamic_models(Models& models);
};
}