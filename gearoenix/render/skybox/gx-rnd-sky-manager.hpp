#ifndef GEAROENIX_RENDER_SKYBOX_MANAGER_HPP
#define GEAROENIX_RENDER_SKYBOX_MANAGER_HPP
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../core/cache/gx-cr-cache-file.hpp"
#include "../../core/gx-cr-types.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "gx-rnd-sky-skybox.hpp"
#include <memory>

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::skybox {
class Manager {
protected:
    engine::Engine* const e;
    core::cache::File<Skybox> cache;

public:
    Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* e) noexcept;
    ~Manager() noexcept;
    [[nodiscard]] std::shared_ptr<Skybox> get_gx3d(core::Id mid, core::sync::EndCaller<Skybox>& c) noexcept;
    template <typename T>
    [[nodiscard]] typename std::enable_if<std::is_base_of<Skybox, T>::value, std::shared_ptr<T>>::type
    create(std::string name, core::sync::EndCaller<T>& c) noexcept;
};
}

template <typename T>
typename std::enable_if<std::is_base_of<gearoenix::render::skybox::Skybox, T>::value, std::shared_ptr<T>>::type
gearoenix::render::skybox::Manager::create(std::string name, core::sync::EndCaller<T>& c) noexcept
{
    const core::Id id = core::asset::Manager::create_id();
    const core::sync::EndCaller<core::sync::EndCallerIgnore> call([c] {});
    const std::shared_ptr<T> result(new T(id, std::move(name), e, call));
    c.set_data(result);
    cache.get_cacher().get_cacheds()[id] = result;
    return result;
}

#endif
