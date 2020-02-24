#ifndef GEAROENIX_RENDER_ENGINE_ENGINE_HPP
#define GEAROENIX_RENDER_ENGINE_ENGINE_HPP
#include "../../core/cr-build-configuration.hpp"
#include "../../core/cr-static.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../texture/rnd-txt-attachment.hpp"
#include "rnd-eng-limitations.hpp"
#include "rnd-eng-type.hpp"
#include <chrono>
#include <vector>

namespace gearoenix::core {
class FunctionLoader;
}

namespace gearoenix::core::asset {
class Asset;
}

namespace gearoenix::core::event {
class Event;
}

namespace gearoenix::core::sync {
class KernelWorkers;
}

namespace gearoenix::physics {
class Engine;
}

namespace gearoenix::system {
class Application;
}

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::command {
class Buffer;
class Manager;
}

namespace gearoenix::render::buffer {
class Mesh;
class Uniform;
class Manager;
}

namespace gearoenix::render::graph::node {
class Node;
}

namespace gearoenix::render::graph::tree {
class Tree;
}

namespace gearoenix::render::pipeline {
class Manager;
class Pipeline;
}

namespace gearoenix::render::sampler {
class Manager;
}

namespace gearoenix::render::scene {
class Scene;
}

namespace gearoenix::render::shader {
class Shader;
class Resources;
}

namespace gearoenix::render::sync {
class Semaphore;
}

namespace gearoenix::render::texture {
class TextureCube;
class Target;
class Texture2D;
}

namespace gearoenix::render::engine {
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
    GX_GET_VAL_PRT(unsigned int, frame_number_from_start, 0)
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
    std::mutex late_delete_assets_guard;
    std::vector<std::vector<std::shared_ptr<core::asset::Asset>>> late_delete_assets;
    std::size_t late_delete_index = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_frame_time = std::chrono::high_resolution_clock::now();
    Engine(system::Application* system_application, Type engine_type) noexcept;
    void do_late_delete() noexcept;

public:
    virtual ~Engine() noexcept;
    void late_delete(std::shared_ptr<core::asset::Asset> asset) noexcept;
    virtual void update() noexcept;
    virtual void terminate() noexcept;
    [[nodiscard]] virtual std::shared_ptr<sync::Semaphore> create_semaphore() const noexcept = 0;
    /// Data will be copied
    [[nodiscard]] virtual std::shared_ptr<texture::Texture2D> create_texture_2d(
        core::Id id,
        const void* data,
        const texture::TextureInfo& info,
        unsigned int img_width,
        unsigned int img_height,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept = 0;
    /// Data will be copied
    [[nodiscard]] virtual std::shared_ptr<texture::TextureCube> create_texture_cube(
        core::Id id,
        const void* data,
        const texture::TextureInfo& info,
        unsigned int aspect,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept = 0;
    [[nodiscard]] virtual std::shared_ptr<texture::Target> create_render_target(
        core::Id id,
        const std::vector<texture::AttachmentInfo>& infos,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept = 0;
    virtual void submit(
        std::size_t pres_count,
        const sync::Semaphore* const* pres,
        std::size_t cmds_count,
        const command::Buffer* const* cmds,
        std::size_t nxts_count,
        const sync::Semaphore* const* nxts) noexcept = 0;
};
}
#endif
