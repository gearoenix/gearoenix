#ifndef GEAROENIX_CORE_ASSET_MANAGER_HPP
#define GEAROENIX_CORE_ASSET_MANAGER_HPP
#include "../gx-cr-types.hpp"
#include "../macro/gx-cr-mcr-getter-setter.hpp"
#include "../sync/gx-cr-sync-end-caller.hpp"
#include <memory>
#include <string>
#ifndef GX_THREAD_NOT_SUPPORTED
#include <atomic>
#endif

namespace gearoenix::audio {
struct Manager;
}

namespace gearoenix::physics::constraint {
struct Manager;
}

namespace gearoenix::render::camera {
struct Manager;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::font {
struct Manager;
}

namespace gearoenix::render::light {
struct Manager;
}

namespace gearoenix::render::mesh {
struct Manager;
}

namespace gearoenix::render::model {
struct Manager;
}

namespace gearoenix::render::reflection {
struct Manager;
}

namespace gearoenix::render::skybox {
struct Manager;
}

namespace gearoenix::render::scene {
struct Manager;
}

namespace gearoenix::render::texture {
struct Manager;
}

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::platform::stream {
struct Asset;
}

namespace gearoenix::core::cache {
template <typename T>
struct File;
}

namespace gearoenix::core::asset {
struct Manager {
    GX_GET_CPTR_PRV(platform::Application, platform_application)
    GX_GET_CPTR_PRV(render::engine::Engine, render_engine)
    GX_GET_UCPTR_PRV(platform::stream::Asset, file)
    GX_GET_UPTR_PRV(render::camera::Manager, camera_manager)
    GX_GET_UPTR_PRV(audio::Manager, audio_manager)
    GX_GET_UPTR_PRV(render::light::Manager, light_manager)
    GX_GET_UPTR_PRV(render::texture::Manager, texture_manager)
    GX_GET_UPTR_PRV(render::font::Manager, font_manager)
    GX_GET_UPTR_PRV(render::mesh::Manager, mesh_manager)
    GX_GET_UPTR_PRV(render::model::Manager, model_manager)
    GX_GET_UPTR_PRV(render::reflection::Manager, reflection_manager)
    GX_GET_UPTR_PRV(render::skybox::Manager, skybox_manager)
    GX_GET_UPTR_PRV(physics::constraint::Manager, constraint_manager)
    GX_GET_UPTR_PRV(render::scene::Manager, scene_manager)
private:
#ifdef GX_THREAD_NOT_SUPPORTED
    static Id last_id;
#else
    static std::atomic<Id> last_id;
#endif

public:
    Manager(platform::Application* platform_application, const std::string& file) noexcept;
    ~Manager() noexcept;
    static Id create_id() noexcept;
};
}
#endif
