#ifndef GEAROENIX_RENDER_MODEL_MODEL_HPP
#define GEAROENIX_RENDER_MODEL_MODEL_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-matrix.hpp"
#include "../../math/math-sphere.hpp"
#include "../../math/math-vector.hpp"
#include "../../physics/collider/phs-cld-collider.hpp"
#include "../../physics/phs-transformation.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "rnd-mdl-mesh.hpp"
#include "rnd-mdl-type.hpp"
#include "rnd-mdl-uniform.hpp"
#include <map>
#include <memory>
#include <vector>

namespace gearoenix::render {
namespace buffer {
    class FramedUniform;
}
namespace engine {
    class Engine;
}
namespace scene {
    class Scene;
}
namespace model {
    class Model : public core::asset::Asset {
	public:
		using MapMesh = std::map<core::Id, std::shared_ptr<Mesh>>;
		using MapModel = std::map<core::Id, std::shared_ptr<Model>>;

        GX_GET_CVAL_PRT(Type, model_type)
		GX_GET_UPTR_PRT(physics::collider::Collider, collider)
		GX_GET_UPTR_PRT(physics::Transformation, transformation)
		GX_GET_UCPTR_PRT(buffer::FramedUniform, uniform_buffers)
		GX_GET_VAL_PRT(core::State, shadowing, core::State::Unset)
		GX_GET_VAL_PRT(core::State, transparency, core::State::Unset)
		GX_GET_VAL_PRT(core::State, enability, core::State::Set)
		GX_GET_CREF_PRT(MapMesh, meshes)
        GX_GET_CREF_PRT(MapModel, children)
        GX_GET_CREF_PRT(math::Sphere, occlusion_sphere)
        GX_GET_PTR_PRT(Model, parent)
        GX_GET_PTR_PRT(scene::Scene, scene)
    protected:
        engine::Engine* const e;
        Uniform uniform;

        Model(
            core::Id my_id,
            Type t,
            physics::Transformation *transformation,
            system::stream::Stream* f,
            engine::Engine* e,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
        Model(
            core::Id my_id,
            Type t,
            physics::Transformation* transformation,
            engine::Engine* e,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

    public:
		virtual ~Model() noexcept = default;
        virtual void update() noexcept;
        void add_mesh(const std::shared_ptr<Mesh>& m) noexcept;
        void add_child(const std::shared_ptr<Model>& c) noexcept;
		void set_collider(std::unique_ptr<physics::collider::Collider> c) noexcept;
        const math::Mat4x4& get_model_matrix() const noexcept;
        void set_enability(core::State s) noexcept;
        virtual void set_scene(scene::Scene* s) noexcept;
		virtual bool get_dynamicity() const noexcept = 0;
    };
}
}
#endif
