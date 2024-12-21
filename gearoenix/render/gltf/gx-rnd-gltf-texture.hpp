#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <memory>
#include <vector>

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::render::gltf {
struct Context;
struct Textures final {
    const Context& context;
    std::vector<std::shared_ptr<texture::Texture2D>> texture_2ds;

    Textures(const Context&);
    ~Textures();
    void load(int index, core::job::EndCaller<>&&);
    void load(core::job::EndCaller<>&&);
    [[nodiscard]] const std::shared_ptr<texture::Texture2D>& get(int index, const std::shared_ptr<texture::Texture2D>& default_value) const;
};
}