#ifndef GEAROEMIX_RENDER_SCENE_SCENE_HPP
#define GEAROEMIX_RENDER_SCENE_SCENE_HPP
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
		class Uniform;
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
	namespace pipeline {
		class Resource;
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
			const std::shared_ptr<engine::Engine> &e;
            const Type::Id scene_type_id;
			const std::shared_ptr<pipeline::Resource> pipeline_resource;

			bool renderable = false;
			
			Uniform uniform;
			std::shared_ptr<buffer::Uniform> uniform_buffers[GX_FRAMES_COUNT];

            std::map<core::Id, std::shared_ptr<camera::Camera>> cameras;
            std::map<core::Id, std::shared_ptr<audio::Audio>> audios;
            std::map<core::Id, std::shared_ptr<light::Light>> lights;
            std::map<core::Id, std::shared_ptr<model::Model>> models;
            std::map<core::Id, std::shared_ptr<physics::constraint::Constraint>> constraints;
            std::shared_ptr<skybox::Skybox> skybox;

        public:
			Scene(
				const core::Id my_id,
				const std::shared_ptr<engine::Engine> &e,
				const core::sync::EndCaller<core::sync::EndCallerIgnore> &c);
			Scene(
				const std::shared_ptr<engine::Engine> &e,
				const core::sync::EndCaller<core::sync::EndCallerIgnore> &c);
            virtual ~Scene();

			static Scene* read_gx3d(
				const core::Id my_id,
				const std::shared_ptr<system::stream::Stream>& f,
				const std::shared_ptr<engine::Engine> &e,
				const core::sync::EndCaller<core::sync::EndCallerIgnore> &c);
            
			void enable_rendering();
			void disable_rendering();
            bool is_renderable() const;

            void add_model(const std::shared_ptr<model::Model>& m);
			const std::shared_ptr<model::Model> &get_model(const core::Id model_id) const;
            const std::map<core::Id, std::shared_ptr<model::Model>>& get_models() const;

			void add_constraint(const std::shared_ptr<physics::constraint::Constraint>& cns);
			const std::shared_ptr<physics::constraint::Constraint> &get_constraint(const core::Id constraint_id) const;
        };
    }
}
}
#endif
