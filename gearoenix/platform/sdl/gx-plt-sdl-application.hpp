#pragma once
#include "../gx-plt-build-configuration.hpp"
#if GX_PLATFORM_INTERFACE_SDL

#include "../../render/gx-rnd-build-configuration.hpp"
#include "../gx-plt-application.hpp"

#include <SDL3/SDL.h>

namespace gearoenix::platform {
struct Application final : core::Singleton<Application> {
    GX_GET_VAL_PRV(SDL_Window*, window, nullptr);
    GX_GET_REF_PRV(BaseApplication, base);
    char* current_clipboard = nullptr; //!< It is a temporary memory to keep the text alive

#if GX_RENDER_OPENGL_ENABLED
    GX_GET_VAL_PRV(int, gl_major, 0);
    GX_GET_VAL_PRV(int, gl_minor, 0);
    GX_GET_VAL_PRV(int, gl_samples, 0);
    GX_GET_VAL_PRV(int, gl_depth, 0);
    GX_GET_VAL_PRV(SDL_GLContext, gl_context, nullptr);
#endif

    static void initialize_sdl();
    void initialize_screen();
    void initialize_window();
    void initialize_mouse();
    [[nodiscard]] bool create_window(std::uint32_t flags);

#if GX_RENDER_OPENGL_ENABLED
    [[nodiscard]] bool create_gl_window(int mj, int mn, std::uint32_t flags);
    [[nodiscard]] bool create_gl_sample_window(int samples, std::uint32_t flags);
    [[nodiscard]] bool create_gl_depth_window(int depth, std::uint32_t flags);
#endif

    void fetch_events();

public:
    explicit Application(GX_MAIN_ENTRY_ARGS_DEF);
    ~Application() override;
    void run(core::Application* = nullptr);
    void loop();
    void set_caption(const std::string&);
    void set_window_fullscreen(bool b);
    void start_keyboard_capture();
    void set_text_input_area(int x, int y, int w, int h);
    void stop_keyboard_capture();
    [[nodiscard]] const char* get_clipboard();
    void set_clipboard(const char*);
    [[nodiscard]] bool open_url(const char *url);

#if GX_RENDER_VULKAN_ENABLED
    [[nodiscard]] std::vector<const char*> get_vulkan_extensions() const;
    void create_vulkan_surface(void* vulkan_instance, void* vulkan_data_ptr) const;
#endif
};
}
#endif