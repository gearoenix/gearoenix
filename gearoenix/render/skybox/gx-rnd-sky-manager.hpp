#ifndef GEAROENIX_RENDER_SKYBOX_MANAGER_HPP
#define GEAROENIX_RENDER_SKYBOX_MANAGER_HPP
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <map>
#include <memory>
#include <string>
#include <variant>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::platform::stream {
struct Path;
}

namespace gearoenix::render::texture {
struct Texture2D;
struct TextureCube;
}

namespace gearoenix::render::skybox {
struct Builder;
struct Manager {
    typedef std::variant<std::shared_ptr<texture::Texture2D>, std::shared_ptr<texture::TextureCube>> Texture;

protected:
    engine::Engine& e;

    explicit Manager(engine::Engine& e) noexcept;

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    virtual ~Manager() noexcept;

    [[nodiscard]] virtual std::shared_ptr<Builder> build(
        std::string&& name,
        Texture&& bound_texture,
        const core::sync::EndCaller& c) noexcept
        = 0;

    [[nodiscard]] virtual std::shared_ptr<Builder> build(
        std::string&& name,
        platform::stream::Path&& texture_path,
        const core::sync::EndCaller& c) noexcept;
};
}
#endif
