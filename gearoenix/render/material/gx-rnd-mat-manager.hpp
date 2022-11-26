#ifndef GEAROENIX_RENDER_MATERIAL_MANAGER_HPP
#define GEAROENIX_RENDER_MATERIAL_MANAGER_HPP
#include "../../core/allocator/gx-cr-alc-static-block.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <map>
#include <string>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::material {
struct Material;
struct Pbr;
struct Unlit;
struct Sprite;
struct Manager {
    engine::Engine& e;

protected:
    core::allocator::StaticBlock<1024 * 1024> allocator;
    std::map<std::string, std::weak_ptr<Material>> materials;

    explicit Manager(engine::Engine&) noexcept;
    [[nodiscard]] virtual std::shared_ptr<Pbr> construct_pbr(const std::string& name, const core::sync::EndCaller& c) noexcept = 0;
    [[nodiscard]] virtual std::shared_ptr<Unlit> construct_unlit(const std::string& name, const core::sync::EndCaller& c) noexcept = 0;
    [[nodiscard]] virtual std::shared_ptr<Sprite> construct_sprite(const std::string& name, const core::sync::EndCaller& c) noexcept = 0;

public:
    virtual ~Manager() noexcept;
    [[nodiscard]] std::shared_ptr<Pbr> get_pbr(const std::string& name, const core::sync::EndCaller& c) noexcept;
    [[nodiscard]] std::shared_ptr<Unlit> get_unlit(const std::string& name, const core::sync::EndCaller& c) noexcept;
    [[nodiscard]] std::shared_ptr<Sprite> get_sprite(const std::string& name, const core::sync::EndCaller& c) noexcept;
};
}

#endif