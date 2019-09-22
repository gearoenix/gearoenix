#ifndef GEAROENIX_CORE_ASSET_MANAGER_HPP
#define GEAROENIX_CORE_ASSET_MANAGER_HPP
#include "../cr-static.hpp"
#include "../cr-types.hpp"
#include "../sync/cr-sync-end-caller.hpp"
#include <atomic>
#include <memory>
#include <string>
namespace gearoenix {
namespace audio {
    class Manager;
}
namespace physics {
    namespace constraint {
        class Manager;
    }
}
namespace render {
    namespace camera {
        class Manager;
    }
    namespace engine {
        class Engine;
    }
    namespace font {
        class Manager;
    }
    namespace light {
        class Manager;
    }
    namespace mesh {
        class Manager;
    }
    namespace model {
        class Manager;
    }
    namespace skybox {
        class Manager;
    }
    namespace scene {
        class Manager;
    }
    namespace texture {
        class Manager;
    }
}
namespace system {
    class Application;
    namespace stream {
        class Asset;
    }
}
namespace core {
    namespace cache {
        template <class T>
        class File;
    }
    namespace asset {
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
            GX_GET_UPTR_PRV(render::skybox::Manager, skybox_manager)
            GX_GET_UPTR_PRV(physics::constraint::Manager, constraint_manager)
            GX_GET_UPTR_PRV(render::scene::Manager, scene_manager)
        private:
            static std::atomic<Id> last_id;

        public:
            Manager(system::Application* sys_app, const std::string& file) noexcept;
			~Manager() noexcept;
            static Id create_id() noexcept;
        };
    } // namespace asset
} // namespace core
} // namespace gearoenix
#endif // GEAROENIX_CORE_ASSET_MANAGER_HPP
