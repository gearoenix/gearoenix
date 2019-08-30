#ifndef GEAROENIX_CORE_ASSET_MANAGER_HPP
#define GEAROENIX_CORE_ASSET_MANAGER_HPP
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
        private:
            const std::shared_ptr<system::Application> sys_app;
            const std::shared_ptr<render::engine::Engine> render_engine;
            const std::shared_ptr<system::stream::Asset> file;
            std::shared_ptr<render::camera::Manager> camera_manager = nullptr;
            std::shared_ptr<audio::Manager> audio_manager = nullptr;
            std::shared_ptr<render::light::Manager> light_manager = nullptr;
            std::shared_ptr<render::texture::Manager> texture_manager = nullptr;
            std::shared_ptr<render::font::Manager> font_manager = nullptr;
            std::shared_ptr<render::mesh::Manager> mesh_manager = nullptr;
            std::shared_ptr<render::model::Manager> model_manager = nullptr;
            std::shared_ptr<render::skybox::Manager> skybox_manager = nullptr;
            std::shared_ptr<physics::constraint::Manager> constraint_manager = nullptr;
            std::shared_ptr<render::scene::Manager> scene_manager = nullptr;
            static std::atomic<Id> last_id;

        public:
            Manager(system::Application * sys_app, const std::string& file) noexcept;
            const std::shared_ptr<system::stream::Asset>& get_file() const noexcept;
            const std::shared_ptr<render::camera::Manager>& get_camera_manager() const noexcept;
            const std::shared_ptr<audio::Manager>& get_audio_manager() const noexcept;
            const std::shared_ptr<render::light::Manager>& get_light_manager() const noexcept;
            const std::shared_ptr<render::texture::Manager>& get_texture_manager() const noexcept;
            const std::shared_ptr<render::font::Manager>& get_font_manager() const noexcept;
            const std::shared_ptr<render::mesh::Manager>& get_mesh_manager() const noexcept;
            const std::shared_ptr<render::model::Manager>& get_model_manager() const noexcept;
            const std::shared_ptr<render::skybox::Manager>& get_skybox_manager() const noexcept;
            const std::shared_ptr<physics::constraint::Manager>& get_constraint_manager() const noexcept;
            const std::shared_ptr<render::scene::Manager>& get_scene_manager() const noexcept;
            static Id create_id() noexcept;
        };
    } // namespace asset
} // namespace core
} // namespace gearoenix
#endif // GEAROENIX_CORE_ASSET_MANAGER_HPP
