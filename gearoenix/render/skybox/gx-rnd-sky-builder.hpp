#ifndef GEAROENIX_RENDER_SKYBOX_BUILDER_HPP
#define GEAROENIX_RENDER_SKYBOX_BUILDER_HPP
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <memory>
#include <string>
#include <variant>

namespace gearoenix::core::ecs {
struct EntitySharedBuilder;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct Texture2D;
struct TextureCube;
}

namespace gearoenix::render::skybox {
struct Builder {
    GX_GET_REFC_PRT(std::shared_ptr<core::ecs::EntitySharedBuilder>, entity_builder)

protected:
    Builder(
        engine::Engine& e,
        const std::string& name,
        std::variant<std::shared_ptr<texture::Texture2D>, std::shared_ptr<texture::TextureCube>>&& bound_texture,
        const core::sync::EndCallerIgnored& end_callback) noexcept;

public:
    Builder(Builder&&) = delete;
    Builder(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;
    Builder& operator=(const Builder&) = delete;
    virtual ~Builder() noexcept;
};
}
#endif
