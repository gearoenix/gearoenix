#ifndef GEAROENIX_PLATFORM_SDL2_APPLICATION_HPP
#define GEAROENIX_PLATFORM_SDL2_APPLICATION_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_SDL2
#include "../../render/gx-rnd-build-configuration.hpp"
#include "../gx-plt-application.hpp"
#include <SDL2/SDL.h>
#include <vector>

namespace gearoenix::platform {
struct Application final {
    GX_GET_VAL_PRV(SDL_Window*, window, nullptr);
    GX_GET_REF_PRV(BaseApplication, base);
#ifdef GX_RENDER_OPENGL_ENABLED
    GX_GET_VAL_PRV(int, gl_major, 0);
    GX_GET_VAL_PRV(int, gl_minor, 0);
    GX_GET_VAL_PRV(int, gl_samples, 0);
    GX_GET_VAL_PRV(int, gl_depth, 0);
    GX_GET_VAL_PRV(SDL_GLContext, gl_context, nullptr);
#endif
private:
    static void initialize_sdl();
    void initialize_screen();
    void initialize_window();
    void initialize_mouse();
    [[nodiscard]] bool create_window(std::uint32_t flags);
#ifdef GX_RENDER_OPENGL_ENABLED
    [[nodiscard]] bool create_gl_window(int mj, int mn, std::uint32_t flags);
    [[nodiscard]] bool create_gl_sample_window(int samples, std::uint32_t flags);
    [[nodiscard]] bool create_gl_depth_window(int depth, std::uint32_t flags);
#endif
    void fetch_events();

public:
    explicit Application(GX_MAIN_ENTRY_ARGS_DEF);
    ~Application();
    void run(core::Application* = nullptr);
    void loop();
    void set_caption(const std::string&);
    void set_window_fullscreen(bool b);
#ifdef GX_RENDER_VULKAN_ENABLED
    [[nodiscard]] std::vector<const char*> get_vulkan_extensions() const;
    void create_vulkan_surface(void* vulkan_instance, void* vulkan_data_ptr) const;
#endif
};
}
#endif
#endif