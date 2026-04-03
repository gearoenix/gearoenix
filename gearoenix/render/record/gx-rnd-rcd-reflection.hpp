#pragma once
#include <boost/container/flat_map.hpp>

#include <vector>

namespace gearoenix::core::ecs {
struct Entity;
}

namespace gearoenix::physics::accelerator {
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
    static void update_models(const Data& d, physics::accelerator::Bvh& bvh, std::vector<Model>& models);
    void update_models(physics::accelerator::Bvh& bvh, std::vector<Model>& models);
    void update_static_models(Models& models);
    void update_dynamic_models(Models& models);
};
}