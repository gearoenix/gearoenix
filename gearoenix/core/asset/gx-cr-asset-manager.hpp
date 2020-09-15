#ifndef GEAROENIX_CORE_ASSET_MANAGER_HPP
#define GEAROENIX_CORE_ASSET_MANAGER_HPP
#include "../gx-cr-static.hpp"
#include "../gx-cr-types.hpp"
#include "../sync/gx-cr-sync-end-caller.hpp"
#include <memory>
#include <string>
#ifndef GX_THREAD_NOT_SUPPORTED
#include <atomic>
#endif

namespace gearoenix::audio {
class Manager;
}

namespace gearoenix::physics::constraint {
class Manager;
}

namespace gearoenix::render::camera {
class Manager;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::font {
class Manager;
}

namespace gearoenix::render::light {
class Manager;
}

namespace gearoenix::render::mesh {
class Manager;
}

namespace gearoenix::render::model {
class Manager;
}

namespace gearoenix::render::reflection {
class Manager;
}

namespace gearoenix::render::skybox {
class Manager;
}

namespace gearoenix::render::scene {
class Manager;
}

namespace gearoenix::render::texture {
class Manager;
}

namespace gearoenix::system {
class Application;
}

namespace gearoenix::system::stream {
class Asset;
}

namespace gearoenix::core::cache {
template <class T>
class File;
}

namespace gearoenix::core::asset {
class Manager {
    GX_GET_CPTR_PRV(system::Application, sys_app)
    GX_GET_CPTR_PRV(render::engine::Engine, render_engine)
    GX_GET_UCPTR_PRV(system::stream::Asset, file)
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
    Manager(system::Application* sys_app, const std::string& file) noexcept;
    ~Manager() noexcept;
    static Id create_id() noexcept;
};
}
#endif
