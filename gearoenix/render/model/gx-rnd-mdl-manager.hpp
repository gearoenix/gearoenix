#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "gx-rnd-mdl-model.hpp"
#include <string>

namespace gearoenix::render::model {
struct Manager {
protected:
    Manager();

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    virtual ~Manager();
    [[nodiscard]] static Manager& get();

    [[nodiscard]] virtual core::ecs::EntityPtr build(std::string&& name, core::ecs::Entity* parent, meshes_set_t&& meshes, bool is_transformable);
};
}