#ifndef GEAROENIX_RENDER_SCENE_SCENE_HPP
#define GEAROENIX_RENDER_SCENE_SCENE_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-scn-type.hpp"
#include "rnd-scn-uniform.hpp"
#include <map>
#include <memory>
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
    namespace buffer {
        class FramedUniform;
    }
    namespace camera {
        class Camera;
    }
    namespace engine {
        class Engine;
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
        protected:
            const std::shared_ptr<engine::Engine>& e;
            const Type::Id scene_type_id;
            const std::shared_ptr<buffer::FramedUniform> uniform_buffers;

            bool enabled = false;

            Uniform uniform;

            std::map<core::Id, std::shared_ptr<camera::Camera>> cameras;
            std::map<core::Id, std::shared_ptr<audio::Audio>> audios;
            std::map<core::Id, std::shared_ptr<light::Light>> lights;
            std::map<core::Id, std::shared_ptr<model::Model>> models;
            std::map<core::Id, std::shared_ptr<physics::constraint::Constraint>> constraints;
            std::shared_ptr<skybox::Skybox> skybox;

        public:
            /// It's going to read itself from gx3d stream.
            Scene(
                const core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& f,
                const std::shared_ptr<engine::Engine>& e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);

            Scene(
                const core::Id my_id,
                const std::shared_ptr<engine::Engine>& e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);

            virtual ~Scene();

            void enable();
            void disable();
            bool is_enabled() const;

#define GX_HELPER(x, c)                                             \
    void add_##x(const std::shared_ptr<c>& m);                      \
    const std::shared_ptr<c>& get_##x(const core::Id x##_id) const; \
    const std::map<core::Id, std::shared_ptr<c>>& get_##x##s() const

            GX_HELPER(camera, camera::Camera);
            GX_HELPER(audio, audio::Audio);
            GX_HELPER(light, light::Light);
            GX_HELPER(model, model::Model);
            GX_HELPER(constraint, physics::constraint::Constraint);

#undef GX_HELPER

            void set_skybox(const std::shared_ptr<skybox::Skybox>& s);
            const std::shared_ptr<model::Model>& get_skybox(const core::Id skybox_id) const;

            const std::shared_ptr<buffer::FramedUniform>& get_uniform_buffers() const;

            void update_uniform();
        };
    }
}
}
#endif
