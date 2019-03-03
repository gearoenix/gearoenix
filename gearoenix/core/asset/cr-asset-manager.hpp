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
    class Engine;
    namespace camera {
        class Manager;
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
        class File;
    }
}
namespace core {
    namespace cache {
        template<class T> class File;
    }
    namespace asset {
        class Manager {
        private:
            system::Application* sys_app = nullptr;
            render::Engine* render_engine = nullptr;
            std::shared_ptr<system::stream::Asset> file = nullptr;
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
            std::atomic<Id> last_id;

        public:
            Manager(system::Application* sys_app, const std::string& file);
            ~Manager();
            void initialize();
            void set_render_engine(render::Engine* rndeng);
            const std::shared_ptr<system::stream::File> &get_file() const;
            const std::shared_ptr<render::camera::Manager> &camera_manager() const;
            const std::shared_ptr<audio::Manager> &audio_manager() const;
            const std::shared_ptr<render::light::Manager> &light_manager() const;
            const std::shared_ptr<render::texture::Manager> &texture_manager() const;
            const std::shared_ptr<render::font::Manager> &font_manager() const;
            const std::shared_ptr<render::mesh::Manager> &mesh_manager() const;
            const std::shared_ptr<render::model::Manager> &model_manager() const;
            const std::shared_ptr<render::skybox::Manager> &skybox_manager() const;
            const std::shared_ptr<physics::constraint::Manager> &constraint_manager() const;
            const std::shared_ptr<render::scene::Manager> &scene_manager() const;
            Id create_id();
        };
    } // namespace asset
} // namespace core
} // namespace gearoenix
#endif // GEAROENIX_CORE_ASSET_MANAGER_HPP
