#ifndef GEAROENIX_CORE_ASSET_MANAGER_HPP
#define GEAROENIX_CORE_ASSET_MANAGER_HPP
#include "../cr-types.hpp"
#include <memory>
#include <string>
namespace gearoenix {
namespace audio {
    class Audio;
}
namespace core {
    class EndCaller;
}
namespace render {
    class Engine;
    namespace camera {
        class Camera;
    }
    namespace light {
        class Light;
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
    class File;
}
namespace core {
    namespace cache {
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
            system::File* file = nullptr;
            cache::file::Sparse* shaders = nullptr;
            cache::file::File* cameras = nullptr;
            cache::file::File* audios = nullptr;
            cache::file::File* lights = nullptr;
            cache::file::File* textures = nullptr;
            cache::file::File* models = nullptr;
            cache::file::File* scenes = nullptr;

        public:
            Manager(system::Application* sys_app, const std::string& file);
            void initialize();
            std::shared_ptr<render::shader::Shader> get_shader(Id id);
            std::shared_ptr<render::shader::Shader> get_cached_shader(Id id) const;
            std::shared_ptr<render::camera::Camera> get_camera(Id id);
            std::shared_ptr<render::camera::Camera> get_cached_camera(Id id) const;
            std::shared_ptr<audio::Audio> get_audio(Id id);
            std::shared_ptr<audio::Audio> get_cached_audio(Id id);
            std::shared_ptr<render::light::Light> get_light(Id id);
            std::shared_ptr<render::light::Light> get_cached_light(Id id) const;
            std::shared_ptr<render::texture::Texture> get_texture(Id id, std::shared_ptr<EndCaller> end);
            std::shared_ptr<render::texture::Texture> get_cached_texture(Id id) const;
            std::shared_ptr<render::model::Model> get_model(Id id, std::shared_ptr<EndCaller> e);
            std::shared_ptr<render::model::Model> get_cached_model(Id id) const;
            std::shared_ptr<render::scene::Scene> get_scene(Id id, std::shared_ptr<EndCaller> e);
            std::shared_ptr<render::scene::Scene> get_cached_scene(Id id) const;
        };
    } // namespace asset
} // namespace core
} // namespace gearoenix
#endif // GEAROENIX_CORE_ASSET_MANAGER_HPP
