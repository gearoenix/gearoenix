#ifndef GEAROENIX_CORE_ASSET_MANAGER_HPP
#define GEAROENIX_CORE_ASSET_MANAGER_HPP
#include "../cr-end-caller.hpp"
#include "../cr-types.hpp"
#include <memory>
#include <string>
namespace gearoenix {
namespace audio {
    class Audio;
}
namespace physics {
    namespace constraint {
        class Constraint;
    }
}
namespace render {
    class Engine;
    namespace camera {
        class Camera;
    }
    namespace light {
        class Light;
    }
    namespace mesh {
        class Mesh;
    }
    namespace model {
        class Model;
    }
    namespace scene {
        class Scene;
    }
    namespace shader {
        class Shader;
    }
    namespace texture {
        class Texture;
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
        class Cacher;
        namespace file {
            class File;
            class Sparse;
        }
    }
    namespace asset {
        class Manager {
        private:
            system::Application* sys_app = nullptr;
            render::Engine* render_engine = nullptr;
            system::stream::Asset* file = nullptr;
            cache::Cacher* shaders = nullptr;
            cache::file::File* cameras = nullptr;
            cache::file::File* audios = nullptr;
            cache::file::File* lights = nullptr;
            cache::file::File* textures = nullptr;
            cache::file::File* meshes = nullptr;
            cache::file::File* models = nullptr;
            cache::file::File* constraints = nullptr;
            cache::file::File* scenes = nullptr;

        public:
            Manager(system::Application* sys_app, const std::string& file);
            ~Manager();
            void initialize();
            system::stream::Asset* get_file();
            std::shared_ptr<render::shader::Shader> get_shader(Id id, EndCaller<render::shader::Shader> end);
            std::shared_ptr<render::shader::Shader> get_cached_shader(Id id) const;
            std::shared_ptr<render::camera::Camera> get_camera(Id id);
            std::shared_ptr<render::camera::Camera> get_cached_camera(Id id) const;
            std::shared_ptr<audio::Audio> get_audio(Id id);
            std::shared_ptr<audio::Audio> get_cached_audio(Id id);
            std::shared_ptr<render::light::Light> get_light(Id id);
            std::shared_ptr<render::light::Light> get_cached_light(Id id) const;
            std::shared_ptr<render::texture::Texture> get_texture(Id id, EndCaller<render::texture::Texture> end);
            std::shared_ptr<render::texture::Texture> get_cached_texture(Id id) const;
            std::shared_ptr<render::mesh::Mesh> get_mesh(Id id, EndCaller<render::mesh::Mesh> e);
            std::shared_ptr<render::mesh::Mesh> get_cached_mesh(Id id) const;
            std::shared_ptr<render::model::Model> get_model(Id id, EndCaller<render::model::Model> e);
            std::shared_ptr<render::model::Model> get_cached_model(Id id) const;
            std::shared_ptr<physics::constraint::Constraint> get_constriants(Id id, EndCaller<physics::constraint::Constraint> e);
            std::shared_ptr<physics::constraint::Constraint> get_cached_constraints(Id id) const;
            std::shared_ptr<render::scene::Scene> get_scene(Id id, EndCaller<render::scene::Scene> e);
            std::shared_ptr<render::scene::Scene> get_cached_scene(Id id) const;
        };
    } // namespace asset
} // namespace core
} // namespace gearoenix
#endif // GEAROENIX_CORE_ASSET_MANAGER_HPP
