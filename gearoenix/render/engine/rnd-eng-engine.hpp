#ifndef GEAROENIX_RENDER_ENGINE_ENGINE_HPP
#define GEAROENIX_RENDER_ENGINE_ENGINE_HPP
#include "../../core/cr-build-configuration.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../texture/rnd-txt-format.hpp"
#include "../texture/rnd-txt-sample.hpp"
#include "rnd-eng-type.hpp"
#include <chrono>
#include <vector>
namespace gearoenix {
namespace core {
    class FunctionLoader;
    namespace event {
        class Event;
    }
    namespace sync {
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
            core::Real delta_time = 0.0f;
            std::chrono::time_point<std::chrono::high_resolution_clock> last_frame_time = std::chrono::high_resolution_clock::now();

			system::Application *sysapp = nullptr;
            core::FunctionLoader *fun_loader = nullptr;
            physics::Engine *physics_engine = nullptr;
            core::sync::KernelWorker *kernels = nullptr;
            texture::Target *main_render_target = nullptr;

            pipeline::Manager *pipeline_manager = nullptr;
            command::Manager *command_manager = nullptr;
            sampler::Manager *sampler_manager = nullptr;
            buffer::Manager *buffer_manager = nullptr;
			/// It is not owned by engine and the user who has set this must delete it.
			/// In addition, this design is temporary and in next version of engine it is going to be changed.
            graph::tree::Tree *render_tree = nullptr;

            Engine(system::Application *system_application, const Type::Id engine_type_id) noexcept;

        public:
            virtual ~Engine() noexcept;
            virtual void update() noexcept;
            virtual void terminate() noexcept;
            virtual sync::Semaphore* create_semaphore() const noexcept = 0;
            /// Caller of this function must maintain the pointer to data untill call of EndCaller.
            virtual texture::Texture2D* create_texture_2d(
                const core::Id id,
                const void* data,
                const texture::TextureFormat::Id f,
                const texture::SampleInfo s,
                const unsigned int width,
                const unsigned int heigt,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
                = 0;
            /// Caller of this function must maintain the pointer to data untill call of EndCaller.
            virtual texture::Cube* create_texture_cube(
                const core::Id id,
                const void* data,
                const texture::TextureFormat::Id f,
                const texture::SampleInfo s,
                const unsigned int aspect,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
                = 0;
            virtual void submit(
				const std::size_t pres_count,
                const sync::Semaphore*const*const pres,
				const std::size_t cmds_count,
                const command::Buffer*const*const cmds,
				const std::size_t nxts_count,
                const sync::Semaphore*const*const nxts) noexcept
                = 0;
            /// TODO: This is (design/mechanism) going to change in far future
            /// game developer must keep all the render-trees by himself/herself
            /// Then every thing based on the following must be choosed by render-trees:
            ///   - Type of object/scene (e.g. UI, GAME, MODEL, Widget, etc)
            ///   - Tagging the objects or meshes that must be rendered by which render-tree
            /// This is going to add a great flexibility in rendering of engine
            ///
            /// The other way of accomplishing of this functionallity is to create a render-tree
            /// that is composed with several other render-trees
            virtual void set_render_tree(graph::tree::Tree* tree) noexcept;

			const system::Application* get_system_application() const noexcept;
			system::Application* get_system_application() noexcept;

			const core::FunctionLoader* get_function_loader() const noexcept;
			core::FunctionLoader *get_function_loader() noexcept;

			const physics::Engine* get_physics_engine() const noexcept;
			physics::Engine* get_physics_engine() noexcept;

			const pipeline::Manager* get_pipeline_manager() const noexcept;
			pipeline::Manager* get_pipeline_manager() noexcept;

			const command::Manager* get_command_manager() const noexcept;
			command::Manager* get_command_manager() noexcept;

			const buffer::Manager* get_buffer_manager() const noexcept;
			buffer::Manager* get_buffer_manager() noexcept;

			const core::sync::KernelWorker* get_kernels() const noexcept;
			core::sync::KernelWorker* get_kernels() noexcept;

			const texture::Target* get_main_render_target() const noexcept;
			texture::Target* get_main_render_target() noexcept;

            Type::Id get_engine_type_id() const noexcept;
            unsigned int get_frame_number() const noexcept;
            unsigned int get_frames_count() const noexcept;
            core::Real get_delta_time() const noexcept;
        };
    }
}
}
#endif
