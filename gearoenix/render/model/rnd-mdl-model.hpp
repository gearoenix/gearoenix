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
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "rnd-mdl-mesh.hpp"
#include "rnd-mdl-transformation.hpp"
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
namespace model {
    class Model : public core::asset::Asset {
	public:
		using MapMesh = std::map<core::Id, std::shared_ptr<Mesh>>;
		using MapModel = std::map<core::Id, std::shared_ptr<Model>>;

		GX_GET_UPTR_PRT(physics::collider::Collider, collider)
		GX_GET_UCPTR_PRT(buffer::FramedUniform, uniform_buffers)
		GX_GET_UCPTR_PRT(Transformation, transformation)
		GX_GET_CREF_PRT(math::Sphere, occlusion_sphere)
		GX_GET_VAL_PRT(bool, has_shadow_caster, false)
		GX_GET_VAL_PRT(bool, has_transparent, false)
		GX_GETSET_VAL_PRT(core::State, dynamicity, core::State::Unset)
		GX_GETSET_VAL_PRT(core::State, enability, core::State::Set)
		GX_GET_CREF_PRT(MapMesh, meshes)
		GX_GET_CREF_PRT(MapModel, children)
    protected:
        engine::Engine* const e;
        Uniform uniform;
    public:
        Model(
            core::Id my_id,
            system::stream::Stream* f,
            engine::Engine* e,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
        Model(
            core::Id my_id,
            engine::Engine* e,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
        virtual void update_uniform() noexcept;
        void add_mesh(const std::shared_ptr<Mesh>& m) noexcept;
        void add_child(const std::shared_ptr<Model>& c) noexcept;
		void set_collider(std::unique_ptr<physics::collider::Collider> c) noexcept;
        const math::Mat4x4& get_model_matrix() const noexcept;
    };
}
}
#endif
