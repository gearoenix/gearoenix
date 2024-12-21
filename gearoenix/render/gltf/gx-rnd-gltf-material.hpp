#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <memory>
#include <vector>

namespace gearoenix::render::material {
struct Pbr;
}

namespace gearoenix::render::gltf {
struct Context;
struct Materials final {
    const Context& context;
    std::vector<std::shared_ptr<material::Pbr>> materials;

    explicit Materials(const Context& context);
    ~Materials();
    void initialise(int) const;
    void load(int, core::job::EndCaller<>&&);
    void load(core::job::EndCaller<>&&);
};
}