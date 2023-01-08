#ifndef GEAROENIX_RENDER_SKYBOX_SKYBOX_HPP
#define GEAROENIX_RENDER_SKYBOX_SKYBOX_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <memory>
#include <variant>

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::texture {
struct Texture2D;
struct TextureCube;
}

namespace gearoenix::render::skybox {
struct Skybox final : public core::ecs::Component {
    typedef std::variant<std::shared_ptr<texture::Texture2D>, std::shared_ptr<texture::TextureCube>> Texture;

    GX_GET_CREF_PRT(std::shared_ptr<mesh::Mesh>, bound_mesh);
    GX_GET_CREF_PRT(Texture, bound_texture);
    GX_GETSET_VAL_PRT(core::ecs::entity_id_t, scene_id, 0);
    GX_GETSET_VAL_PRT(double, layer, 0.0);

public:
    Skybox(
        std::shared_ptr<mesh::Mesh>&& bound_mesh,
        Texture&& bound_texture) noexcept;
    ~Skybox() noexcept final;
    Skybox(Skybox&&) noexcept;
    [[nodiscard]] bool is_equirectangular() const noexcept { return bound_texture.index() == 0; }
    [[nodiscard]] bool is_cube() const noexcept { return bound_texture.index() == 1; }
};
}
#endif
