#ifndef GEAROENIX_RENDER_ENGINE_ENGINE_HPP
#define GEAROENIX_RENDER_ENGINE_ENGINE_HPP
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
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

namespace gearoenix::core::asset {
struct Asset;
}

namespace gearoenix::core::event {
struct Event;
}

namespace gearoenix::core::sync {
struct KernelWorkers;
struct UpdateManager;
}

namespace gearoenix::physics {
struct Engine;
}

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::scene {
struct Scene;
}

namespace gearoenix::render::texture {
struct TextureCube;
struct Target;
struct Texture2D;
}

namespace gearoenix::render::engine {
struct Engine {
    GX_GET_CVAL_PRT(Type, engine_type)
    GX_GET_CRRF_PRT(platform::Application, platform_application)
    // GX_GET_UPTR_PRT(core::FunctionLoader, function_loader)
    // GX_GET_UPTR_PRT(physics::Engine, physics_engine)
    GX_GET_CREF_PRT(Limitations, limitations)
    GX_GET_VAL_PRT(unsigned int, frames_count, 2)
    GX_GET_VAL_PRT(unsigned int, frame_number, 0)
    GX_GET_VAL_PRT(unsigned int, next_frame_number, 1)
    GX_GET_VAL_PRT(unsigned int, previous_frame_number, 1)
    GX_GET_VAL_PRT(unsigned int, frame_number_from_start, static_cast<decltype(frame_number)>(-1))
    GX_GET_VAL_PRT(double, delta_time, 0.0f)

protected:
    //    GX_CREATE_GUARD(late_delete_assets)
    //    std::vector<std::vector<std::shared_ptr<core::asset::Asset>>> late_delete_assets;
    //    std::size_t late_delete_index = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_frame_time = std::chrono::high_resolution_clock::now();
    Engine(Type engine_type, const platform::Application& platform_application) noexcept;
    //    void do_late_delete() noexcept;

public:
    [[nodiscard]] static std::set<Type> get_available_engines() noexcept;
    [[nodiscard]] static std::unique_ptr<Engine> construct(const platform::Application& platform_application) noexcept;
    virtual ~Engine() noexcept;
    //    void late_delete(std::shared_ptr<core::asset::Asset> asset) noexcept;
    virtual void start_frame() noexcept;
    virtual void end_frame() noexcept;
    virtual void upload_imgui_fonts() noexcept = 0;
    //    [[nodiscard]] virtual std::shared_ptr<sync::Semaphore> create_semaphore() const noexcept = 0;
    /// Function to create texture in the render engine
    ///
    /// \param id is Asset-ID of the texture
    /// \param data will be moved
    /// \param info is texture creation information
    /// \param img_width is texture width
    /// \param img_height is texture height
    /// \param call is a callback object that will be called whenever the texture is ready to render.
    /// \return The returned texture is ready to use (not render) only in assignment in the load process.
    //    [[nodiscard]] virtual std::shared_ptr<texture::Texture2D> create_texture_2d(
    //        core::Id id,
    //        std::string name,
    //        std::vector<std::vector<std::uint8_t>> data,
    //        const texture::TextureInfo& info,
    //        std::size_t img_width,
    //        std::size_t img_height,
    //        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept = 0;
    /// Function to create cube-texture in the render engine
    ///
    /// \param id is Asset-ID of the texture
    /// \param data will be moved ant it's size must be exactly 6
    /// \param info is texture creation information
    /// \param aspect is texture aspect
    /// \param call is a callback object that will be called whenever the texture is ready to render
    /// \return The returned texture is ready to use (not render) only in assignment in the load process.
    //    [[nodiscard]] virtual std::shared_ptr<texture::TextureCube> create_texture_cube(
    //        core::Id id,
    //        std::string name,
    //        std::vector<std::vector<std::vector<std::uint8_t>>> data,
    //        const texture::TextureInfo& info,
    //        std::size_t aspect,
    //        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept = 0;
    //    [[nodiscard]] virtual std::shared_ptr<texture::Target> create_render_target(
    //        core::Id id,
    //        const std::vector<texture::AttachmentInfo>& infos,
    //        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept = 0;
    //    virtual void submit(
    //        std::size_t pres_count,
    //        const sync::Semaphore* const* pres,
    //        std::size_t cmds_count,
    //        const command::Buffer* const* cmds,
    //        std::size_t nxts_count,
    //        const sync::Semaphore* const* nxts) noexcept = 0;
};
}
#endif
