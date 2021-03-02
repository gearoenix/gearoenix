#ifndef GEAROENIX_RENDER_ENGINE_ENGINE_HPP
#define GEAROENIX_RENDER_ENGINE_ENGINE_HPP
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-vertex.hpp"
#include "../gx-rnd-runtime-configuration.hpp"
#include "../texture/gx-rnd-txt-attachment.hpp"
#include "gx-rnd-eng-limitations.hpp"
#include "gx-rnd-eng-type.hpp"
#include <chrono>
#include <memory>
#include <set>
#include <vector>

namespace gearoenix::core {
struct FunctionLoader;
}

namespace gearoenix::core::ecs {
struct World;
}

namespace gearoenix::physics {
struct Engine;
}

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::scene {
struct Manager;
}

namespace gearoenix::render::engine {
struct Engine {
    GX_GET_CVAL_PRT(Type, engine_type)
    GX_GET_CRRF_PRT(platform::Application, platform_application)
    // GX_GET_UPTR_PRT(physics::Engine, physics_engine)
    GX_GET_CREF_PRT(Limitations, limitations)
    GX_GET_VAL_PRT(unsigned int, frames_count, 2)
    GX_GET_VAL_PRT(unsigned int, frame_number, 0)
    GX_GET_VAL_PRT(unsigned int, next_frame_number, 1)
    GX_GET_VAL_PRT(unsigned int, previous_frame_number, 1)
    GX_GET_VAL_PRT(unsigned int, frame_number_from_start, static_cast<decltype(frame_number)>(-1))
    GX_GET_VAL_PRT(double, delta_time, 0.0f)
    GX_GET_UPTR_PRT(scene::Manager, scene_manager)
    GX_GET_UPTR_PRT(core::ecs::World, world)

protected:
    std::chrono::time_point<std::chrono::high_resolution_clock> last_frame_time = std::chrono::high_resolution_clock::now();
    Engine(Type engine_type, const platform::Application& platform_application) noexcept;

public:
    [[nodiscard]] static std::set<Type> get_available_engines() noexcept;
    [[nodiscard]] static std::unique_ptr<Engine> construct(const platform::Application& platform_application) noexcept;
    virtual ~Engine() noexcept;
    virtual void start_frame() noexcept;
    virtual void end_frame() noexcept;
    virtual void upload_imgui_fonts() noexcept = 0;
    virtual void create_mesh(
        const std::string& name,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        core::sync::EndCaller<mesh::Mesh>& c) noexcept = 0;
};
}
#endif
