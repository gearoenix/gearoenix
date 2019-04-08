#ifndef GEAROENIX_RENDER_ENGINE_ENGINE_HPP
#define GEAROENIX_RENDER_ENGINE_ENGINE_HPP
#include "../../core/cr-build-configuration.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../texture/rnd-txt-format.hpp"
#include "../texture/rnd-txt-sample.hpp"
#include "rnd-eng-type.hpp"
#include <memory>
#include <vector>
namespace gearoenix {
	namespace core {
		class FunctionLoader;
		namespace event {
			class Event;
		}
		namespace sync {
			class QueuedSemaphore;
			class KernelWorker;
		}
	}
	namespace physics {
		class Engine;
	}
	namespace system {
		class Application;
		namespace stream {
			class Stream;
		}
	}
	namespace render {
		namespace command {
			class Buffer;
			class Manager;
		}
		namespace buffer {
			class Mesh;
			class Uniform;
			class Manager;
		}
		namespace graph {
			namespace tree {
				class Tree;
			}
		}
		namespace pipeline {
			class Manager;
			class Pipeline;
		}
		namespace sampler {
			class Manager;
		}
		namespace scene {
			class Scene;
		}
		namespace shader {
			class Shader;
			class Resources;
		}
		namespace sync {
			class Semaphore;
		}
		namespace texture {
			class Cube;
			class Target;
			class Texture2D;
		}
		namespace engine {
			class Engine {
			protected:
				unsigned int frames_count = 2;
				unsigned int frame_number = 0;
				const Type::Id engine_type_id;
				std::shared_ptr<system::Application> sysapp = nullptr;
				std::shared_ptr<core::FunctionLoader> fun_loader = nullptr;
				std::shared_ptr<physics::Engine> physics_engine = nullptr;
				std::shared_ptr<core::sync::KernelWorker> kernels = nullptr;
				std::shared_ptr<graph::tree::Tree> render_tree = nullptr;
				std::shared_ptr<texture::Target> main_render_target = nullptr;
				/// managers pointers are own only by this class
				std::shared_ptr<pipeline::Manager> pipeline_manager = nullptr;
				std::shared_ptr<command::Manager> command_manager = nullptr;
				std::shared_ptr<sampler::Manager> sampler_manager = nullptr;
				std::shared_ptr<buffer::Manager> buffer_manager = nullptr;
				Engine(const std::shared_ptr<system::Application>& system_application, const Type::Id engine_type_id);
			public:
				virtual ~Engine();
				virtual void update();
				virtual void terminate();
				virtual sync::Semaphore* create_semaphore() = 0;
				/// Caller of this function must maintain the pointer to data untill call of EndCaller.
				virtual texture::Texture2D* create_texture_2d(
					const core::Id id,
					const void *data,
					const texture::TextureFormat::Id f,
					const texture::SampleInfo s,
					const unsigned int width,
					const unsigned int heigt,
					const core::sync::EndCaller<core::sync::EndCallerIgnore> &call) = 0;
				virtual void submit(const std::vector<std::shared_ptr<sync::Semaphore>>& p, const std::shared_ptr<command::Buffer>& c, const std::shared_ptr<sync::Semaphore>& n) = 0;
				/// TODO: This is (design/mechanism) going to change in far future
				/// game developer must keep all the render-trees by himself/herself
				/// Then every thing based on the following must be choosed by render-trees:
				///   - Type of object/scene (e.g. UI, GAME, MODEL, Widget, etc)
				///   - Tagging the objects or meshes that must be rendered by which render-tree
				/// This is going to add a great flexibility in rendering of engine
				///
				/// The other way of accomplishing of this functionallity is to create a render-tree 
				/// that is composed with several other render-trees
				virtual void set_render_tree(const std::shared_ptr<graph::tree::Tree> &tree);

				const std::shared_ptr<system::Application> &get_system_application() const;
				const std::shared_ptr<core::FunctionLoader> &get_function_loader() const;
				const std::shared_ptr<physics::Engine> &get_physics_engine() const;
				const std::shared_ptr<pipeline::Manager> &get_pipeline_manager() const;
				const std::shared_ptr<command::Manager> &get_command_manager() const;
				const std::shared_ptr<buffer::Manager> &get_buffer_manager() const;
				const std::shared_ptr<core::sync::KernelWorker> &get_kernels() const;
				const std::shared_ptr<texture::Target> &get_main_render_target() const;
				Type::Id get_engine_type_id() const;
				unsigned int get_frame_number() const;
				unsigned int get_frames_count() const;
			};
		}
	}
}
#endif
