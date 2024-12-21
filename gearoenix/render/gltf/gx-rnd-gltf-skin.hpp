#pragma once

namespace gearoenix::render::gltf {
struct Context;
struct Skins final {
    const Context& context;

    explicit Skins(const Context& context);
    ~Skins();
    [[nodiscard]] bool is_skin(int node_index) const;
};
}
