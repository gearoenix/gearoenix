#ifndef GEAROENIX_RENDER_SCENE_SCENE_HPP
#define GEAROENIX_RENDER_SCENE_SCENE_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../../core/cr-static.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-scn-type.hpp"
#include "rnd-scn-uniform.hpp"
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <vector>

namespace gearoenix {
namespace audio {
    class Audio;
}
namespace core::event {
    class Event;
}
namespace math {
    struct Ray3;
}
namespace physics {
    namespace accelerator {
        class Bvh;
    }
    namespace body {
        class Body;
        class Rigid;
    }
    namespace collider {
        class Collider;
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
        public:
            using MapAudio = std::map<core::Id, std::shared_ptr<audio::Audio>>;
            using MapCamera = std::map<core::Id, std::shared_ptr<camera::Camera>>;
            using MapConstraint = std::map<core::Id, std::shared_ptr<physics::constraint::Constraint>>;
            using MapLight = std::map<core::Id, std::shared_ptr<light::Light>>;
            using MapModel = std::map<core::Id, std::shared_ptr<model::Model>>;

            GX_GET_CVAL_PRT(Type, scene_type_id)
            GX_GETSET_VAL_PRT(core::Real, layer, 0.0f)
            GX_GETSET_VAL_PRT(bool, enability, false)
            GX_GETSET_VAL_PRT(bool, models_changed, false)
            GX_GET_UCPTR_PRT(buffer::FramedUniform, uniform_buffers)
            GX_GET_UCPTR_PRT(physics::accelerator::Bvh, static_accelerator)
            GX_GET_UCPTR_PRT(physics::accelerator::Bvh, dynamic_accelerator)
            GX_GET_CREF_PRT(MapAudio, audios)
            GX_GET_CREF_PRT(MapCamera, cameras)
            GX_GET_CREF_PRT(MapConstraint, constraints)
            GX_GET_CREF_PRT(MapLight, lights)
            GX_GET_CREF_PRT(MapModel, models)
            GX_GET_CREF_PRT(std::vector<physics::collider::Collider*>, static_colliders)
            GX_GET_CREF_PRT(std::vector<physics::collider::Collider*>, dynamic_colliders)
            GX_GET_CREF_PRT(std::shared_ptr<skybox::Skybox>, skybox)
        protected:
            engine::Engine* const e;
            Uniform uniform;

            Scene(
                core::Id my_id, Type t, engine::Engine* e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
            Scene(
                core::Id my_id, Type t, system::stream::Stream* f, engine::Engine* e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

        public:
            ~Scene() noexcept override;

#define GX_HELPER(x, c)                                         \
protected:                                                      \
    void scene_add_##x(const std::shared_ptr<c>& m) noexcept;   \
                                                                \
public:                                                         \
    virtual void add_##x(const std::shared_ptr<c>& m) noexcept; \
    const std::shared_ptr<c>& get_##x(core::Id x##_id) const noexcept;

            GX_HELPER(camera, camera::Camera)
            GX_HELPER(audio, audio::Audio)
            GX_HELPER(light, light::Light)
            GX_HELPER(model, model::Model)
            GX_HELPER(constraint, physics::constraint::Constraint)

#undef GX_HELPER

            // void set_skybox(std::shared_ptr<skybox::Skybox> s) noexcept;

            void update() noexcept;
            [[nodiscard]] std::optional<std::pair<core::Real, physics::collider::Collider*>> hit(const math::Ray3& r, core::Real d_min) const noexcept;
        };
    }
}
}
#endif
