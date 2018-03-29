#ifndef GEAROEMIX_RENDER_SCENE_SCENE_HPP
#define GEAROEMIX_RENDER_SCENE_SCENE_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-vector.hpp"
#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <vector>

namespace gearoenix {
namespace audio {
    class Audio;
}
namespace core {
    namespace event {
        class Event;
    }
}
namespace physics {
    class Kernel;
    namespace body {
        class Body;
        class Rigid;
    }
    namespace constraint {
        class Constraint;
    }
}
namespace system {
    class File;
}
namespace render {
    class Engine;
    namespace camera {
        class Camera;
    }
    namespace light {
        class Light;
        class Sun;
    }
    namespace material {
        class Material;
    }
    namespace mesh {
        class Mesh;
    }
    namespace model {
        class Model;
    }
    namespace skybox {
        class Skybox;
    }
    namespace texture {
        class Texture2D;
    }
    namespace scene {
        class Scene : public core::asset::Asset {
            friend class physics::Kernel;

        public:
            typedef enum : core::Id {
                GAME = 1,
                UI = 2,
            } SceneType;

        protected:
            const SceneType scene_type;
            std::map<core::Id, std::shared_ptr<camera::Camera>> cameras;
            std::map<core::Id, std::shared_ptr<audio::Audio>> audios;
            std::map<core::Id, std::shared_ptr<light::Light>> lights;
            std::map<core::Id, std::shared_ptr<model::Model>> root_models;
            std::map<core::Id, std::shared_ptr<physics::body::Body>> root_bodies;
            std::shared_ptr<skybox::Skybox> skybox;
            std::map<core::Id, std::shared_ptr<physics::constraint::Constraint>> root_constraints;
            bool all_models_needs_cleaning = false;
            std::map<core::Id, std::weak_ptr<model::Model>> all_models;
            std::map<core::Id, std::weak_ptr<physics::body::Body>> all_bodies;
            // shadow_caster_shader_id -> model_id -> mesh_id
            std::map<core::Id, std::map<core::Id, std::set<core::Id>>> shadow_caster_models;
            // shader_id -> model_id -> mesh_id
            std::map<core::Id, std::map<core::Id, std::set<core::Id>>> opaque_models;
            // model_id -> mesh_id
            std::map<core::Id, std::set<core::Id>> transparent_models;
            core::Id cam_id = 0;
            core::Id sun_id = 0;
            bool renderable = false;
            bool ambient_light_changed = true;
            math::Vec3 ambient_light = math::Vec3(0.2f, 0.2f, 0.2f);
            Engine* const render_engine;
            Scene(core::Id my_id, SceneType t, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c);

        public:
            void cast_shadow();
            void draw_sky();
            void draw(texture::Texture2D* shadow_texture);
            virtual ~Scene();
            static Scene* read(core::Id my_id, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            const std::map<core::Id, std::weak_ptr<model::Model>>& get_all_models() const;
            const std::map<core::Id, std::shared_ptr<physics::constraint::Constraint>>& get_all_root_constraints() const;
            const std::shared_ptr<camera::Camera>& get_current_camera() const;
            const math::Vec3& get_ambient_light() const;
            bool get_ambient_light_changed() const;
            const light::Sun* get_sun() const;
            void set_renderable(bool);
            void clean();
            virtual void on_event(core::event::Event& e);
            bool is_renderable() const;
            void add_mesh(core::Id mesh_id, core::Id model_id, std::shared_ptr<material::Material> mat, std::shared_ptr<material::Material> dp);
            void add_model(core::Id model_id, core::sync::EndCaller<core::sync::EndCallerIgnore> c = core::sync::EndCaller<core::sync::EndCallerIgnore>([](std::shared_ptr<core::sync::EndCallerIgnore>) -> void {}));
            void add_model(const std::shared_ptr<model::Model>& m);
            std::weak_ptr<model::Model> get_model(core::Id model_id);
            void add_constraint(const std::shared_ptr<physics::constraint::Constraint>& cns);
            void add_body(const std::shared_ptr<physics::body::Body>& b);
        };
    }
}
}
#endif
