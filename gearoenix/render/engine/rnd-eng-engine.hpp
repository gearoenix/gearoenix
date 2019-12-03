#ifndef GEAROENIX_RENDER_ENGINE_ENGINE_HPP
#define GEAROENIX_RENDER_ENGINE_ENGINE_HPP
#include "../../core/cr-build-configuration.hpp"
#include "../../core/cr-static.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../texture/rnd-txt-format.hpp"
#include "../texture/rnd-txt-info.hpp"
#include "../texture/rnd-txt-sample.hpp"
#include "rnd-eng-limitations.hpp"
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
        class KernelWorkers;
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
        namespace node {
            class Node;
        }
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
        class Target;
    }
    namespace engine {
        class Engine {
            GX_GET_CVAL_PRT(Type, engine_type)
            GX_GET_CPTR_PRT(system::Application, system_application)
            GX_GET_UPTR_PRT(core::FunctionLoader, function_loader)
            GX_GET_UPTR_PRT(core::sync::KernelWorkers, kernels)
            GX_GET_UPTR_PRT(physics::Engine, physics_engine)
            GX_GET_UPTR_PRT(pipeline::Manager, pipeline_manager)
            GX_GET_UPTR_PRT(command::Manager, command_manager)
            GX_GET_UPTR_PRT(sampler::Manager, sampler_manager)
            GX_GET_UPTR_PRT(buffer::Manager, buffer_manager)
            GX_GET_CREF_PRT(std::shared_ptr<texture::Target>, main_render_target)
            GX_GET_CREF_PRT(Limitations, limitations)
            GX_GET_VAL_PRT(unsigned int, frames_count, 2)
            GX_GET_VAL_PRT(unsigned int, frame_number, 0)
            GX_GET_VAL_PRT(core::Real, delta_time, 0.0f)
            // It is not owned by engine and the user who had set this, must delete it.
            // In addition, this design is temporary and in next version of engine it is going to be changed.
            //
            // TODO: This is (design/mechanism) going to change in far future
            // game developer must keep all the render-trees by himself/herself
            // Then every thing based on the following must be chose by render-trees:
            //   - Type of object/scene (e.g. UI, GAME, MODEL, Widget, etc)
            //   - Tagging the objects or meshes that must be rendered by which render-tree
            // This is going to add a great flexibility in render engine
            //
            // The other way of accomplishing of this functionality is to create a render-tree
            // that is composed with several other render-trees
            GX_GETSET_PTR_PRT(graph::tree::Tree, render_tree)
        protected:
            std::chrono::time_point<std::chrono::high_resolution_clock> last_frame_time = std::chrono::high_resolution_clock::now();
            Engine(system::Application* system_application, Type engine_type) noexcept;

        public:
            virtual ~Engine() noexcept;
            virtual void update() noexcept;
            virtual void terminate() noexcept;
            [[nodiscard]] virtual std::shared_ptr<sync::Semaphore> create_semaphore() const noexcept = 0;
            /// Caller of this function must maintain the pointer to data until call of EndCaller.
            [[nodiscard]] virtual std::shared_ptr<texture::Texture2D> create_texture_2d(
                core::Id id,
                const void* data,
                texture::TextureFormat f,
                texture::SampleInfo s,
                unsigned int width,
                unsigned int height,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
                = 0;
            /// Caller of this function must maintain the pointer to data until call of EndCaller.
            [[nodiscard]] virtual std::shared_ptr<texture::Cube> create_texture_cube(
                core::Id id,
                const void* data,
                texture::TextureFormat f,
                texture::SampleInfo s,
                unsigned int aspect,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
                = 0;
            [[nodiscard]] virtual std::shared_ptr<texture::Target> create_render_target(
                core::Id id,
                const std::vector<texture::Info>& infos,
                unsigned int width,
                unsigned int height,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
                = 0;
            virtual void submit(
                std::size_t pres_count,
                const sync::Semaphore* const* pres,
                std::size_t cmds_count,
                const command::Buffer* const* cmds,
                std::size_t nxts_count,
                const sync::Semaphore* const* nxts) noexcept
                = 0;
        };
    }
}
}
#endif
