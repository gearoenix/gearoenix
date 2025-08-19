#pragma once
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "gx-rnd-mdl-model.hpp"
#include <string>

namespace gearoenix::render::model {
struct Manager : core::Singleton<Manager> {
protected:
    Manager();

public:
    ~Manager() override;

    [[nodiscard]] virtual core::ecs::EntityPtr build(std::string&& name, core::ecs::Entity* parent, meshes_set_t&& meshes, bool is_transformable);
};
}