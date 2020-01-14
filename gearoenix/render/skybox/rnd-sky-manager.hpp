#ifndef GEAROENIX_RENDER_SKYBOX_MANAGER_HPP
#define GEAROENIX_RENDER_SKYBOX_MANAGER_HPP
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-sky-skybox.hpp"
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
    create(core::sync::EndCaller<T>& c) noexcept;
};
}

template <typename T>
typename std::enable_if<std::is_base_of<gearoenix::render::skybox::Skybox, T>::value, std::shared_ptr<T>>::type
gearoenix::render::skybox::Manager::create(core::sync::EndCaller<T>& c) noexcept
{
    return std::shared_ptr<T>();
}

#endif
